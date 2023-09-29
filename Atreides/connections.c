#include "connections.h"


int sendFrame(int socket, char * source, char type, char * data) {
    char buffer[FRAME_SIZE];
    bzero(buffer, FRAME_SIZE);

    int i;
    for (i = 0; source[i] != '\0'; i++) {
        buffer[i] = source[i];
    }
    i = SOURCE_SIZE;
    buffer[i++] = type;
    int j = 0;
    while (i < FRAME_SIZE) {
        buffer[i++] = data[j++];
    }

    if (write(socket, buffer, FRAME_SIZE) < 1) return 0;
    return 1;
}

char readFrameType(int socket) {
    char frame[FRAME_SIZE];

    read(socket, frame, FRAME_SIZE);

    return frame[SOURCE_SIZE];
}

int startListening(int port) {
    struct sockaddr_in serv_addr;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        perror("ERROR opening socket");
        return -1;
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(fileInfo.ip);
    serv_addr.sin_port = htons(port);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR on binding");
        return -1;
    }
    // Now start listening
    listen(sockfd,50);
    return 1;
}

int saveUser(char * name, char * postal_code) {
    int users_fd = open("users.txt", O_CREAT | O_RDWR);
    if (users_fd == -1) {
        write(1, "Error opening file\n", strlen("Error opening file\n"));
        return -1;
    }
    User user_in;
    int lastID = -1, loop = 0;
    char aux;
    if (read(users_fd, &aux, sizeof(char)) != 0) {
        lseek(users_fd, -1, SEEK_CUR);
        loop = 1;
    }
    while (loop) {
        char * id_aux = readUntil(users_fd, '-');
        user_in.id = atoi(id_aux);
        lastID = user_in.id;
        free(id_aux);
        user_in.name = readUntil(users_fd, '-');
        user_in.postal_code = readUntil(users_fd, '\n');

        if (strcmp(user_in.name, name) == 0) {
            free(user_in.name);
            free(user_in.postal_code);
            close(users_fd);
            return lastID;
        }
        free(user_in.name);
        user_in.name = NULL;
        free(user_in.postal_code);
        user_in.postal_code = NULL;

        if (read(users_fd, &aux, sizeof(char)) != 0) {
            lseek(users_fd, -1, SEEK_CUR);
            loop = 1;
        } else {
            loop = 0;
        }
    }
    /*
    user_in.id = lastID+1;
    user_in.name = (char*) malloc (sizeof (char) * strlen(name)+1);
    user_in.postal_code = (char*) malloc (sizeof (char) * strlen(postal_code)+1);
    strcpy(user_in.name, name);
    strcpy(user_in.postal_code, postal_code);
    */

    // Format in file:
    // ID-name-postalcode\n
    char buffer[300];
    bzero(buffer, 300);
    //sprintf(buffer, "%d-%s-%s\n", user_in.id, user_in.name, user_in.postal_code);
    sprintf(buffer, "%d-%s-%s\n", lastID+1, name, postal_code);
    write(users_fd, buffer, strlen(buffer));

    user_in.name ? free(user_in.name) : NULL;
    user_in.postal_code ? free(user_in.postal_code) : NULL;
    close(users_fd);

    return lastID+1;
}

int validateData(char * frame) {
    char type;
    int chars = 0, zeros = 0, i;
    for (i = 0; i < SOURCE_SIZE; i++) {
        if (frame[i] >= 'A' && frame[i] <= 'Z') chars++;
        if (frame[i] == '\0') zeros++;
    }
    if (chars+zeros != SOURCE_SIZE) return 0;
    type = frame[i++];

    int asteriscs = 0;
    chars = 0;
    zeros = 0;
    for (int j = 0; i < FRAME_SIZE; i++, j++) {
        if ((frame[i] >= 'A' && frame[i] <= 'Z')||(frame[i] >= 'a' && frame[i] <= 'z')||(frame[i] >= '0' && frame[i] <= '9')) chars++;
        if (frame[i] == '\0') zeros++;
        if (frame[i] == '*') asteriscs++;
    }
    if (chars+zeros+asteriscs != DATA_SIZE) return 0;

    switch (type) {
        case 'C':
            return asteriscs == 1;
        case 'S':
            return asteriscs == 2;
        case 'F':
            return asteriscs == 2;
        case 'D':
            return asteriscs == 0;
        case 'P':
            return asteriscs == 0;
        case 'I':
            return asteriscs == 0;
        case 'R':
            return asteriscs == 0;
        case 'Z':
            return asteriscs == 0;
    }
    return 0;
}

