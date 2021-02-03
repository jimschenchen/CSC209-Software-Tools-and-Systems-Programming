#include <stdio.h>
#include <stdlib.h>
#include "knn.h"

/* 
*
 */


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s imagefile\n", argv[0]);
        exit(1);
    }

    unsigned char image1[NUM_PIXELS];
    unsigned char image2[NUM_PIXELS];

    load_image("datasets/training/3-1.pgm", image1);
    load_image("datasets/training/23-1.pgm", image2);

    printf("%f\n", distance(image1, image2));

    unsigned char image3[NUM_PIXELS];
    unsigned char image4[NUM_PIXELS];
    unsigned char image5[NUM_PIXELS];
    load_image("datasets/training/40-1.pgm", image3);
    load_image("datasets/training/17-8.pgm", image4);
    load_image("datasets/training/35-5.pgm", image5);

    printf("%f\n", distance(image1, image3));
    printf("%f\n", distance(image3, image4));
    printf("%f\n", distance(image4, image5));

    return 0;
}
