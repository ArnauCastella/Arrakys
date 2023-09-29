#ifndef G10_P1_TYPES_H
#define G10_P1_TYPES_H

#define _GNU_SOURCE
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/stat.h>

#define FRAME_SIZE 256
#define SOURCE_SIZE 15
#define DATA_SIZE 240

#define ATREIDES "\nATREIDES SERVER\n"
#define FILE_ERROR "Could not open file\n"
#define FILE_READ "Configuration file read\n"
#define WAITING_CONNECTION "Waiting connections...\n\n"
#define REPLY_SENT "Reply sent\n"
#define DISCONNECTED "Disconnected from Atreides.\n"

typedef struct {
    int id;
    char * name;
    char * postal_code;
} User;

#endif
