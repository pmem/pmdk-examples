/*
Copyright (c) 2017-2019, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemkv.hpp>
#include <regex>
#include <string.h>
#include <string>
#include <string_view>
#include <vector>
#include <cassert>
#include <thread>

#define GBSIZE ((size_t) (1024 * 1024 * 1024))
#define POOLSIZE ((size_t) (GBSIZE * 16))

using namespace std;
using namespace pmem;
using namespace pmem::obj;
using namespace pmem::kv;


/* globals */
class root;
pool<root> pop;
int num_threads = 1;


/* persistent data structures */

class pattern
{
    private:
    struct file
    {
        char name[1024];
        time_t mtime;
        size_t num_lines;
        size_t raw_data_alloc;
        char raw_data[0];
    };

    persistent_ptr<pattern> next;
    persistent_ptr<char[]> patternstr;
    PMEMoid files_db_index;
    db *files_db;

    static void
    get_mtime_callback (const char *value, size_t vsize, void *args)
    {
        time_t *mtime = (time_t *) args;
        struct file *f = (struct file *) value;
        *mtime = f->mtime;
    }

    int
    get_mtime (const char *filename, time_t *mtime)
    {
        status ret;

        ret = files_db->get (filename, get_mtime_callback, (void *) mtime);
        if (ret == status::NOT_FOUND)
            return -1;
        assert (ret == status::OK);

        return 0; 
    }

    static int
    print_callback (const char *key, 
                    size_t skey, 
                    const char *value, 
                    size_t svalue,
                    void *args)
    {
        const struct file *f = (struct file *) value;
        size_t *cnt = (size_t *) args;

        if (f->num_lines > 0) {
            cout << "" << endl;
            cout << "###############" << endl;
            cout << "FILE = " << key << endl;
            cout << "###############" << endl;
            cout << "*** pattern present in " << f->num_lines;
            cout << " lines ***" << endl;

            for (size_t j = f->num_lines; j > 0; j--) {
                size_t *meta_array = (size_t *) &(f->raw_data[0]);
                const char *lines_array = &(f->raw_data[(f->num_lines)*2*sizeof(size_t)]);
                cout << meta_array[(j-1)*2] << ": ";
                cout << string (&(lines_array[meta_array[(j-1)*2+1]]));
                cout << endl;
            }

            *cnt += 1;
        }
        return 0;
    }

    void
    put_file (const struct file *f)
    {
        status ret;
        size_t obj_size = sizeof (struct file) + f->raw_data_alloc;

        string_view key   = string_view (f->name, strlen(f->name));
        string_view value = string_view ((char *) f, obj_size);

        ret = files_db->put (key, value);
        assert (ret == status::OK);
    }

    void
    create_new_line (struct file **f, string linestr, size_t linenum)
    {
        struct file *old_f = *f;
        struct file *new_f;
        size_t new_obj_size = 0;

        new_obj_size += sizeof (struct file);
        new_obj_size += old_f->raw_data_alloc;
        new_obj_size += sizeof (size_t) * 2;
        new_obj_size += linestr.length () + 1;

        // malloc mem for new object
        new_f = (struct file *) malloc (new_obj_size);
        if (new_f == nullptr)
            throw runtime_error ("Error calling malloc");

        // copying old data
        memcpy ((void *) new_f, 
                (const void *) old_f, 
                sizeof (struct file));

        memcpy ((void *) &(new_f->raw_data[0]),
                (const void *) &(old_f->raw_data[0]),
                sizeof (size_t) * 2 * old_f->num_lines);

        size_t size_old_lines_array 
                    = old_f->raw_data_alloc - (sizeof (size_t) * 2 * old_f->num_lines);
        char *old_lines_array 
                    = &(old_f->raw_data[old_f->num_lines * 2 * sizeof (size_t)]);

        new_f->num_lines += 1;
        char *lines_array  = &(new_f->raw_data[new_f->num_lines * 2 * sizeof (size_t)]);

        memcpy ((void *) lines_array, 
                (const void *) old_lines_array, 
                size_old_lines_array);

        // copying new data
        new_f->raw_data_alloc += (sizeof (size_t) * 2) + linestr.length () + 1;
        size_t *meta_array = (size_t *) &(new_f->raw_data[0]);

        strcpy (&(lines_array[size_old_lines_array]), linestr.c_str ());
        meta_array[(old_f->num_lines)*2]   = linenum;
        meta_array[(old_f->num_lines)*2+1] = size_old_lines_array+1;

        // change pointers
        free (old_f);
        *f = new_f;
    }

