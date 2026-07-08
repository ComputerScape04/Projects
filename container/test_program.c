#include<stdio.h>
#include<unistd.h>

int main(void) {
    printf("Hello from Process %d\n", getpid());
    sleep(60);
}
