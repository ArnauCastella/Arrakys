#include "commands.h"

int connectPort(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    if (sock == -1) {
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(fileData.ip);
    server.sin_port = htons(port);

    int conn_status = connect(sock , (struct sockaddr *)&server , sizeof(server));
    if (conn_status < 0) {
        return -1;
    }
    sockfd = sock;
    return 1;
}

int validateData(char * frame) {
    char type;
    int chars = 0, zeros = 0, i;
    for (i = 0; i < ORIGIN_SIZE; i++) {
        if (frame[i] >= 'A' && frame[i] <= 'Z') chars++;
        if (frame[i] == '\0') zeros++;
    }
    if (chars+zeros != ORIGIN_SIZE) return 0;
    type = frame[i++];

    int asteriscs = 0;
    chars = 0;
    zeros = 0;
    for (int j = 0; i < FRAME_SIZE; i++, j++) {
        if ((frame[i] >= 'A' && frame[i] <= 'Z')||(frame[i] >= 'a' && frame[i] <= 'z')||(frame[i] >= '0' && frame[i] <= '9')) chars++;
        if (frame[i] == '\0') zeros++;
        if (frame[i] == '*') asteriscs++;
    }
    //printf("%d %d %d\n", asteriscs, chars, zeros);
    if (chars+zeros+asteriscs != DATA_SIZE) return 0;

    switch (type) {
        case 'O':
            return asteriscs == 0;
        case 'E':
            return asteriscs == 0;
        case 'S':
            return asteriscs == 2;
        case 'L':
            return 1;
        case 'K':
            return asteriscs == 0 && chars == 0;
        case 'I':
            return asteriscs == 0;
        case 'R':
            return asteriscs == 0;
        case 'F':
            return asteriscs == 2 || asteriscs == 0;
        case 'D':
            return asteriscs == 0;
        case 'Z':
            return asteriscs == 0;
    }
    return 0;
}

int sendFrame(char * source, char type, char * data) {
    char buffer[FRAME_SIZE];
    bzero(buffer, FRAME_SIZE);

    int i;
    for (i = 0; source[i] != '\0'; i++) {
        buffer[i] = source[i];
    }
    i = ORIGIN_SIZE;
    buffer[i++] = type;
    int j = 0;
    while (i < FRAME_SIZE) {
        buffer[i++] = data[j++];
    }

    if (write(sockfd, buffer, FRAME_SIZE) < 1) {
        return 0;
    }

    return 1;
}

char readFrameType() {
    char frame[FRAME_SIZE];

    read(sockfd, frame, FRAME_SIZE);

    return frame[ORIGIN_SIZE];
}

void login(char * name, char * postal_code) {
    if (connectPort(atoi(fileData.port)) == -1) {
        write(1, "Error connecting to Atreides!\n", strlen("Error connecting to Atreides!\n"));
        return;
    }
    //Connection protocol
    char frame[FRAME_SIZE], data[DATA_SIZE], source[ORIGIN_SIZE], type;
    bzero(data, DATA_SIZE);
    sprintf(data, "%s*%s", name, postal_code);
    if (sendFrame("FREMEN\0", 'C', data) == 0) return;

    read(sockfd, frame, FRAME_SIZE);

    if (validateData(frame)) {
        bzero(source, ORIGIN_SIZE);
        bzero(data, DATA_SIZE);
        strcpy(source, frame);
        type = frame[ORIGIN_SIZE];
        strcpy(data, &frame[ORIGIN_SIZE+1]);

        if (type == 'O') {
            char buffer[300];
            loggedIn = 1;
            myUser.id = atoi(data);
            myUser.name = (char*)malloc(sizeof (char) * (strlen(name)+2));
            myUser.postal_code = (char*)malloc(sizeof (char) * (strlen(postal_code)+2));
            strcpy(myUser.name, name);
            strcpy(myUser.postal_code, name);
            sprintf(buffer, "Welcome %s. You have the ID %s.\n", name, data);
            write(1, buffer, strlen(buffer));
            write(1, CONNECTED_ATREIDES, strlen(CONNECTED_ATREIDES));
        } else {
            write(1, FRAME_ERROR, strlen(FRAME_ERROR));
        }
    } else {
        sendFrame("FREMEN\0", 'Z', "ERROR T\0");
    }
}

void logout() {
    char buffer[DATA_SIZE];
    bzero(buffer, DATA_SIZE);
    sprintf(buffer, "%s*%d", myUser.name, myUser.id);
    sendFrame("FREMEN\0", 'Q', buffer);
    loggedIn = 0;
}

void search(char * postal_code) {
    char frame[FRAME_SIZE], data[DATA_SIZE], source[ORIGIN_SIZE], type;

    bzero(data, DATA_SIZE);
    sprintf(data, "%s*%d*%s", myUser.name, myUser.id, postal_code);
    if (sendFrame("FREMEN\0", 'S', data) == 0) return;

    read(sockfd, frame, FRAME_SIZE);

    bzero(source, ORIGIN_SIZE);
    bzero(data, DATA_SIZE);
    strcpy(source, frame);
    type = frame[ORIGIN_SIZE];
    strcpy(data, &frame[ORIGIN_SIZE+1]);

    if (validateData(frame)) {
        bzero(source, ORIGIN_SIZE);
        bzero(data, DATA_SIZE);
        strcpy(source, frame);
        type = frame[ORIGIN_SIZE];
        strcpy(data, &frame[ORIGIN_SIZE+1]);

        if (type == 'L') {
            int numUsers = atoi(strtok(data, "*"));

            char buffer[100];
            sprintf(buffer, "There are %d human beings at %s:\n", numUsers, postal_code);
            write(1, buffer, strlen(buffer));

            for (int i = 0; i < numUsers; i++) {
                char * name = strtok(NULL, "*");
                if (name == NULL) {
                    read(sockfd, frame, FRAME_SIZE);
                    if (validateData(frame)) {
                        bzero(source, ORIGIN_SIZE);
                        bzero(data, DATA_SIZE);
                        strcpy(source, frame);
                        type = frame[ORIGIN_SIZE];
                        strcpy(data, &frame[ORIGIN_SIZE + 1]);
                        if (type == 'L') {
                            name = strtok(data, "*");
                        }
                    }
                }
                int id = atoi(strtok(NULL, "*"));
                sprintf(buffer, "%d %s\n", id, name);
                write(1, buffer, strlen(buffer));
            }

        } else {
            write(1, FRAME_ERROR, strlen(FRAME_ERROR));
        }

    } else {
        sendFrame("FREMEN\0", 'Z', "ERROR T\0");
    }
}

void sendPhoto(char * photo_name) {
    char data[DATA_SIZE];
    char buffer[100];
    sprintf(buffer, "%s/%s", fileData.folder, photo_name);

    int imgfd = open(buffer, O_RDONLY);
    if (imgfd == -1) {
        write(1, "File does not exist\n", strlen("File does not exist\n"));
        return;
    }
    char * md5sum = getMd5sum(buffer);
    int size = getFileSize(buffer);
    //Name*Size*MD5SUM
    bzero(data, DATA_SIZE);
    sprintf(data, "%s*%d*%s", photo_name, size, md5sum);
    sendFrame("FREMEN\0", 'F', data);
    bzero(data, DATA_SIZE);


    int bytesLeft = size;
    while (bytesLeft > 0) {
        if (bytesLeft < DATA_SIZE) {
            read(imgfd, data, bytesLeft);
        } else {
            read(imgfd, data, DATA_SIZE);
        }
        bytesLeft -= DATA_SIZE;
        if (sendFrame("FREMEN\0", 'D', data) == 0) return;
        bzero(data, DATA_SIZE);
    }
    close(imgfd);
    free(md5sum);

    if (readFrameType() == 'I') {
        write(1, "Photo successfully sent to Atreides.\n", strlen("Photo successfully sent to Atreides.\n"));
    } else {
        write(1, "Something went wrong sending the image\n", strlen("Something went wrong sending the image\n"));
    }
}

void requestPhoto(int id) {
    char frame[FRAME_SIZE], data[DATA_SIZE];

    bzero(data, DATA_SIZE);
    sprintf(data, "%d", id);
    sendFrame("FREMEN\0", 'P', data);

    if (read(sockfd, frame, FRAME_SIZE) == 0) {
        raise(SIGPIPE);
        return;
    }
    strcpy(data, &frame[ORIGIN_SIZE+1]);

    if (strcmp(data, "FILE NOT FOUND\0") == 0){
        write(1, "No photo registered\n", strlen("No photo registered\n"));
        return;
    }
    char file_name[31], md5sum[33], buffer[200];
    strcpy(file_name, strtok(data, "*"));
    int size = atoi(strtok(NULL, "*"));
    strcpy(md5sum, strtok(NULL, "*"));

    bzero(buffer, 200);
    sprintf(buffer, "%s/%s", fileData.folder, file_name);
    remove(buffer);
    int imgfd = open(buffer, O_WRONLY | O_CREAT, 0777);
    int bytesLeft = size;
    char newFrame[FRAME_SIZE];
    while (bytesLeft > 0) {
        read(sockfd, newFrame, FRAME_SIZE);

        if (bytesLeft < DATA_SIZE) {
            write(imgfd, &newFrame[ORIGIN_SIZE+1], bytesLeft);
        } else {
            write(imgfd, &newFrame[ORIGIN_SIZE+1], DATA_SIZE);
        }
        bytesLeft -= DATA_SIZE;
    }
    close(imgfd);
    char * newMd5 = getMd5sum(buffer);
    if (strcmp(md5sum, newMd5) == 0) {
        write(1, "Photo downloaded\n", strlen("Photo downloaded\n"));
        sendFrame("FREMEN\0", 'I', "IMAGE OK\0");
    } else {
        sendFrame("FREMEN\0", 'R', "IMAGE KO\0");
        remove(buffer);
    }
    free(newMd5);
}