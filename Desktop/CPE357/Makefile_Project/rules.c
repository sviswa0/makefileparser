#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "rules.h"

char *trim(char *s) {
    char *start = NULL;
    int len = strlen(s);
    int i = 0;
    int j;

    for (i = 0; i < len; i++) {
        if (s[i] != ' ') {
            break;
        }
    }

    if(i == len) {
        return NULL;
    }

    start = s + i;
    j = i;
    while(j < len) {
        if(s[j] == ' ') { /*replacing first instance of space with \0*/
            s[j] = '\0';
            break;
        } 
        j++;
    }

    return start;

}

Rule *createRule(char *t, Dep *root) { 
    Rule *r = (Rule *)malloc(sizeof(Rule));

    r->target = t;
    r->deproot = root;

    return r;
}

ActionList* addAction(ActionList *lst, char *act) { 
    Action *trav;
    Action *a = (Action *) malloc(sizeof(Action));
    a->name = act;
    a->next = NULL;
    if(lst->head == NULL) {
        lst->head = a;
    } else {
        trav = lst->head;

        while(trav->next != NULL) {
            trav = trav->next;
        }

        trav->next = a;
    }

    return lst;

}

Dep * getDependencies(char *d) { 
    char *single;

    Dep *root;
    Dep *newNode;

    single = strtok(d, " ");
    if(single == NULL) return NULL;
    single = trim(single);
    root = (Dep *)malloc(sizeof(Dep));

    root->name = single;
    root->next = NULL;

    while((single = strtok(NULL, " ")) != NULL) {
        newNode = (Dep *)malloc(sizeof(Dep));
        newNode->name = single;
        newNode->next = root;
        root = newNode;
    }

    return root;


}

Rule *findRuleFromTarget(Rule *root, char *targName) { 
    Rule *node = root;

    while(node != NULL) {
        if(strcmp(targName, node->target) == 0) return node;

        node = node->next;

    }

    return NULL;
} 

int fileExists(char *fName) {
    FILE *f = fopen(fName, "r");

    if(f == NULL) {
        return 0; /*file doesn't exist*/
    } else {
        fclose(f);
        return 1;
    }

}
int isUpdated(char *target, char *dependency) {
    struct stat tBuf, dBuf;

    if(fileExists(target) == 0) return 1;

    if(stat(target, &tBuf) || stat(dependency, &dBuf)) {
        perror("Couldn't determine if updated");
        exit(-1);
    } else {
        time_t t = tBuf.st_mtime;
        time_t d = dBuf.st_mtime;

        if((t - d) < 0) return 1; /*target is older than dependency*/
        else return 0;
    }
}


Dep *reverseDepList(Dep *root) { 
    /*need 3 pointers:
    //1. current spot
    //2. previous spot (which is set to curr.next)
    //3. to traverse to the actual next*/

    Dep *cur, *prev = NULL, *after = NULL;
    cur = root;

    while(cur != NULL) {
        /*save and store the next address since cur->next = prev soon*/
        after = cur->next;
        /*set the previous to cur's next (switch the link)*/
        cur->next = prev;

        /*setting up for next iteration*/
        prev = cur;
        cur = after;
    }

    /*all links have been modified*/
    root = prev; /*has the root*/
    return root;
    
}

Rule *reverseLinkedList(Rule *root) {
    /*need 3 pointers:
    //1. current spot
    //2. previous spot (which is set to curr.next)
    //3. to traverse to the actual next*/

    Rule *cur, *prev = NULL, *after = NULL;
    cur = root;

    while(cur != NULL) {
        /*save and store the next address since cur->next = prev soon*/
        after = cur->next;
        /*set the previous to cur's next (switch the link)*/
        cur->next = prev;

        /*setting up for next iteration*/
        prev = cur;
        cur = after;
    }

    /*all links have been modified*/
    root = prev; /*has the root*/
    return root;
    

    
}

int validateDependency(Rule *root, Dep *dptr) { 
    /*2 -> both, 1 -> target, -1 -> file, 0 -> neither*/
    Rule *cur = root;

    /*search through rules for a target with the same name
    int valid = 0; //0 = not a file nor a target*/

    int val = 0;
    
    while(cur != NULL) {
        if(strcmp(cur->target, dptr->name) == 0) {
            val = 1; 
            break;
        }
        cur = cur->next;
    }

    if(fileExists(dptr->name) == 1) {

        if(val) return 2;
        return -1;
    }

    return val; 
}

Rule *findRule(Rule *root, char *depName) { 
    Rule *node = root;

    while(node != NULL) {
        if(strcmp(node->target, depName) == 0) {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

void executeActions(ActionList *ptr) { 
    Action *a = ptr->head;
    int i = 0;
    while(a != NULL) {
        printf("%s\n", a->name);
        
        i = system(a->name);

        /*32512 is for issue with command inside the terminal*/
        if(i == -1 || i == 32512) {
            exit(-1);
        }

        a = a->next;
    }
}

int traverseRules(Rule *root, Rule *trav) { 
    Dep *depList = trav->deproot;

    Rule *targetDep;
    int validated;
    int updated = 0;

    
    
    if(depList == NULL) {
        updated = 1;
    } else {
        while(depList != NULL) {
            
            validated = validateDependency(root, depList);

            if(validated == 0) {
                printf("make: *** No rule to make target `%s', needed by `%s'.  Stop.\n", depList->name, trav->target);
                exit(-1);
            } 
            
            if(validated == 1 || validated == 2) {
                
                targetDep = findRule(root, depList->name);
                if(traverseRules(root, targetDep)) {
                    updated = 1;
                } else updated = 0;
                
            } 
            
            if(validated == -1 || validated == 2) {
                
                /*file not target*/
                if(isUpdated(trav->target, depList->name)) {
                    updated = 1;
                }
                
            }   

            depList = depList->next;
        }
    }
    if(fileExists(trav->target) == 0) {
        updated = 1;
    }

    if(updated == 0) {
        return 0;
    } else {
        /*perform the actions*/

        executeActions(trav->actroot);
        return 1;
    }

}
void printRules(Rule *trav) { 
    Dep *d;
    Action *a;
    Rule *root = trav;
    while(trav != NULL) {
        printf("Target: %s\n", trav->target);
        
        printf("Dependencies: ");

        d = trav->deproot;
        
        while(d != NULL) {
            printf("%s -> %d\n", d->name, validateDependency(root, d));

            d = d->next;
        }
        printf("\n");

        a = root->actroot->head;
        printf("Actions: ");
        while(a != NULL) {
            printf("%s|", a->name);
            a = a->next;
        }

        printf("\n");

        trav = trav->next;
    }
}

void freeAction(struct List *root) { 
    struct List *node = root;
    if(node == NULL) return;

    freeAction(node->next);
    free(node->name);
    free(node);
}
void freeDep(struct List *root) { 
    struct List *node = root;
    if(node == NULL) return;

    freeDep(node->next);
    free(node);
}
void freeRules(Rule *root) { 
    Rule *node = root;

    if(node == NULL) return;

    freeRules(node->next);
    freeDep(node->deproot);
    freeAction(node->actroot->head);
    free(node->actroot);
    free(node->target);
    free(node);
}
