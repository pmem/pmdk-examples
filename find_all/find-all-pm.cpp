/*
 Copyright (c) 2019, Intel Corporation

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

#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>
#include <regex>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
namespace pobj = pmem::obj;

#define POOLSIZE ((size_t)(1024 * 1024 * 32)) /* 32 MB */

/* globals */
class root;
pobj::pool<root> pop;

/* auxiliary functions */
bool pattern_match(const char *str, const char *pattern) {
    string patternstr("(.*)(");
    patternstr += string(pattern) + string(")(.*)");
    regex exp(patternstr);
    string str_(str);

    if (regex_match(str_, exp))
        return true;
    return false;
}

#define NEW_PM_STRING(pmstr, str)                                              \
    {                                                                          \
        pmstr = pobj::make_persistent<char[]>(strlen(str) + 1);                \
        strcpy(pmstr.get(), str);                                              \
    }

#define PATH_APPEND(newpath, path, addition)                                   \
    {                                                                          \
        newpath = (char *)malloc(strlen(path) + strlen(addition) + 2);         \
        if (newpath == NULL) {                                                 \
            cout << "Error number = " << to_string(errno)                      \
                 << " calling malloc()" << endl;                               \
            return -1;                                                         \
        }                                                                      \
        if (strlen(path) == 0) {                                               \
            sprintf(newpath, "%s", addition);                                  \
        } else if (path[strlen(path) - 1] == '/') {                            \
            sprintf(newpath, "%s%s", path, addition);                          \
        } else {                                                               \
            sprintf(newpath, "%s/%s", path, addition);                         \
        }                                                                      \
    }

/* Data types */
class entry {
  private:
    pobj::persistent_ptr<char[]> parent;
    pobj::persistent_ptr<char[]> name;
    pobj::p<bool> isdir;
    pobj::p<time_t> mtime;
    pobj::persistent_ptr<entry> entries;
    pobj::persistent_ptr<entry> next;

    entry *find_entry(const char *name) {
        pobj::persistent_ptr<entry> ptr = this->entries;
        while (ptr != nullptr) {
            if (strcmp(name, ptr->name.get()) == 0)
                return ptr.get();
            ptr = ptr->next;
        }
        return nullptr;
    }
    entry *add_new_entry(const char *parentname, const char *name, bool isdir) {
        entry *new_entry;
        pobj::transaction::run(pop, [&] {
            pobj::persistent_ptr<entry> ne =
                pobj::make_persistent<entry>(parentname, name, isdir);
            ne->next = this->entries;
            this->entries = ne;
            new_entry = ne.get();
        });
        return new_entry;
    }
    int print_matches(const char *pattern) {
        pobj::persistent_ptr<entry> ptr = this->entries;
        char *newpath;
        while (ptr != nullptr) {
            if (ptr->isdir == false ||
                pattern_match(ptr->name.get(), pattern)) {
                PATH_APPEND(newpath, ptr->parent.get(), ptr->name.get());
                cout << newpath << endl;
                free(newpath);
            }
            if (ptr->isdir)
                ptr->process_directory(pattern);
            ptr = ptr->next;
        }
        return 0;
    }

