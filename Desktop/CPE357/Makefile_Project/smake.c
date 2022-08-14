#ifndef STANDARD
#define STANDARD
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "rules.h"
#endif

char* read_line(FILE *file);
Rule *storeRules(FILE *f);


int main(int argc, char *argv[]) {
    Rule *find;
    Rule *root;
    FILE *f = fopen("Smakefile", "r");
    if(f == NULL) { 
        printf("make: *** No targets specified and no makefile found.  Stop.\n");
        exit(1);
    }

    root = storeRules(f);
    fclose(f);

    if(argc > 2) {
        printf("Wrong usage of smake.\n");
        exit(-1);
    } else if(argc == 2) {
        find = findRuleFromTarget(root, argv[1]);
        if(find == NULL) {
            printf("Specified target is not found.\n");
            exit(-1);
        }

        if(traverseRules(root, find) == 0) {
            printf("Target: %s is up to date.\n", find->target);
        }

    } else {

        /*actionn = root->actroot->head;

        while(actionn != NULL) {
            actionn = actionn->next;
        }*/
        if(traverseRules(root, root) == 0) {
            printf("Target: %s is up to date.\n", root->target);
        }

    }

    freeRules(root);
    return 0;
}

char* read_line(FILE *file) {
    int size = 70, index = 0;
    int c;
    char * ptr, *newptr;
    c = fgetc(file);
    
    if(c == '\0' || c == '\n') {
        return ptr = "";
    } 

    ptr = (char *) malloc(sizeof(char) * size);
    if(ptr == NULL) exit(1);
    while(c != EOF && c != '\n') {
        if(index >= size) {
            /*too many elems, need to increase size*/
            newptr = (char *) realloc(ptr, sizeof(char) * 2 * size);
            if(newptr == NULL) {
                free(ptr);
                exit(1);
            } 
            ptr = newptr;
            size *= 2;
        }

        ptr[index++] = c;

        c = fgetc(file);
    }

    /*last elem*/
    if(index >= size) {
        /*too many elems, need to increase size*/
        newptr = (char *) realloc(ptr, sizeof(char) * size + 1);
        if(newptr == NULL) {
            free(ptr);
            exit(1);
        }

        ptr = newptr;
        size++;
    }
    if(index == 0) {
        free(ptr);
        ptr = "";
        return ptr;
    }
    
    ptr[index] = '\0';

    return ptr; 
}

Rule *storeRules(FILE *f) {
    int lineNum;
    char *line, *targ;
    char *modLine;
    Rule *r = NULL, *root = NULL;
    Dep *depens = NULL;

    char *dupe;
    ActionList *aList = NULL;
    lineNum = 1;
    while(!feof(f)) {
        line = read_line(f); 
        
        if(line[0] != '\t') { 

            if(aList != NULL) { /*means that you have a new rule and need to add aList to prev*/
                root->actroot = aList;
                aList = NULL;
            }
            if(line[0] == '\0') { 
                continue; 
            }
            modLine = strdup(line); 
            targ = strtok(modLine, ":");

            if(strlen(targ) == strlen(line)) {
                printf("Makefile:%d: *** missing separator.  Stop.\n", lineNum);
                exit(1);
            } 

            free(line);

            targ = trim(targ); /*removing all white spaces from target*/

            line = strtok(NULL, ":"); /*line contains "main.o other.o" now*/
            depens = reverseDepList(getDependencies(line));

            r = createRule(targ, depens);
            r->next = NULL;

            if(root == NULL) {
                root = r;
                root->next = NULL;
            } else {
                r->next = root;
                root = r;
            }
            

        } else if(line[0] == '\t') {
            if(root == NULL) {
                printf("Makefile:%d: *** commands commence before first target.  Stop.\n", lineNum);
                exit(1);
            }
            
            dupe = strdup(line + 1);
            if(aList == NULL ) aList = (ActionList *)malloc(sizeof(ActionList));
            free(line);
            line = NULL;
            aList = addAction(aList, dupe);

        }
        

    }

    root = reverseLinkedList(root);
    return root;
}