    public:    
    void
    process_file (const char *filename, time_t mtime)
    {
        time_t stored_mtime;
        int ret;
        
        ret = get_mtime (filename, &stored_mtime);
        if (!ret && difftime (mtime, stored_mtime) == 0) // up to date
            return;

        struct file *f = (struct file *) malloc (sizeof (struct file));
        if (f == nullptr)
            throw runtime_error ("Error calling malloc");

        strcpy (f->name, filename);
        f->mtime          = mtime;
        f->num_lines      = 0;
        f->raw_data_alloc = 0;           

        // scanning file for lines matching pattern
        ifstream fd (filename);
        string line;

        string pat ("(.*)(");
        pat += string (patternstr.get ()) + string (")(.*)");
        regex exp (pat);

        if (fd.is_open ()) {
            size_t linenum = 0;
            while (getline (fd, line)) {
                ++linenum;
                if (regex_match (line, exp)) 
                    create_new_line (&f, line, linenum);                
            }
        } else {
            char err[2048];
            sprintf (err, "unable to open file %s", filename);
            throw runtime_error (err);
        }

        put_file (f);

        free (f);
    }

    pattern (const char *str)
    {
        patternstr = make_persistent<char[]> (strlen (str) + 1);
        strcpy (patternstr.get (), str);
        files_db_index = OID_NULL;
    }

    void
    start (void)
    {
        config cfg;
        status ret;
        string objn = string("oid");

        ret = cfg.put_object (objn, &files_db_index, nullptr);
        assert (ret == status::OK);

        files_db = new db ();
        assert (files_db != nullptr);

        ret = files_db->open ("cmap", move (cfg));
        assert (ret == status::OK);
    }

    void
    stop (void)
    {
        delete files_db;
    }

    persistent_ptr<pattern>
    get_next (void)
    {
        return next;
    }

    void
    set_next (persistent_ptr<pattern> n)
    {
        next = n;
    }

    char *
    get_str (void)
    {
        return patternstr.get ();
    }

    void
    print (void)
    {
        status ret;
        size_t cnt = 0;

        cout << "PATTERN = " << patternstr.get () << endl;
        
        ret = files_db->get_all (print_callback, (void *) &cnt);
        assert (ret == status::OK);
        cout << "\n\t" << cnt << " files(s) scanned" << endl;
    }
};

class root
{
    private:
    p<size_t> npatterns;
    persistent_ptr<pattern> patterns;

    public:
    pattern *
    get_pattern (size_t index)
    {
        persistent_ptr<pattern> ptr = patterns;
        size_t i = 0;
        while (i < index && ptr != nullptr) {
            ptr = ptr->get_next ();
            i++;
        }
        return ptr.get ();
    }

    pattern *
    find_pattern (const char *patternstr)
    {
        persistent_ptr<pattern> ptr = patterns;
        while (ptr != nullptr) {
            if (strcmp (patternstr, ptr->get_str ()) == 0)
                return ptr.get ();
            ptr = ptr->get_next ();
        }
        return nullptr;
    }

    pattern *
    create_new_pattern (const char *patternstr)
    {
        transaction::run (pop, [&] {
            persistent_ptr<pattern> new_patterns 
                                = make_persistent<pattern> (patternstr);
            new_patterns->set_next (patterns);
            patterns = new_patterns;
            npatterns = npatterns + 1;
        });
        return patterns.get ();
    }

