#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        printf("Дочерний процесс: PID = %d\n", getpid());
        while (1){
            sleep(1); 
        }
    } else {
        printf("Родительский процесс: PID = %d\n", getpid());
        sleep(2); 
        exit(0);
        printf("Родительский процесс завершился.\n");
    }

    return 0;
}