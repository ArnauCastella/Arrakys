#include "file.h"

char* readUntil(int fd, char cEnd) {
    int i = 0;
    char c;
    char* buffer = (char*)malloc(sizeof(char));

    read(fd, & c, sizeof(char));
    while (c != cEnd) {
        if (c != cEnd) {
            buffer[i] = c;
            buffer = (char *) realloc(buffer, sizeof(char) * (i + 2));
        }
        i++;
        read(fd, & c, sizeof(char));
    }
    buffer[i] = '\0';
    return buffer;
}

FileStruct readConfFile(int fd) {
    FileStruct fs;
    fs.ip = readUntil(fd, '\n');
    fs.port = readUntil(fd, '\n');
    fs.folder = readUntil(fd, '\n');
    fs.folder[strlen(fs.folder)-1] = '\0';

    return fs;
}

char * getMd5sum(char * file) {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) < 0) return NULL;
    if ((pid = fork()) < 0) return NULL;

    if (pid == 0){
        char *args[] = {"md5sum", file, NULL};
        dup2(pipefd[1], 1);
        execvp(args[0], args);
        return NULL;
    } else {
        wait(NULL);
        char * md5sum = readUntil(pipefd[0], ' ');
        close(pipefd[0]);
        close(pipefd[1]);
        return md5sum;
    }
}

int getFileSize(char * file) {
    int fd = open(file, O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    close(fd);
    return size;
}