#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

char * user_name;
char * pid_kill;

char* readUntil(int fd, char cEnd) {
    int i = 0;
    char c;
    char* buffer = (char*)malloc(sizeof(char));

    if (read(fd, & c, sizeof(char)) == 0) {
        free(buffer);
        return NULL;
    }
    while (c != cEnd && c != '\0') {
        printf("%c", c);
        buffer[i] = c;
        buffer = (char *) realloc(buffer, sizeof(char) * (i + 2));
        i++;
        read(fd, & c, sizeof(char));
    }
    buffer[i] = '\0';
    return buffer;
}

void getUsername() {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) < 0) return;
    if ((pid = fork()) < 0) return;

    if (pid == 0){
        char *args[] = {"whoami", NULL};
        dup2(pipefd[1], 1);
        execvp(args[0], args);
        exit(0);
    } else {
        wait(NULL);
        user_name = readUntil(pipefd[0], '\n');
        close(pipefd[0]);
        close(pipefd[1]);
    }
}

void ctrlc() {
    free(user_name);
    pid_kill ? free(pid_kill) : NULL;
    for (int i = 0; i < 3; i++) {
        close(i);
    }
    write(1, "\nExiting...\n", strlen("Exiting...\n"));
    exit(0);
}

void getPID() {
    pid_t pid;
    int i = 0;
    char ** pids;

    pid = fork();
    if (pid == 0) {
        int fd = open("pids.txt", O_RDWR | O_CREAT, 0777);
        char *args[] = {"pgrep", "-u", user_name, "Fremen", NULL};
        dup2(fd, 1);
        execvp(args[0], args);
        close(fd);
        exit(0);
    } else {
        wait(NULL);
        char aux;
        int end = 1;

        int fd = open("pids.txt", O_RDONLY);
        if (read(fd, &aux, 1) > 0) {
            lseek(fd, -1, SEEK_CUR);
            end = 0;
            pids = (char**)malloc(sizeof(char*));
        } else {
            close(fd);
            pid_kill = NULL;
            return;
        }
        while (!end) {
            pids[i++] = readUntil(fd, '\n');

            if (read(fd, &aux, 1) > 0) {
                lseek(fd, -1, SEEK_CUR);
                end = 0;
                pids = (char**)realloc(pids, sizeof (char*) * (i+1));
            } else {
                end = 1;
            }
        }
        close(fd);
        int index = rand()%i;
        pid_kill = (char*) malloc (sizeof (char) * strlen(pids[index])+1);
        strcpy(pid_kill, pids[index]);
        for (int j = 0; j < i; j++) {
            free(pids[j]);
            pids[j] = NULL;
        }
        free(pids);
        remove("pids.txt");
    }
}

int main (int argc, char * argv[]) {
    char buffer[100];
    pid_t pid;
    pid_kill = NULL;
    remove("pids.txt");

    if (argc != 2) {
        write(1, "Incorrect number of parameters\n", strlen("Incorrect number of parameters\n"));
        return 0;
    }

    write(1, "\nStarting Harkonen...\n", strlen("\nStarting Harkonen...\n"));

    srand(time(NULL));
    signal(SIGINT, ctrlc);

    getUsername();

    while (1) {
        write(1, "\nScanning PIDs...\n", strlen("\nScanning PIDs...\n"));
        getPID();

        if (pid_kill != NULL) {
            sprintf(buffer, "Killing PID %s\n", pid_kill);
            write(1, buffer, strlen(buffer));
            if ((pid = fork()) < 0) return 0;
            if (pid == 0) {
                char *args[] = {"kill", pid_kill, NULL};
                execvp(args[0], args);
                exit(0);
            } else {
                wait(NULL);
            }
        } else {
            write(1, "There are no Fremen to kill\n", strlen("There are no Fremen to kill\n"));
        }

        if (pid_kill != NULL) {
            free(pid_kill);
            pid_kill = NULL;
        }
        sleep(atoi(argv[1]));
    }

    return 0;
}

