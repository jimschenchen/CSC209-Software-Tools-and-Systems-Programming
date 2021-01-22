#include <stdio.h>

int main() {
    char phone[11];
    int index;
    int ret = 0;

    scanf("%s", phone);
    while(scanf("%d", &index) != EOF) {
        if (index == -1) {
            printf("%s\n", phone);
        } else if (index < -1 || index > 9) {
            printf("ERROR\n");
            ret = 1;
        } else {
            printf("%c\n", phone[index]);
        }
    }
    return ret;
}