  public:
    entry(const char *parentname, const char *name, bool isdir) {
        if (parentname == NULL)
            this->parent = nullptr;
        else
            NEW_PM_STRING(this->parent, parentname);
        NEW_PM_STRING(this->name, name);
        this->isdir = isdir;
        this->mtime = 0;
        this->entries = this->next = nullptr;
    }
    int process_directory(const char *pattern) {
        DIR *dp;
        struct dirent *dirp;
        struct stat st;
        char *path;
        char *newpath;

        if (this->parent == nullptr) {
            PATH_APPEND(path, "", this->name.get());
        } else {
            PATH_APPEND(path, this->parent.get(), this->name.get());
        }
        /* Can we open the directory? */
        if ((dp = opendir(path)) == NULL) {
            cout << "Error number = " << to_string(errno) << " opening " << path
                 << endl;
            return -1;
        }

        /* Let's get current 'last modif time' */
        stat(path, &st);
        time_t new_mtime = st.st_mtime;
        if (difftime(new_mtime, this->mtime) != 0) {
            /* dir content has changed, we need
             * to re-scan it
             * */

            while ((dirp = readdir(dp)) != NULL) {
                if (strcmp(".", dirp->d_name) == 0 ||
                    strcmp("..", dirp->d_name) == 0)
                    continue;

                PATH_APPEND(newpath, path, dirp->d_name);
                /* Is dir? */
                bool isdir = false;
                stat(newpath, &st);
                if (st.st_mode & S_IFDIR)
                    isdir = true;

                if (pattern_match(dirp->d_name, pattern) || isdir) {
                    entry *e = this->find_entry(dirp->d_name);
                    if (e == nullptr)
                        e = this->add_new_entry(path, dirp->d_name, isdir);
                    if (isdir == false)
                        e->mtime = st.st_mtime;
                    pop.persist(e->mtime);
                }
            }
            this->mtime = new_mtime;
            pop.persist(this->mtime);
        }
        return print_matches(pattern);
    }
};

class pattern {
  private:
    pobj::persistent_ptr<char[]> patstr;
    pobj::persistent_ptr<char[]> rootstr;
    pobj::persistent_ptr<entry> rootdir;
    pobj::persistent_ptr<pattern> next;

  public:
    pattern(const char *patstr, const char *rootstr) {
        NEW_PM_STRING(this->patstr, patstr);
        NEW_PM_STRING(this->rootstr, rootstr);
        this->rootdir = pobj::make_persistent<entry>(nullptr, rootstr, true);
        this->next = nullptr;
    }
    const char *get_patstr(void) { return this->patstr.get(); }
    const char *get_rootstr(void) { return this->rootstr.get(); }
    pobj::persistent_ptr<pattern> get_next(void) { return this->next; }
    void set_next(pobj::persistent_ptr<pattern> pat) { this->next = pat; }
    int find_all(void) {
        return rootdir->process_directory(this->patstr.get());
    }
};

class root {
  private:
    pobj::persistent_ptr<pattern> patterns;

  public:
    pattern *find_pattern(const char *patstr, const char *rootstr) {
        pobj::persistent_ptr<pattern> ptr = this->patterns;
        while (ptr != nullptr) {
            if ((strcmp(patstr, ptr->get_patstr()) == 0) &&
                (strcmp(rootstr, ptr->get_rootstr()) == 0))
                return ptr.get();
            ptr = ptr->get_next();
        }
        return nullptr;
    }
    pattern *create_pattern(const char *patstr, const char *rootstr) {
        pattern *new_pattern;
        pobj::transaction::run(pop, [&] {
            pobj::persistent_ptr<pattern> new_patterns =
                pobj::make_persistent<pattern>(patstr, rootstr);
            new_patterns->set_next(this->patterns);
            this->patterns = new_patterns;
            new_pattern = this->patterns.get();
        });
        return new_pattern;
    }
};

/*
 * MAIN
 */
int main(int argc, char *argv[]) {
    /* reading params */
    if (argc < 4) {
        cout << "USE " << string(argv[0]) << " pmem-file root-dir pattern";
        cout << endl << flush;
        return 1;
    }
    if (access(argv[1], F_OK)) /* new file */
        pop =
            pobj::pool<root>::create(argv[1], "FIND-ALL-PM", POOLSIZE, S_IRWXU);
    else /* file exists */
        pop = pobj::pool<root>::open(argv[1], "FIND-ALL-PM");
    auto proot = pop.root(); /* read root structure */

    pattern *p = proot->find_pattern(argv[3], argv[2]);
    if (p == nullptr)
        p = proot->create_pattern(argv[3], argv[2]);
    int ret = p->find_all();

    pop.close();
    return ret;
}
