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
#include <libpmemobj++/allocator.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>
#include <regex>
#include <string.h>
#include <string>
#include <vector>

#define POOLSIZE ((size_t) (1024 * 1024 * 256)) /* 256 MB */

using namespace std;
using namespace pmem;
using namespace pmem::obj;


/* globals */
class root;
pool<root> pop;


/* persistent data structures */

struct line {
	persistent_ptr<char[]> linestr;
	p<size_t> linenum;
};

class file
{
	private:
	persistent_ptr<file> next;
	persistent_ptr<char[]> name;
	p<time_t> mtime;
	vector<line, pmem::obj::allocator<line>> lines;

	public:
	file (const char *filename)
	{
		name = make_persistent<char[]> (strlen (filename) + 1);
		strcpy (name.get (), filename);
		mtime = 0;
	}

	char *
	get_name (void)
	{
		return name.get ();
	}

	size_t
	get_nlines (void)
	{
		return lines.size (); /* nlines; */
	}

	struct line *
	get_line (size_t index)
	{
		return &(lines[index]);
	}

	persistent_ptr<file>
	get_next (void)
	{
		return next;
	}

	void
	set_next (persistent_ptr<file> n)
	{
		next = n;
	}

	time_t
	get_mtime (void)
	{
		return mtime;
	}

	void
	set_mtime (time_t mt)
	{
		mtime = mt;
	}

	void
	create_new_line (string linestr, size_t linenum)
	{
		transaction::run (pop, [&] {
			struct line new_line;

			/* creating new line */
			new_line.linestr
			= make_persistent<char[]> (linestr.length () + 1);
			strcpy (new_line.linestr.get (), linestr.c_str ());
			new_line.linenum = linenum;

			lines.push_back (new_line);
		});
	}

	int
	process_pattern (const char *str)
	{
		std::ifstream fd (name.get ());
		string line;

		string patternstr ("(.*)(");
		patternstr += string (str) + string (")(.*)");
		regex exp (patternstr);

		int ret = 0;
		transaction::run (
		pop,
		[&] { /* dont leave a file processed
		       * half way through */
		      if (fd.is_open ()) {
			      size_t linenum = 0;
			      while (getline (fd, line)) {
				      ++linenum;
				      if (regex_match (line, exp))
					      /* adding this line... */
					      create_new_line (line, linenum);
			      }
		      } else {
			      cout
			      << "unable to open file " + string (name.get ())
			      << endl;
			      ret = -1;
		      }
		});
		return ret;
	}

	void
	remove_lines ()
	{
		lines.clear ();
	}
};

class pattern
{
	private:
	persistent_ptr<pattern> next;
	persistent_ptr<char[]> patternstr;
	persistent_ptr<file> files;
	p<size_t> nfiles;

	public:
	pattern (const char *str)
	{
		patternstr = make_persistent<char[]> (strlen (str) + 1);
		strcpy (patternstr.get (), str);
		files = nullptr;
		nfiles = 0;
	}

