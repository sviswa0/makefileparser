#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]) {

    if(argc >= 2) {
        char *ptr = argv[1];

        int i = 0;

        while(ptr[i] != '\0') {
            printf("%c", ptr[i++]);
        }

        if(strcmp("hello\n", ptr) == 0) {
            printf("it's a match!");
        }
    }

    return 0;
}