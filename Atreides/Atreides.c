#include <stdio.h>
#include "types.h"
#include "file.h"
#include "connections.h"
#include "linked_list.h"

void freeMemory() {
    LL_freeAll();

    fileInfo.ip ? free(fileInfo.ip) : NULL;
    fileInfo.port ? free(fileInfo.port) : NULL;
    fileInfo.folder ? free(fileInfo.folder) : NULL;
}

void ctrlc() {
    for (int i = 0; i < connections_count; i++) {
        pthread_cancel(connections[i]);
        pthread_join(connections[i], NULL);
        pthread_detach(connections[i]);
        close(sockets[i]);
    }
    close(sockfd);
    for (int i = 0; i < 4; i++){
        close(i);
    }
    freeMemory();
    exit(0);
}

void * manageConnection(void * args) {
    int socket = *(int *)args;
    char frame[FRAME_SIZE], buffer[100];
    int exit = 0;
    int id;

    while (!exit) {
        if (read(socket, frame, FRAME_SIZE) < 1) {
            sprintf(buffer, "Fremen with ID %d disconected\n", id);
            write(1, buffer, strlen(buffer));
            sem_wait(&user_list);
            LL_removeUser(id);
            sem_post(&user_list);
            exit = 1;
        } else {
            exit = manageFrame(socket, frame, &id);
        }
        write(1, WAITING_CONNECTION, strlen(WAITING_CONNECTION));
        bzero(frame, FRAME_SIZE);
    }

    pthread_exit(NULL);
}

int main(int argc, char * argv[]) {

    signal(SIGINT, ctrlc);

    if (argc != 2) {
        write(1, "Error: Incorrect number of arguments\n", strlen("Error: Incorrect number of arguments\n"));
        return -1;
    }

    //Initialize semaphores
    sem_init(&users_sem, 0, 1);
    sem_init(&user_list, 0, 1);

    write(1, ATREIDES, strlen(ATREIDES));

    //Open configuration file
    int file;
    file = open(argv[1], O_RDONLY);
    if (file < 0) {
        write(1, FILE_ERROR, strlen(FILE_ERROR));
        return -1;
    }

    //Read and close configuration file
    fileInfo = readConfFile(file);
    write(1, FILE_READ, strlen(FILE_READ));
    close(file);

    struct stat st = {0};
    if (stat(fileInfo.folder, &st) == -1) {
        mkdir(fileInfo.folder, 0777);
    }

    if (startListening(atoi(fileInfo.port)) == -1) {
        write(1, "Error: could not connect to the port\n", strlen("Error: could not connect to the port\n"));
        return -1;
    }
    write(1, WAITING_CONNECTION, strlen(WAITING_CONNECTION));

    socklen_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);
    connections_count = 0;
    while(1) {
        sockets[connections_count] = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (sockets[connections_count] < 0) {
            write(1, "Error accepting connection\n", strlen("Error accepting connection\n"));
        }
        connections_count++;
        if (pthread_create(&connections[connections_count-1], NULL, manageConnection,(void *) &sockets[connections_count-1]) != 0) {
            write(1, "Failed to create thread\n", strlen("Failed to create thread\n"));
        }
    }

    return 0;
}