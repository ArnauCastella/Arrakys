#include "file.h"
#include "types.h"
#include "input.h"
#include "commands.h"

int exit_all;

void exitRoutine(){
    write(1, BYE, strlen(BYE));
    for (int i = 0; i < 4; i++){
        close(i);
    }
    fileData.cleanup_time ? free(fileData.cleanup_time) : NULL;
    fileData.ip ? free(fileData.ip) : NULL;
    fileData.port ? free(fileData.port) : NULL;
    fileData.folder ? free(fileData.folder) : NULL;
    myUser.name ? free(myUser.name) : NULL;
    myUser.postal_code ? free(myUser.postal_code) : NULL;
    close(sockfd);
}

void ctrlc() {
    exitRoutine();
    exit(0);
}

void disconnected() {
    write(1, "Atreides connection ended.\n", strlen("Atreides connection ended.\n"));
    if (loggedIn) write(1, "You are no longer logged in!\n", strlen("You are no longer logged in!\n"));
    loggedIn = 0;
    signal(SIGPIPE, disconnected);
}

void cleanFiles() {
    DIR * d;
    struct dirent * dir;
    char buffer[300];

    char dir_path[100];
    sprintf(dir_path, "%s", fileData.folder);
    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            char extension[5];
            strcpy(extension, &dir->d_name[strlen(dir->d_name)-4]);
            if (strcmp(extension, ".jpg") == 0){
                sprintf(buffer, "%s/%s", fileData.folder, dir->d_name);
                if (remove(buffer) != 0) {
                    sprintf(buffer, "\nError removing %s\n", dir->d_name);
                    write(1, buffer, strlen(buffer));
                } else {
                    sprintf(buffer, "\nRemoved %s\n", dir->d_name);
                    write(1, buffer, strlen(buffer));
                }
            }
        }
        closedir(d);
        write(1, "Files cleaned!\n", strlen("Files cleaned!\n"));
    }
    signal(SIGALRM, cleanFiles);
    alarm(atoi(fileData.cleanup_time));
}

int main (int argc, char * argv[]) {

    loggedIn = 0;
    int file = 0;
    exit_all = 0;
    char buffer[100], input[100];

    signal(SIGINT, ctrlc);
    signal(SIGPIPE, disconnected);
    signal(SIGALRM, cleanFiles);
    signal(SIGTERM, ctrlc);

    if (argc < 2) {
        write(1, LESS_ARGS_ERROR, strlen(LESS_ARGS_ERROR));
        return -1;
    } else if (argc > 3) {
        write(1, MORE_ARGS_ERROR, strlen(MORE_ARGS_ERROR));
        return -1;
    }

    file = open(argv[1], O_RDONLY);
    if (file < 0) {
        write(1, FILE_ERROR, strlen(FILE_ERROR));
        return -1;
    }

    fileData = readFile(file);
    alarm(atoi(fileData.cleanup_time));
    close(file);
    sprintf(buffer, "\n%s", WELCOME);
    write(1, buffer, strlen(buffer));

    struct stat st = {0};
    if (stat(fileData.folder, &st) == -1) {
        mkdir(fileData.folder, 0777);
    }

    int command;
    while (!exit_all) {
        write(1, "\n$ ", 3);
        bzero(input, 100);
        int input_size = read(0, input, 100);
        if (input_size == 0) {
            do {
                bzero(input, 100);
                write(1, "\n$ ", 3);
                input_size = read(0, input, 100);
            } while (input_size == 0);
        }
        input[strlen(input)-1] = '\0';
        if (exit_all) break;
        if (checkValidCommand(input)) {
            command = checkInput(input);
            switch (command) {
                case -1:
                    sprintf(buffer, "%s", MORE_ARGS_ERROR);
                    write(1, buffer, strlen(buffer));
                    break;
                case -2:
                    sprintf(buffer, "%s", LESS_ARGS_ERROR);
                    write(1, buffer, strlen(buffer));
                    break;
                case 0:
                    if (loggedIn) {
                        logout();
                    }
                    exit_all = 1;

                    break;
                case 1: {
                    //LOGIN
                    if (loggedIn) {
                        write(1, "You are already logged in!\n", strlen("You are already logged in!\n"));
                        break;
                    }
                    char * name, * postal_code, * aux;
                    strtok(input, " ");
                    aux = strtok(NULL, " ");
                    name = (char *) malloc (sizeof (char) * (strlen(aux)+1));
                    strcpy(name,aux);
                    aux = strtok(NULL, " ");
                    postal_code = (char *) malloc (sizeof (char) * (strlen(aux)+1));
                    strcpy(postal_code,aux);
                    login(name, postal_code);

                    name ? free(name) : NULL;
                    postal_code ? free(postal_code) : NULL;
                    break;
                }
                case 2: {
                    //SEARCH
                    if (!loggedIn) {
                        write(1, "You need to be logged in!\n", strlen("You need to be logged in!\n"));
                        break;
                    }
                    char * postal_code, * aux;
                    strtok(input, " ");
                    aux = strtok(NULL, " ");
                    postal_code = (char *) malloc (sizeof (char) * strlen(aux)+1);
                    strcpy(postal_code,aux);

                    search(postal_code);

                    postal_code ? free(postal_code) : NULL;
                    break;
                }
                case 3: {
                    //SEND
                    if (!loggedIn) {
                        write(1, "You need to be logged in!\n", strlen("You need to be logged in!\n"));
                        break;
                    }
                    char * photo_name, * aux;
                    strtok(input, " ");
                    aux = strtok(NULL, " ");
                    photo_name = (char *) malloc (sizeof (char) * strlen(aux)+1);
                    strcpy(photo_name,aux);

                    sendPhoto(photo_name);
                    photo_name ? free(photo_name) : NULL;
                    break;
                }
                case 4: {
                    //PHOTO
                    if (!loggedIn) {
                        write(1, "You need to be logged in!\n", strlen("You need to be logged in!\n"));
                        break;
                    }
                    strtok(input, " ");
                    int id = atoi(strtok(NULL, " "));

                    requestPhoto(id);
                    break;
                }
            }
        } else {
            pid_t cpid = fork();
            switch (cpid) {
                case -1:
                    write(1,FORK_ERROR, strlen(FORK_ERROR));
                    break;
                case 0:
                    execl("/bin/sh", "/bin/sh", "-c", input, 0);
                    exit(0);
                    break;
                default:
                    waitpid(cpid,NULL,0);
            }
        }
    }
    exitRoutine();

    return 0;
}
