#define _GNU_SOURCE

#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/utsname.h>
#include <sys/mount.h>
#include <sys/capability.h>
#include <stdbool.h>

#define STACK_SIZE 1024*1024

/* Define the configuration of the child process*/
struct child_config {
    char* hostname; // hostname
    uid_t uid; // user id
    char* init_proc; // first process to run 
    char* mount_dir; // isolated filesystem
};

// init proc
int child(void* arg) {

    struct child_config *conf = (struct child_config *)arg;

    char *args[] = {conf->init_proc, 0};

    if (sethostname(conf->hostname, sizeof(conf->hostname)) == -1) {
        perror("sethostname");
    }


    
    // running shell in the new namespace
    if(execve(args[0], args, NULL) == -1) {
        perror("execve");
    }

    return 0;
}

int main(int argc, char** argv) {
    
    /**
     *
     * linux container via namespaces
     * -- using different clone flags 
     *  - clone_newpid (process id)
     *  - clone_newns (mount)
     *  - clone_newnet (network stack)
     *  - clone_newuts (hostname)
     *  - clone_newuser (elevated permissions in isolation)
     *  - clone_newipc (inter-process communication)
     **/

    struct child_config config;
    
    int flags = CLONE_NEWPID | CLONE_NEWNS |
                CLONE_NEWNET | CLONE_NEWUTS |
                CLONE_NEWIPC | CLONE_NEWUSER ;

    char stack[STACK_SIZE];

    bool has_h, has_m, has_u, has_i;
    has_h = has_m = has_u = has_i = 0;
    
    // handle command line arguments
    // accept hostname, uid and mount directory 
    int opt;
    while((opt = (char)getopt(argc, argv, "h:m:u:i:")) != -1) {
        switch(opt) {
            case 'h':
                config.hostname = optarg; 
                has_h = 1;
                break;
            case 'm':
                config.mount_dir = optarg; 
                has_m = 1;
                break;
            case 'u':
                if (sscanf(optarg, "%d", &(config.uid)) == -1) {
                    perror("uid format");
                    return 1;
                }
                has_u = 1;
                break;
            case 'i':
                config.init_proc = optarg;
                has_i = 1;
                break;
            default:
                fprintf(stderr, "invalid flag %c\n", opt);
                exit(0);
                break;
        }
    }

    if (!has_h || !has_m || !has_u || !has_i) {
                fprintf(stderr, "usage: -h <hostname> -m <mount_dir> -u <user_id> -i <init_proc>\n");
                exit(0);
    }   

    void* args = (void *)&config;

    pid_t t = clone(&child, stack + STACK_SIZE, flags | SIGCHLD, args);
    
    waitpid(t, NULL, 0);

    if (errno) {
        printf("Error: %s\n", strerror(errno));
    }

	return 0;
}
