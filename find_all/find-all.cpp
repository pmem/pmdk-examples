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
#include <regex>
#include <string.h>
#include <string>
#include <sys/stat.h>

using namespace std;

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

#define PATH_APPEND(newpath, path, addition)                                   \
    {                                                                          \
        newpath = (char *)malloc(strlen(path) + strlen(addition) + 2);         \
        if (newpath == NULL) {                                                 \
            cout << "Error number = " << to_string(errno)                      \
                 << " calling malloc()";                                       \
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

int process_directory(const char *dirname, const char *pattern) {
    DIR *dp;
    struct dirent *dirp;
    struct stat st;
    char *entryname = NULL;

    if ((dp = opendir(dirname)) == NULL) {
        cout << "Error number = " << to_string(errno) << " opening " << dirname
             << endl;
        return -1;
    }
    while ((dirp = readdir(dp)) != NULL) {
        if (entryname != NULL) free (entryname);
        PATH_APPEND(entryname, dirname, dirp->d_name);

        if (pattern_match(dirp->d_name, pattern))
            cout << entryname << endl;

        stat(entryname, &st);
        if (st.st_mode & S_IFDIR) {
            if (strcmp(".", dirp->d_name) == 0 ||
                strcmp("..", dirp->d_name) == 0)
                continue;            

            if (process_directory(entryname, pattern) < 0)
                return -1;            
        }
    }
    return 0;
}

/*
 * MAIN
 */
int main(int argc, char *argv[]) {
    /* reading params */
    if (argc < 3) {
        cout << "USE " << string(argv[0]) << " root-dir pattern";
        cout << endl << flush;
        return 1;
    }
    return process_directory(argv[1], argv[2]);
}
