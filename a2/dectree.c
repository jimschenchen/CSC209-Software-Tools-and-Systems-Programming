/*
 * This code is provided solely for the personal and private use of students
 * taking the CSC209H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Mustafa Quraish, Bianca Schroeder, Karen Reid
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2021 Karen Reid
 */

#include "dectree.h"

/**
 * Load the binary file, filename into a Dataset and return a pointer to 
 * the Dataset. The binary file format is as follows:
 *
 *     -   4 bytes : `N`: Number of images / labels in the file
 *     -   1 byte  : Image 1 label
 *     - NUM_PIXELS bytes : Image 1 data (WIDTHxWIDTH)
 *          ...
 *     -   1 byte  : Image N label
 *     - NUM_PIXELS bytes : Image N data (WIDTHxWIDTH)
 *
 * You can set the `sx` and `sy` values for all the images to WIDTH. 
 * Use the NUM_PIXELS and WIDTH constants defined in dectree.h
 */
Dataset *load_dataset(const char *filename) {
    // TODO: Allocate data, read image data / labels, return
    int error;

    // Malloc dataset
    Dataset *ds = malloc(sizeof(Dataset));

    // Open file
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    // Read headers
    error = fread(&ds->num_items, sizeof(int), 1, file);
    if (error != 1) {
        fprintf(stderr, "Error: could not fread header\n");
        return NULL;
    }

    // Malloc images, labels
    ds->images = malloc(sizeof(Image) * ds->num_items);
    ds->labels = malloc(sizeof(unsigned char) * ds->num_items);

    // Read data
    for(int i = 0; i < ds->num_items; i ++) {
        // Read labels
        error = fread(&ds->labels[i], 1, 1, file);
        if (error != 1) {
            fprintf(stderr, "Error: could not fread %d labels\n", i);
            return NULL;
        }

        // Read images
        ds->images[i].sx = WIDTH;
        ds->images[i].sy = WIDTH;
        // Malloc images data
        ds->images[i].data = malloc(sizeof(unsigned char) * NUM_PIXELS);
        error = fread(ds->images[i].data, sizeof(unsigned char), NUM_PIXELS, file);
        if (error != NUM_PIXELS) {
            fprintf(stderr, "Error: could not fread %d images\n", i);
            return NULL;
        }
    }

    // Close file
    error = fclose(file);
    if (error != 0) {
        perror("fclose");
        exit(1);
    }

    return ds;
}

/**
 * Compute and return the Gini impurity of M images at a given pixel
 * The M images to analyze are identified by the indices array. The M
 * elements of the indices array are indices into data.
 * This is the objective function that you will use to identify the best 
 * pixel on which to split the dataset when building the decision tree.
 *
 * Note that the gini_impurity implemented here can evaluate to NAN 
 * (Not A Number) and will return that value. Your implementation of the 
 * decision trees should ensure that a pixel whose gini_impurity evaluates 
 * to NAN is not used to split the data.  (see find_best_split)
 * 
 * DO NOT CHANGE THIS FUNCTION; It is already implemented for you.
 */
double gini_impurity(Dataset *data, int M, int *indices, int pixel) {
    int a_freq[10] = {0}, a_count = 0;
    int b_freq[10] = {0}, b_count = 0;

    for (int i = 0; i < M; i++) {
        int img_idx = indices[i];

        // The pixels are always either 0 or 255, but using < 128 for generality.
        if (data->images[img_idx].data[pixel] < 128) {
            a_freq[data->labels[img_idx]]++;
            a_count++;
        } else {
            b_freq[data->labels[img_idx]]++;
            b_count++;
        }
    }

    double a_gini = 0, b_gini = 0;
    for (int i = 0; i < 10; i++) {
        double a_i = ((double)a_freq[i]) / ((double)a_count);
        double b_i = ((double)b_freq[i]) / ((double)b_count);
        a_gini += a_i * (1 - a_i);
        b_gini += b_i * (1 - b_i);
    }

    // Weighted average of gini impurity of children
    return (a_gini * a_count + b_gini * b_count) / M;
}

/**
 * Given a subset of M images and the array of their corresponding indices, 
 * find and use the last two parameters (label and freq) to store the most
 * frequent label in the set and its frequency.
 *
 * - The most frequent label (between 0 and 9) will be stored in `*label`
 * - The frequency of this label within the subset will be stored in `*freq`
 * 
 * If multiple labels have the same maximal frequency, return the smallest one.
 */
void get_most_frequent(Dataset *data, int M, int *indices, int *label, int *freq) {
    // TODO: Set the correct values and return
    int freq_temp[10] = {0};

    // Count freq of each label
    for (int i = 0; i < M; i++) {
        int img_idx = indices[i];
        freq_temp[data->labels[img_idx]]++;
    }

    // Find max of freq
    int max_i = 0;
    for (int i = 0; i < 10; i++) {
        if (freq_temp[i] > freq_temp[max_i]) {
            max_i = i;
        }
    }
    
    *label = max_i;
    *freq = freq_temp[max_i];
}

/**
 * Given a subset of M images as defined by their indices, find and return
 * the best pixel to split the data. The best pixel is the one which
 * has the minimum Gini impurity as computed by `gini_impurity()` and 
 * is not NAN. (See handout for more information)
 * 
 * The return value will be a number between 0-783 (inclusive), representing
 *  the pixel the M images should be split based on.
 * 
 * If multiple pixels have the same minimal Gini impurity, return the smallest.
 */
