#include "input.h"

int countChar(char c, char* string) {
    int result = 0;
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] == c && string[i+1] != c) {
            result++;
        }
    }
    return result;
}

int checkValidCommand(char * input) {
    char command[10];
    int i;
    for (i = 0; input[i] != ' ' && input[i] != '\0'; i++) {
        command[i] = input[i];
    }
    command[i] = '\0';

    if ((strcasecmp(command,"login") == 0)||(strcasecmp(command,"search") == 0)||(strcasecmp(command,"send") == 0)||
        (strcasecmp(command,"photo") == 0)||(strcasecmp(command,"logout") == 0)) {
        return 1;
    }
    return 0;
}

int checkInput(char * input){
    int numArgs = 0, i;
    char com[10];

    for (i = 0; input[i] != ' ' && input[i] != '\0'; i++){
        com[i] = input[i];
    }
    com[i] = '\0';

    numArgs = countChar(' ', input);

    if(strcasecmp(com,"login") == 0){
        if(numArgs < 2){
            return -2;
        }else if(numArgs > 2){
            return -1;
        }else{
            return 1;
        }
    }else if(strcasecmp(com,"search") == 0){
        if(numArgs < 1){
            return -2;
        }else if(numArgs > 1){
            return -1;
        }else{
            return 2;
        }
    }else if(strcasecmp(com,"send") == 0){
        if(numArgs < 1){
            return -2;
        }else if(numArgs > 1){
            return -1;
        }else{
            return 3;
        }
    }else if(strcasecmp(com,"photo") == 0){
        if(numArgs < 1){
            return -2;
        }else if(numArgs > 1){
            return -1;
        }else{
            return 4;
        }
    }else if(strcasecmp(com,"logout") == 0){
        if(numArgs > 0){
            return -1;
        }else{
            return 0;
        }
    }
    return -3;
}
