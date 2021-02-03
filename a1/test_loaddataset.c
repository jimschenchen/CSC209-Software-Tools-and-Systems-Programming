#include <stdio.h>
#include <stdlib.h>
#include "knn.h"

/* 
*
 */
unsigned char image[NUM_PIXELS];
unsigned char dataset[MAX_SIZE][NUM_PIXELS];
unsigned char labels[MAX_SIZE];

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s imagefile\n", argv[0]);
        exit(1);
    }

    int data_num = load_dataset(argv[1], dataset, labels);

    for (int i =0; i < data_num; i++) {
        printf("Labels: %hhu\n", labels[0]);
        print_image(dataset[0]);
    }
    return 0;
}
