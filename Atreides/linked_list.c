//
// Created by arnau on 12/05/2022.
//

#include "linked_list.h"

void LL_addUser(User user) {
    Node * new_node = (Node *) malloc (sizeof (Node));
    Node * last = head;
    new_node->user.id = user.id;
    new_node->user.name = (char *) malloc (sizeof (char) * strlen(user.name)+1);
    new_node->user.postal_code = (char *) malloc (sizeof (char) * strlen(user.postal_code)+1);
    strcpy(new_node->user.name, user.name);
    strcpy(new_node->user.postal_code, user.postal_code);
    new_node->next = NULL;
    if (head == NULL) {
        head = new_node;
        return;
    }
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = new_node;
}

void LL_removeUser(int id) {
    Node * temp = head, *prev;
    if (temp != NULL && temp->user.id == id) {
        head = temp->next;
        free(temp->user.name);
        free(temp->user.postal_code);
        free(temp);
        return;
    }

    while (temp != NULL && temp->user.id != id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;

    prev->next = temp->next;
    free(temp->user.name);
    free(temp->user.postal_code);
    free(temp);
}

void LL_printList() {
    printf("Printing list\n");
    Node * node = head;
    char buffer[100];
    while (node != NULL) {
        sprintf(buffer, "%d %s %s\n", node->user.id, node->user.name, node->user.postal_code);
        write(1, buffer, strlen(buffer));
        node = node->next;
    }
}

void LL_freeAll() {
    Node * current = head, *next;

    while (current != NULL) {
        next = current->next;

        free(current->user.name);
        free(current->user.postal_code);
        free(current);
        current = next;
    }
    head = NULL;
}

char * getUsername(int id){
    Node * current = head;
    while (current->user.id != id) {
        current = current->next;
    }
    return current->user.name;
}