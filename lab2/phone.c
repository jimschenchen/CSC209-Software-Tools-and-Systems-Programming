#include <stdio.h>

/*
 * Sample usage:
 * $ gcc -Wall -std=gnu99 -g -o phone phone.c
 * $ ./phone
 */
int main() {
    char phone[11];
    int index;
    printf("Type your phone number and index please:\n");
    scanf("%s%d", phone, &index);

    if (index == -1) {
        printf("%s\n", phone);
        return 0;
    }
    if (index < -1 || index > 9) {
        printf("ERROR\n");
        return 1;
    }

    printf("%c\n", phone[index]);
    return 0;
}