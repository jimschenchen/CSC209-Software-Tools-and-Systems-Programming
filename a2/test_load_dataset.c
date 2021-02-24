#include <stdio.h>
#include <stdlib.h>
#include "dectree.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s imagefile\n", argv[0]);
        exit(1);
    }

    Dataset *ds = load_dataset(argv[1]);
    if (ds == NULL) {
        fprintf(stderr, "Error: load_dataset failed\n");
        return 1;
    }
    printf("%d\n", ds->num_items);


    int num_displays = 10;
    for (int j = 0; j < num_displays; j ++) {
        Image img = ds->images[j];
        for (int i = 0; i < NUM_PIXELS; i++) {
            printf("%d", 1 ? img.data[i] == 255 : 0);
            if ((i + 1) % WIDTH == 0) {
                printf("\n");
            }
        }
        printf("label: %d\n", ds->labels[j]);
    }

    return 0;
}
