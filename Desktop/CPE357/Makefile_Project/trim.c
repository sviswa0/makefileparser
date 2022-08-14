#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <ctype.h>

char *trim(char *s) {
    char *start = NULL;
    int len = strlen(s);
    int i = 0;
    int j;

    for (i = 0; i < len; i++) {
        if (! isspace(s[i])) {
            break;
        }
    }

    if(i == len) {
        printf("Only whitespaces found");
        return NULL;
    }

    printf("i is %d\n", i);
    printf("s[i] = %c\n", s[i]);

    start = s + i;

    //printf("start is %s|\n", start);
    j = i;
    //printf("j is %d\n", j);
    while(j < len) {
        if(isspace(s[j])) {
            s[j] = '\0';
            break;
        } 
        j++;
    }
    /*

    j = i;

    while(j < len) {
        if(s[j] == ' ') {
            start[j] = '\0';
            return start;
        } else {
            j++;
        }
    }

    return start;*/

    return start;

}


int main(int argc, char *argv[]) {
    char *d = strdup("    Hello    ");
    char *s = trim(d);

    printf("%s|\n", s);

}