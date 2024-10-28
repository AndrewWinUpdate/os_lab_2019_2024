#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Ошибка при создании процесса
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Это дочерний процесс, выполняем sequiential_min_max
        char *args[] = {"./sequiential_min_max", NULL};  // Программа и её аргументы
        if (execvp(args[0], args) == -1) {
            perror("exec failed");
            exit(1);
        }
    } else {
        // Это родительский процесс, ждем завершения дочернего
        int status;
        waitpid(pid, &status, 0);  // Ожидание завершения дочернего процесса

        if (WIFEXITED(status)) {
            printf("Child process finished with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process did not terminate normally.\n");
        }
    }

    return 0;
}
