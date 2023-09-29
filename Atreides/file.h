#ifndef G10_P1_FILE_H
#define G10_P1_FILE_H

#include "types.h"

typedef struct {
    char * ip;
    char * port;
    char * folder;
} FileStruct;

FileStruct fileInfo;

sem_t users_sem;

FileStruct readConfFile(int fd);
char * readUntil(int fd, char cEnd);
char * getMd5sum(char * file);
int getFileSize(char * file);

#endif