int find_best_split(Dataset *data, int M, int *indices) {
    // TODO: Return the correct pixel
    int min_i = INFINITY;
    double min_v = INFINITY;
    for (int i = 0; i < 784; i++) {
        double gini_v = gini_impurity(data, M, indices, i);

        // Exclude NAN INFINITY (JUST FOR SAFETY)
        if (gini_v == NAN || gini_v == INFINITY) {
            continue;
        }

        // NAN auto filter out by <
        if (gini_v < min_v) {
            min_v = gini_v;
            min_i = i;
        }
    }

    if (min_i == NAN || min_i == INFINITY) {
        fprintf(stderr, "ERROR: find_best_split cannot find an appropriate pixel\n");
    }
    return min_i;
}

/**
 * Create the Decision tree. In each recursive call, we consider the subset of the
 * dataset that correspond to the new node. To represent the subset, we pass 
 * an array of indices of these images in the subset of the dataset, along with 
 * its length M. Be careful to allocate this indices array for any recursive 
 * calls made, and free it when you no longer need the array. In this function,
 * you need to:
 *
 *    - Compute ratio of most frequent image in indices, do not split if the
 *      ration is greater than THRESHOLD_RATIO
 *    - Find the best pixel to split on using `find_best_split`
 *    - Split the data based on whether pixel is less than 128, allocate 
 *      arrays of indices of training images and populate them with the 
 *      subset of indices from M that correspond to which side of the split
 *      they are on
 *    - Allocate a new node, set the correct values and return
 *       - If it is a leaf node set `classification`, and both children = NULL.
 *       - Otherwise, set `pixel` and `left`/`right` nodes 
 *         (using build_subtree recursively). 
 */
DTNode *build_subtree(Dataset *data, int M, int *indices) {
    // TODO: Construct and return the tree

    // Malloc current node
    DTNode *dtn = malloc(sizeof(DTNode));

    // Classification
    // Malloc label and freq
    int *label = malloc(sizeof(int));
    int *freq = malloc(sizeof(int));
    get_most_frequent(data, M, indices, label, freq);

    if ((double)*freq / M > THRESHOLD_RATIO) {
        dtn->classification = *label;
        dtn->pixel = -1;
        dtn->left = NULL;
        dtn->right = NULL;
        free(label);
        free(freq);
        return dtn;
    } else {
        free(label);
        free(freq);
    }

    // Find the pixel
    int pixel =  find_best_split(data, M, indices);

    // Get length
    int left_M = 0, right_M = 0;

    for (int i = 0; i < M; i++) {
        int img_idx = indices[i];
        if (data->images[img_idx].data[pixel] < 128) {
            left_M += 1;
        } else {
            right_M += 1;
        }
    }

    // Malloc sub-indices, free after calling build_subtree
    int *right_indices = malloc(sizeof(int) * right_M);
    int *left_indices = malloc(sizeof(int) * left_M);

    // Split into two sub indices
    left_M = 0, right_M = 0;
    for (int i = 0; i < M; i++) {
        int img_idx = indices[i];
        if (data->images[img_idx].data[pixel] < 128) {
            left_indices[left_M] = img_idx;
            left_M += 1;
        } else {
            right_indices[right_M] = img_idx;
            right_M += 1;
        }
    }

    // Assign value
    dtn->classification = -1;
    dtn->pixel = pixel;
    dtn->left = build_subtree(data, left_M, left_indices);
    dtn->right = build_subtree(data, right_M, right_indices);
    // Free after being used
    free(left_indices);
    free(right_indices);
    return dtn;
}

/**
 * This is the function exposed to the user. All you should do here is set
 * up the `indices` array correctly for the entire dataset and call 
 * `build_subtree()` with the correct parameters.
 */
DTNode *build_dec_tree(Dataset *data) {
    // TODO: Set up `indices` array, call `build_subtree` and return the tree.
    // HINT: Make sure you free any data that is not needed anymore

    // Malloc outest indices
    int *indices = malloc(sizeof(int) * data->num_items);

    // Init indices
    for (int i = 0; i < data->num_items; i++) {
        indices[i] = i;
    }

    // Build DTNode
    DTNode *dtn = build_subtree(data, data->num_items, indices);

    // Free outest indices
    free (indices);

    return dtn;
}

/**
 * Given a decision tree and an image to classify, return the predicted label.
 */
int dec_tree_classify(DTNode *root, Image *img) {
    // TODO: Return the correct label

    while (root->classification == -1) {
        if (img->data[root->pixel] < 128) {
            // Goto left subtree
            root = root->left;
        } else {
            // Goto right subtree
            root = root->right;
        }
    }

    // Reach leaf
    if (root->pixel != -1) {
        fprintf(stderr, "ERROR: leaf does not contain a correct classification\n");
        return -1;
    }
    return root->classification;
}

/**
 * This function frees the Decision tree.
 */
void free_dec_tree(DTNode *node) {
    // TODO: Free the decision tree

    if (node->classification == -1) {
        // Free sub node first
        free_dec_tree(node->left);
        free_dec_tree(node->right);
    }
    // Free self
    free(node);
}

/**
 * Free all the allocated memory for the dataset
 */
void free_dataset(Dataset *data) {
    // TODO: Free dataset (Same as A1)

    for(int i = 0; i < data->num_items; i ++) {
        // Free images data
        free(data->images[i].data);
    }

    // Free images and labels
    free(data->images);
    free(data->labels);

    // Free data itself
    free(data);
}