// Created by Arnau Castellà on 05/10/2021

#ifndef FILE_H

#define FILE_H

#include "types.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char * cleanup_time;
    char * ip;
    char * port;
    char * folder;
} FileInfo;
FileInfo fileData;

FileInfo readFile(int file);
char* readUntil(int fd, char cEnd);
char * getMd5sum(char * file);
int getFileSize(char * file);

#endif
