// Created by Arnau Castellà on 05/10/2021

#include "file.h"
#include <string.h>
#include <stdlib.h>

char* readUntil(int fd, char cEnd) {
    int i = 0;
    char c;
    char* buffer = (char*)malloc(sizeof(char));

    read(fd, & c, sizeof(char));
    while (c != cEnd) {
        buffer[i] = c;
        buffer = (char *) realloc(buffer, sizeof(char) * (i + 2));
        i++;
        read(fd, & c, sizeof(char));
    }
    buffer[i] = '\0';
    return buffer;
}

FileInfo readFile (int file) {
    FileInfo info;
    info.cleanup_time = readUntil(file, '\n');
    info.ip = readUntil(file, '\n');
    info.port = readUntil(file, '\n');
    info.folder = readUntil(file, '\n');
    info.folder[strlen(info.folder)-1] = '\0';

    return info;
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
