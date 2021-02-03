#include <stdio.h>
#include <math.h>    // Need this for sqrt()
#include <stdlib.h>
#include <string.h>

#include "knn.h"

/* Print the image to standard output in the pgmformat.  
 * (Use diff -w to compare the printed output to the original image)
 */
void print_image(unsigned char *image) {
    printf("P2\n %d %d\n 255\n", WIDTH, HEIGHT);
    for (int i = 0; i < NUM_PIXELS; i++) {
        printf("%d ", image[i]);
        if ((i + 1) % WIDTH == 0) {
            printf("\n");
        }
    }
}

/* Return the label from an image filename.
 * The image filenames all have the same format:
 *    <image number>-<label>.pgm
 * The label in the file name is the digit that the image represents
 */
unsigned char get_label(char *filename) {
    // Find the dash in the filename
    char *dash_char = strstr(filename, "-");
    // Convert the number after the dash into a integer
    return (char) atoi(dash_char + 1);
}

/* ******************************************************************
 * Complete the remaining functions below
 * ******************************************************************/


/* Read a pgm image from filename, storing its pixels
 * in the array img.
 * It is recommended to use fscanf to read the values from the file. First, 
 * consume the header information.  You should declare two temporary variables
 * to hold the width and height fields. This allows you to use the format string
 * "P2 %d %d 255 "
 * 
 * To read in each pixel value, use the format string "%hhu " which will store
 * the number it reads in an an unsigned character.
 * (Note that the img array is a 1D array of length WIDTH*HEIGHT.)
 */
void load_image(char *filename, unsigned char *img) {
    // Open corresponding image file, read in header (which we will discard)
    FILE *f2 = fopen(filename, "r");
    if (f2 == NULL) {
        perror("fopen");
        exit(1);
    }
    // consume width and height
	int width, height;
    fscanf(f2, "P2 %d %d 255 ", &width, &height);

    // save the image body
    int index = 0;
    while (fscanf(f2, "%hhu ", &img[index]) == 1) {
        index += 1;
    }

    // closed stream
    fclose(f2);
}


/**
 * Load a full dataset into a 2D array called dataset.
 *
 * The image files to load are given in filename where
 * each image file name is on a separate line.
 * 
 * For each image i:
 *  - read the pixels into row i (using load_image)
 *  - set the image label in labels[i] (using get_label)
 * 
 * Return number of images read.
 */
int load_dataset(char *filename, 
                 unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                 unsigned char *labels) {
    // We expect the file to hold up to MAX_SIZE number of file names
    FILE *f1 = fopen(filename, "r");
    if (f1 == NULL) {
        perror("fopen");
        exit(1);
    }

    // Load images to dataset
    int index = -1;
    char imagefilename[MAX_NAME];
    while(fscanf(f1, "%s\n", imagefilename) == 1){
        // index increasement
        index += 1;

        // add labels
        labels[index] = get_label(imagefilename);

        // load image to dataset
        load_image(imagefilename, dataset[index]);
    }

    fclose(f1);
    return index + 1;
}

/** 
 * Return the euclidean distance between the image pixels in the image
 * a and b.  (See handout for the euclidean distance function)
 */
double distance(unsigned char *a, unsigned char *b) {
    double base = 0;
    for (int i =0; i < NUM_PIXELS; i++) {
        base += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(base);
}


/**
 * Helper Func
 * find the smallest; return the smaller index if tie
 */
int get_smallest_labels(int *frequency, int listsize) {
    int smallest = frequency[0];
    int smallest_index = 0;
    for (int i =0; i < listsize; i++) {
        if (frequency[i] > smallest) {
            smallest = frequency[i];
            smallest_index = i;
        }
    }
    return smallest_index;
}


/**
 * Return the most frequent label of the K most similar images to "input"
 * in the dataset
 * Parameters:
 *  - input - an array of NUM_PIXELS unsigned chars containing the image to test
 *  - K - an int that determines how many training images are in the 
 *        K-most-similar set
 *  - dataset - a 2D array containing the set of training images.
 *  - labels - the array of labels that correspond to the training dataset
 *  - training_size - the number of images that are actually stored in dataset
 * 
 * Steps
 *   (1) Find the K images in dataset that have the smallest distance to input
 *         When evaluating an image to decide whether it belongs in the set of 
 *         K closest images, it will only replace an image in the set if its
 *         distance to the test image is strictly less than all of the images in 
 *         the current K closest images.
 *   (2) Count the frequencies of the labels in the K images
 *   (3) Return the most frequent label of these K images
 *         In the case of a tie, return the smallest value digit.
 */ 

int knn_predict(unsigned char *input, int K,
                unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                unsigned char *labels,
                int training_size) {
    // arr init
    double distance_arr[training_size];
    int index_arr[training_size];

    // store distance
    for (int i = 0; i < training_size; i++) {
        distance_arr[i] = distance(input, dataset[i]);
        index_arr[i] = i;
    }

    // move k smallest to front; selection sort
    for (int i = 0; i < K; i++) {
        // find the smallest
        int svalue = distance_arr[training_size - 1];
        int sindex = training_size - 1;
        for (int j = training_size - 1; j >= i; j--) {
            if (distance_arr[j] < svalue) {
                svalue = distance_arr[j];
                sindex = j;
            }
        }
        // swap sindex with i
        int temp_value = distance_arr[i];
        distance_arr[i] = distance_arr[sindex];
        distance_arr[sindex] = temp_value;
        int temp_index = index_arr[i];
        index_arr[i] = index_arr[sindex];
        index_arr[sindex] = temp_index;
    }

    // init frequency !warm: arr[] must init with all 0
    int frequency[10];
    for (int i =0; i< 10; i++) {
        frequency[i] = 0;
    }

    // count the frequency
    for (int i = 0; i < K; i++) {
        int index = labels[index_arr[i]];
        frequency[index] += 1;

    }

    return get_smallest_labels(frequency, 10);
}