#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {

    // bools to take care of flags
    int rem_newline = 0;

    int opt;
    while((opt = (char)getopt(argc, argv, "n")) != -1) {
        switch(opt) {
            case 'n': 
                rem_newline = 1;
                break;

            default:
                fprintf(stderr, "Invalid flag. Try ./echo --help\n");
                exit(1);
                break;
       }
    }

    if (rem_newline) {
        for (int i = 2; i < argc; i++) {
            printf("%s ", argv[i]);
        }

        return 0;
    }
    
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    return 0;
}

