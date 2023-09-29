#ifndef G10_P1_CONNECTIONS_H
#define G10_P1_CONNECTIONS_H

#include "types.h"
#include "file.h"
#include "linked_list.h"

#define MAX_CONNECTIONS 100

int sockfd, connections_count;
pthread_t connections[MAX_CONNECTIONS];
int sockets[MAX_CONNECTIONS];

int startListening(int port);
int sendFrame(int socket, char * source, char type, char * data);
int manageFrame(int socket, char * frame, int * id);

#endif
