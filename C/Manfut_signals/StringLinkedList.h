//
// Created by pere on 30/12/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LINKEDLIST_STRINGLINKEDLIST_H
#define LINKEDLIST_STRINGLINKEDLIST_H

    typedef struct node Node;
    typedef struct list List;
    List * LinkedList_init();
    void InsertIntoLinkedList(char * value, List * list);
    int LinkedListSize(List * list);
    void printLinkedList(List * list);
    void reverseLinkedList(List * list);
    void destroyLinkedList(List * list);
    Node * private_createnode(char * data);
    void private_delete(char * data, List * list);
    void private_InsertNode(char *value, List *list, Node *current);

#endif //LINKEDLIST_STRINGLINKEDLIST_H
struct node {
    char * data;
    struct node * next;
};

struct list {
    Node * head;
    int max;
};



Node * private_createnode(char *data){
    Node * newNode = malloc(sizeof(Node));
    if (!newNode) {
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}
void private_delete(char * data, List * list){
    Node * current = list->head;
    Node * previous = current;
    while(current != NULL){
        if(strcmp(current->data,data)==0){
            previous->next = current->next;
            if(current == list->head)
                list->head = current->next;
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}
char * private_getFirst(List *list){
    Node * current = list->head;
    if(list->head == NULL){
        perror("EmptyList");
        exit(EXIT_FAILURE);
    }
    if(current != NULL){
        return current->data;
    } else{
        perror("EmptyList");
        exit(EXIT_FAILURE);
    }
}
void private_InsertNode(char *value, List *list, Node *current) {
    current = list->head;
    while (current->next!=NULL){
        current = current->next;
    }
    current->next = private_createnode(value);
}



List * LinkedList_init(int max){
    List * list = malloc(sizeof(List));
    if (!list) {
        return NULL;
    }
    list->head = NULL;
    list->max=(max<=0 )? -1 : max;
    return list;
}
void InsertIntoLinkedList(char *value, List * list){
    Node * current = NULL;
    if(list->head == NULL){
        list->head = private_createnode(value);
    }else if (list->max <=0 ){
        private_InsertNode(value, list, current);
    }else if(LinkedListSize(list)<list->max){
        private_InsertNode(value, list, current);
    } else{
        perror("Exceded size LinkedList (in InsertIntoLinkedList())");
        exit(EXIT_FAILURE);
    }


}
void reverseLinkedList(List * list){
    Node * reversed = NULL;
    Node * current = list->head;
    Node * temp = NULL;
    while(current != NULL){
        temp = current;
        current = current->next;
        temp->next = reversed;
        reversed = temp;
    }
    list->head = reversed;
}
int LinkedListSize(List * list){
    int size=0;
    Node * current = list->head;
    if(list->head == NULL)
        return size;

    for(; current != NULL; current = current->next) {
        size++;
    }
    return size;
}
char *getFirstItem(List *list){
    char * item_return = private_getFirst(list);
    private_delete(item_return,list);
    return item_return;
}
void destroyLinkedList(List * list){
    Node * current = list->head;
    Node * next = NULL;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
void printLinkedList(List * list) {
    Node * current = list->head;
    if(list->head == NULL){
        printf("[ ]");
        return;
    }
    printf("[");
    for(; current != NULL; current = current->next) {
        printf(" %s, ", current->data);
    }
    printf("]\n");
}














