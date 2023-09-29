#ifndef TYPES_H

#define TYPES_H

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
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define FRAME_SIZE 256
#define ORIGIN_SIZE 15
#define DATA_SIZE 240

#define FILE_ERROR "Please enter the correct configuration file name.\n"
#define WELCOME "Welcome to Fremen\n"
#define BYE "\nDisconnected from Atreides. Dew!\n\n"
#define LESS_ARGS_ERROR "Command KO. Missing parameters\n"
#define MORE_ARGS_ERROR "Command KO. Too many parameters\n"
#define COMMAND_OK "Command OK\n"
#define FORK_ERROR "Error using fork()\n"
#define FRAME_ERROR "Error: The data in the frame contained errors\n"
#define CONNECTED_ATREIDES "You are now connected to Atreides\n"

typedef struct {
    int id;
    char * name;
    char * postal_code;
} User;
User myUser;

#endif
