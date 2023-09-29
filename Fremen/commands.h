#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include "file.h"
#include "types.h"

int sockfd;
int loggedIn;

void login(char * name, char * postal_code);
void logout();
void search(char * postal_code);
void sendPhoto(char * photo_name);
void requestPhoto(int id);

#endif
