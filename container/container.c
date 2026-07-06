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
#include <sys/syscall.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/capability.h>

#define STACK_SIZE (1024*1024)

/* Define the configuration of the child process*/
struct child_config {
    char* hostname; // hostname
    uid_t uid; // user id
    char* init_proc; // first process to run 
    char* mount_dir; // isolated filesystem
};

int pivot_root(const char* new_root, const char* put_old) {
    // use syscall
    return syscall(SYS_pivot_root, new_root, put_old);
}
        
int mountfs(struct child_config* config) {
    /**
     * 1. Prevent mount propagation using MS_PRIVATE
     * 2. Create a temp directory (this will be our new /)
     * 3. Create a bind mount inside it & an oldroot temp dir
     * NOTE: use busybox (minimal container filesystem with linux/unix 
     *          utility commands
     * 4. Pivot root - change the root to the temp directory created 
     *                  previously.
     * 5. Unmount the old root and remove access to the oldroot temp
     **/
     
    // 1. MS_PRIVATE: Prevent mount propagation 
    if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1) {
        perror("mount");
    }

    // 2. Create unique temp dir to act as new "/"
    char mount_dir[] = "/tmp/tmp.XXXXXX";
    if (mkdtemp(mount_dir) == NULL) {
        perror("mkdtemp");
    }
    
    // 3. Create a bind mount inside this /
    //    and store the oldroot.XXXXXX
    if (mount(config->mount_dir, mount_dir, NULL, MS_BIND | MS_PRIVATE, NULL) == -1) {
        perror("bind mount"); 
    }

    // 3.1 Create the oldroot.XXXXXX and we'll use this to pivot root
    char inner_mount_dir[] = "/tmp/tmp.XXXXXX/oldroot.XXXXXX";
    memcpy(inner_mount_dir, mount_dir, sizeof(mount_dir)-1);
    if (mkdtemp(inner_mount_dir) == NULL) {
        perror("inner mkdtemp");
    }

    // 4. Pivot root from old root to new root /tmp.XXXXXX
    if (pivot_root(mount_dir, inner_mount_dir) == -1) {
        perror("pivot");
    }

    // 5. Unmount old root to make it unaccessible
    char* old_root_dir = basename(inner_mount_dir);
    char old_root[sizeof(inner_mount_dir) + 1] = { "/" };
    strcpy(&old_root[1], old_root_dir);

    if (chdir("/") == -1) {
        perror("chdir");
    }

    if (umount2(old_root, MNT_DETACH) == -1) {
        perror("umount");
    }

    if (rmdir(old_root) == -1) {
        perror("rmdir");
    }

    return 0;
}

// init proc
int child(void* arg) {

    // setting the capabilities of child proc
    cap_t cap;
    cap = cap_get_proc();
    printf("Cap: %s\n", cap_to_text(cap, NULL));

    struct child_config *conf = (struct child_config *)arg;
    char *args[] = {conf->init_proc, 0};

    // changing hostname 
    if (sethostname(conf->hostname, sizeof(conf->hostname)) == -1) {
        perror("sethostname");
    }

    // mounting a new rootfs for container
    if (mountfs(conf) == 0) {
        printf("Successfully mounted filesystem\n");
    }
    
    // running shell in the new namespace
    if(execve(args[0], args, NULL) == -1) {
        perror("execve");
    }

    cap_free(cap);

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
