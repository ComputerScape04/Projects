/**
 *
 * ls command in C
 *
 **/

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {

    /*
           struct dirent {
               ino_t          d_ino;       // Inode number 
               off_t          d_off;       // Not an offset; see below 
               unsigned short d_reclen;    // Length of this record 
               unsigned char  d_type;      // Type of file; not supported
                                           //  by all filesystem types 
               char           d_name[256]; // Null-terminated filename 
           };
    */

    char cwd[128];
    getcwd(cwd, sizeof cwd);
    //printf("%s\n", cwd);

    // open the dir
    DIR* dir = opendir(cwd);

    // read the dir
    struct dirent *dir_entry;
    while((dir_entry = readdir(dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0) {
            continue;
        }
        printf("%s  ", dir_entry->d_name);
    }
    printf("\n");
    
    return 0;
}
