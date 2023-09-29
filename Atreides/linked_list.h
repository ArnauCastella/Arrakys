//
// Created by arnau on 12/05/2022.
//

#ifndef G10_P1_LINKED_LIST_H
#define G10_P1_LINKED_LIST_H
#include "types.h"

typedef struct Node {
    User user;
    struct Node* next;
}Node;

Node * head;
sem_t user_list;

void LL_addUser(User user);
void LL_removeUser(int id);
void LL_freeAll();
void LL_printList();
char * getUsername(int id);

#endif //G10_P1_LINKED_LIST_H