	file *
	get_file (size_t index)
	{
		persistent_ptr<file> ptr = files;
		size_t i = 0;
		while (i < index && ptr != nullptr) {
			ptr = ptr->get_next ();
			i++;
		}
		return ptr.get ();
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

	file *
	find_file (const char *filename)
	{
		persistent_ptr<file> ptr = files;
		while (ptr != nullptr) {
			if (strcmp (filename, ptr->get_name ()) == 0)
				return ptr.get ();
			ptr = ptr->get_next ();
		}
		return nullptr;
	}

	file *
	create_new_file (const char *filename)
	{
		file *new_file;
		transaction::run (pop, [&] {
			/* allocating new files head */
			persistent_ptr<file> new_files
			= make_persistent<file> (filename);
			/* making the new allocation the actual head */
			new_files->set_next (files);
			files = new_files;
			nfiles = nfiles + 1;

			new_file = files.get ();
		});
		return new_file;
	}

	void
	print (void)
	{
		cout << "PATTERN = " << patternstr.get () << endl;
		cout << "\t" << nfiles << " files(s) scanned" << endl;
		for (size_t i = 0; i < nfiles; i++) {
			file *f = get_file (i);
			cout << "###############" << endl;
			cout << "FILE = " << f->get_name () << endl;
			cout << "###############" << endl;
			cout << "*** pattern present in " << f->get_nlines ();
			cout << " lines ***" << endl;
			for (size_t j = f->get_nlines (); j > 0; j--) {
				cout << f->get_line (j - 1)->linenum << ": ";
				cout
				<< string (f->get_line (j - 1)->linestr.get ());
				cout << endl;
			}
		}
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
		pattern *new_pattern;
		transaction::run (pop, [&] {
			/* allocating new patterns arrray */
			persistent_ptr<pattern> new_patterns
			= make_persistent<pattern> (patternstr);
			/* making the new allocation the actual head */
			new_patterns->set_next (patterns);
			patterns = new_patterns;
			npatterns = npatterns + 1;

			new_pattern = patterns.get ();
		});
		return new_pattern;
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

int
process_reg_file (pattern *p, const char *filename, const time_t mtime)
{
	file *f = p->find_file (filename);
	if (f != nullptr && difftime (mtime, f->get_mtime ()) == 0) /* file
	                                                               exists */
		return 0;
	if (f == nullptr) /* file does not exist */
		f = p->create_new_file (filename);
	else /* file exists but it has an old timestamp (modification) */
		f->remove_lines ();
	if (f->process_pattern (p->get_str ()) < 0) {
		cout << "problems processing file " << filename << endl;
		return -1;
	}
	f->set_mtime (mtime);
	return 0;
}

int
process_directory_recursive (const char *dirname,
                             vector<tuple<string, time_t>> &files)
{
	DIR *dp;
	struct dirent *dirp;
	struct stat st;
	char *entryname;

	if ((dp = opendir(dirname)) == NULL) {
		cout << "Error number = " << errno << " opening " << dirname << endl;
		return -1;
	}
        while ((dirp = readdir(dp)) != NULL) {
                entryname = (char *) malloc (strlen(dirname)+strlen(dirp->d_name)+2);
                if (entryname==NULL)
                        return -1;
                sprintf (entryname, "%s/%s", dirname, dirp->d_name);

                stat (entryname, &st);
                if (st.st_mode & S_IFREG)
                        files.push_back (
			tuple<string, time_t> (string(entryname), st.st_mtime));
                else if (st.st_mode & S_IFDIR) {
                        if (strcmp(".",dirp->d_name)!=0 &&
                            strcmp("..",dirp->d_name)!=0) {
                                if (process_directory_recursive (entryname, files)
                                < 0)
                                        return -1;
                        }
                }
                free (entryname);
        }
	return 0;
}

int
process_directory (pattern *p, const char *dirname)
{
	vector<tuple<string, time_t>> files;
	if (process_directory_recursive (dirname, files) < 0)
		return -1;

	for (vector<tuple<string, time_t>>::iterator it = files.begin ();
	     it != files.end (); ++it)
		if (process_reg_file (p, get<0> (*it).c_str (), get<1> (*it)) < 0)
			return -1;
	return 0;
}

int
process_input (struct pattern *p, const char *input)
{
	struct stat st;

	if (stat (input, &st) == 0) {
		if (st.st_mode & S_IFREG)
			return process_reg_file (p, input, st.st_mtime);
		else if (st.st_mode & S_IFDIR)
			return process_directory (p, input);
	} else {
		cout << string (input);
		cout << " is not a valid input" << endl;
	}
	return -1;
}

/*
 * MAIN
 */
int
main (int argc, char *argv[])
{

	/* reading params */
	if (argc < 2) {
		cout << "USE " << string (argv[0]) << " pmem-file [pattern] ";
		cout << "[input]";
		cout << endl << flush;
		return 1;
	}

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

		if (argc == 3) /* No input is provided. Print data and exit */
			p->print ();
		else
			return process_input (p, argv[3]);
	}
	return 0;
}