int searchUsers(char * postal_code, User ** usersFound) {
    int fd = open("users.txt", O_RDONLY);
    if (fd == -1) {
        write(1, "Error opening file\n", strlen("Error opening file\n"));
        return -1;
    }

    int loop = 0, numUsers = 0;
    char aux;
    if (read(fd, &aux, sizeof(char)) != 0) {
        lseek(fd, -1, SEEK_CUR);
        loop = 1;
    }
    while (loop) {
        char * id = readUntil(fd, '-');
        char * name = readUntil(fd, '-');
        char * code = readUntil(fd, '\n');

        if (strcmp(code, postal_code) == 0) {
            (*usersFound)[numUsers].name = (char *) malloc (sizeof(char)*strlen(name)+1);
            strcpy((*usersFound)[numUsers].name, name);
            (*usersFound)[numUsers].id = atoi(id);
            numUsers++;
            *usersFound = (User *) realloc (*usersFound, sizeof(User) * (numUsers+1));
        }

        free(id);
        free(name);
        free(code);

        if (read(fd, &aux, sizeof(char)) != 0) {
            lseek(fd, -1, SEEK_CUR);
            loop = 1;
        } else {
            loop = 0;
        }
    }

    close(fd);

    return numUsers;
}

int manageFrame(int socket, char * frame, int * myID) {
    char source[SOURCE_SIZE], type, data[DATA_SIZE];
    char buffer[100];

    bzero(source, SOURCE_SIZE);
    bzero(data, DATA_SIZE);
    strcpy(source, frame);
    type = frame[SOURCE_SIZE];
    strcpy(data, &frame[SOURCE_SIZE+1]);

    switch (type) {
        case 'C':
            if (validateData(frame)) {
                User user;

                char * aux = strtok(data, "*");
                user.name = (char *)malloc(sizeof (char) * strlen(aux)+1);
                strcpy(user.name, aux);
                aux = strtok(NULL, "*");
                user.postal_code = (char *)malloc(sizeof (char) * strlen(aux)+1);
                strcpy(user.postal_code, aux);
                bzero(buffer, 100);
                sprintf(buffer, "Received login %s %s\n", user.name, user.postal_code);
                write(1, buffer, strlen(buffer));

                sem_wait(&users_sem);
                int id = saveUser(user.name, user.postal_code);
                sem_post(&users_sem);
                user.id = id;
                *myID = id;

                sem_wait(&user_list);
                LL_addUser(user);
                sem_post(&user_list);
                //LL_printList();
                user.name ? free(user.name) : NULL;
                user.postal_code ? free(user.postal_code) : NULL;

                bzero(buffer, DATA_SIZE);
                sprintf(buffer, "%d", id);
                sendFrame(socket, "ATREIDES\0", 'O', buffer);

                sprintf(buffer, "Assigned to ID %d\n", id);
                write(1, buffer, strlen(buffer));
                write(1, REPLY_SENT, strlen(REPLY_SENT));

                return 0;
            } else {
                sendFrame(socket, "ATREIDES\0", 'E', "ERROR\0");
                //Exit thread
                return 1;
            }
            break;
        case 'Q': {
            //Log out
            char * name, * id;
            name = strtok(data, "*");
            id = strtok(NULL, "*");
            bzero(buffer, 100);
            sprintf(buffer, "Received logout from %s %s\n", name, id);
            write(1, buffer, strlen(buffer));

            close(socket);
            sem_wait(&user_list);
            LL_removeUser(atoi(id));
            sem_post(&user_list);
            write(1, DISCONNECTED, strlen(DISCONNECTED));

            //LL_printList();

            return 1;
            break;
        }
        case 'S': {
            //Search
            if (!validateData(frame)) {
                sendFrame(socket, "ATREIDES\0", 'K', "\0");
                return 1;
            }
            char * name, * id, * postal_code;
            name = strtok(data, "*");
            id = strtok(NULL, "*");
            postal_code = strtok(NULL, "*");
            sprintf(buffer, "Received search %s from %s %d\n", postal_code, name, atoi(id));
            write(1, buffer, strlen(buffer));

            User * usersFound;
            usersFound = (User *) malloc (sizeof (User));
            sem_wait(&users_sem);
            int numUsersFound = searchUsers(postal_code, &usersFound);
            sem_post(&users_sem);
            write(1, "Search finished\n", strlen("Search finished\n"));

            sprintf(buffer, "There are %d human beings at %s:\n", numUsersFound, postal_code);
            write(1, buffer, strlen(buffer));
            for (int i = 0; i < numUsersFound; i++) {
                sprintf(buffer, "%d %s\n", usersFound[i].id, usersFound[i].name);
                write(1, buffer, strlen(buffer));
            }

            bzero(data, DATA_SIZE);
            sprintf(data, "%d*", numUsersFound);
            int i = 0;
            char aux[DATA_SIZE];
            while (i < numUsersFound) {
                sprintf(aux, "%s*%d*", usersFound[i].name, usersFound[i].id);
                if (strlen(aux)+strlen(data) > DATA_SIZE) {
                    sendFrame(socket, "ATREIDES\0", 'L', data);
                    bzero(data, DATA_SIZE);
                    strcat(data, aux);
                    bzero(aux, DATA_SIZE);
                } else {
                    strcat(data, aux);
                    bzero(aux, DATA_SIZE);
                }
                i++;
            }
            sendFrame(socket, "ATREIDES\0", 'L', data);
            write(1, REPLY_SENT, strlen(REPLY_SENT));

            for (int i = 0; i < numUsersFound; i++) {
                free(usersFound[i].name);
            }
            free(usersFound);

            return 0;
            break;
        }
        case 'F':{
            //Receive image
            char imageName[31], md5sum[33], buffer[200];
            strcpy(imageName, strtok(data, "*"));
            int size = atoi(strtok(NULL, "*"));
            strcpy(md5sum, strtok(NULL, "*"));
            md5sum[32] = '\0';

            sem_wait(&user_list);
            char * name = getUsername(*myID);
            sem_post(&user_list);

            sprintf(buffer, "Received send %s from %s %d\n", imageName, name, *myID);
            write(1, buffer, strlen(buffer));

            bzero(buffer, 200);
            sprintf(buffer, "%s/%d.jpg", fileInfo.folder, *myID);
            remove(buffer);
            int imgfd = open(buffer, O_WRONLY | O_CREAT, 0777);
            int bytesLeft = size;
            char newFrame[FRAME_SIZE];
            while (bytesLeft > 0) {
                read(socket, newFrame, FRAME_SIZE);

                if (bytesLeft < DATA_SIZE) {
                    write(imgfd, &newFrame[SOURCE_SIZE+1], bytesLeft);
                } else {
                    write(imgfd, &newFrame[SOURCE_SIZE+1], DATA_SIZE);
                }
                bytesLeft -= DATA_SIZE;
            }
            close(imgfd);
            char * newMd5 = getMd5sum(buffer);
            if (strcmp(md5sum, newMd5) == 0) {
                sprintf(buffer, "Stored as %d.jpg\n", *myID);
                write(1, buffer, strlen(buffer));
                sendFrame(socket, "ATREIDES\0", 'I', "IMAGE OK\0");
            } else {
                sendFrame(socket, "ATREIDES\0", 'R', "IMAGE KO\0");
                remove(buffer);
            }
            free(newMd5);
            break;
        }
        case 'P': {
            //Send image
            int id = atoi(data);
            char * name = getUsername(*myID);
            sprintf(buffer, "Received photo %d from %s %d\n", id, name, *myID);
            write(1, buffer, strlen(buffer));

            char file_name[31];
            sprintf(file_name, "%s/%d.jpg", fileInfo.folder, id);
            int fd = open(file_name, O_RDONLY);
            if (fd == -1) {
                write(1, "No photo registered.\n", strlen("No photo registered.\n"));
                bzero(data, DATA_SIZE);
                sprintf(data, "FILE NOT FOUND");
                sendFrame(socket, "ATREIDES\0", 'F', data);
                write(1, REPLY_SENT, strlen(REPLY_SENT));
                break;
            }
            int size = getFileSize(file_name);
            char * md5sum = getMd5sum(file_name);
            bzero(data, DATA_SIZE);
            sprintf(data, "%d.jpg*%d*%s", id, size, md5sum);
            sendFrame(socket, "ATREIDES\0", 'F', data);
            sprintf(buffer, "Sending %d.jpg\n", id);
            write(1, buffer, strlen(buffer));
            int bytesLeft = size;
            bzero(data, DATA_SIZE);
            while (bytesLeft > 0) {
                if (bytesLeft < DATA_SIZE) {
                    read(fd, data, bytesLeft);
                } else {
                    read(fd, data, DATA_SIZE);
                }
                bytesLeft -= DATA_SIZE;
                if (sendFrame(socket, "ATREIDES\0", 'D', data) == 0) return 0;
                bzero(data, DATA_SIZE);
            }
            close(fd);
            free(md5sum);

            write(1, REPLY_SENT, strlen(REPLY_SENT));
            if (readFrameType(socket) == 'I') {
                write(1, "Photo successfully sent to Fremen.\n", strlen("Photo successfully sent to Fremen.\n"));
            } else {
                write(1, "Something went wrong sending the image\n", strlen("Something went wrong sending the image\n"));
            }

            break;
        }
        default:
            //Unrecognized type
            sendFrame(socket, "ATREIDES\0", 'Z', "ERROR T\0");
            break;
    }
    return 0;
}
