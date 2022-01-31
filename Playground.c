#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (argc == 0) {
        printf("Usage: %s ARG\n", argv[0]);
        return 1;
    }

    // C does not have int* functionality !!
    // int r = 1;
    // int &a = r;
    // printf("%d", a);
    // printf("%d", r);

    struct user {
        int value;
    };

    struct user *jim = malloc(sizeof(struct user));
    jim->value = 1;
    printf("%d", jim->value);
    printf("%d", (*jim).value);



    
}