    void
    print_patterns (void)
    {
        cout << npatterns << " PATTERNS PROCESSED" << endl;
        for (size_t i = 0; i < npatterns; i++)
            cout << string (get_pattern (i)->get_str ()) << endl;
    }
};

/* auxiliary functions */

void
process_directory_recursive (const char *dirname,
                             vector<tuple<string, time_t>> &files)
{
    DIR *dp;
    struct dirent *dirp;
    struct stat st;
    char *entryname;

    if ((dp = opendir(dirname)) == NULL) {
        cout << "Error number = " << errno << " opening " << dirname << endl;
        throw runtime_error ("Error calling opendir()");
    }
    while ((dirp = readdir(dp)) != NULL) {
        entryname = (char *) malloc (strlen(dirname)+strlen(dirp->d_name)+2);
        if (entryname==NULL)
            throw runtime_error ("Error calling malloc()");
        sprintf (entryname, "%s/%s", dirname, dirp->d_name);

        stat (entryname, &st);
        if (st.st_mode & S_IFREG)
            files.push_back (tuple<string, time_t> (string(entryname), st.st_mtime));
        else if (st.st_mode & S_IFDIR) {
            if (strcmp(".",dirp->d_name)!=0 && strcmp("..",dirp->d_name)!=0)
                process_directory_recursive (entryname, files);            
        }
        free (entryname);
    }
}

void
process_directory_thread (int id, pattern *p,
                          const vector<tuple<string, time_t>> &files)
{
    size_t files_len = files.size ();
    size_t start = id * (files_len / num_threads);
    size_t end = start + (files_len / num_threads);
    if (id == num_threads - 1)
        end = files_len;

    for (size_t i = start; i < end; i++)
        p->process_file (get<0> (files[i]).c_str (),
                         get<1> (files[i]));
}

void
process_directory (pattern *p, const char *dirname)
{
    vector<tuple<string, time_t>> files;
    process_directory_recursive (dirname, files);

    /* start threads to split the work */
    thread threads[num_threads];
    for (int i = 0; i < num_threads; i++)
        threads[i] = thread (process_directory_thread, i, p, files);

    /* join threads */
    for (int i = 0; i < num_threads; i++)
        threads[i].join ();
}

/*
 * MAIN
 */
int
main (int argc, char *argv[])
{
    struct stat st;

    /* reading params */
    if (argc < 2) {
        cout << "USE " << string (argv[0]) << " pmem-file [pattern] ";
        cout << "[input] [-nt=num_threads]";
        cout << endl << flush;
        return 1;
    }

    try {

        /* Opening pmem-file */
        if (access (argv[1], F_OK)) /* new file */
            pop = pool<root>::create (argv[1], "PMEMGREP", POOLSIZE, S_IRWXU);
        else /* file exists */
            pop = pool<root>::open (argv[1], "PMEMGREP");

        auto proot = pop.root (); /* read root structure */

        if (argc == 2) /* No pattern is provided. Print stored patterns and exit
                              */
            proot->print_patterns ();
        else {
            pattern *p = proot->find_pattern (argv[2]);
            if (p == nullptr) /* If not found, one is created */
                p = proot->create_new_pattern (argv[2]);            

            p->start ();

            if (argc == 3) /* No input is provided. Print data and exit */
                p->print ();
            else {
                if (argc > 4)
                    num_threads = atoi (&argv[4][4]);
                if (num_threads < 1)
                    num_threads = 1;

                if (stat (argv[3], &st) == 0) {
                    if (st.st_mode & S_IFREG)
                        p->process_file (argv[3], st.st_mtime);
                    else if (st.st_mode & S_IFDIR)
                        process_directory (p, argv[3]);
                } else {
                    cout << string (argv[3]);
                    cout << " is not a valid input" << endl;
                }
            }
            p->stop();
        }

    } catch (exception &e) {
        cerr << "Exception occured: " << e.what() << endl;
    }

    return 0;
}
