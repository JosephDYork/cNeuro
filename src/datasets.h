/*
 * ============================================================================
 *
 *       Filename:  datasets.h
 *
 *    Description:  Header file for cNeuro Dataset utilities.
 *
 *        Created:  30/08/2025 17:31:52
 *       Compiler:  gcc
 *
 *         Author:  Joseph York
 *
 * ============================================================================
 */


#ifndef DATASETS_H
#define DATASETS_H

#include <stdio.h>

/* Datarow Struct */
typedef struct image {
    int label;
    int image_height;
    int image_width;
    int *image_data;
} image_t;

/* Dataset Struct */
typedef struct imageset {
    int length;
    int image_height;
    int image_width;
    image_t *images;
} imageset_t;

typedef struct image_list image_list_t;

struct image_list {
    image_t image;
    image_list_t *next;
};

/* Dataset Functions */
void print_imageset_info(imageset_t imageset);
void print_imageset_head_image(imageset_t imageset);
imageset_t read_imageset_csv(char* file_path, int img_height, int img_width);

/* Datarow Functions */
void print_image(image_t image);
void print_image_properties(image_t image);
int* get_image_1d_tensor(image_t image);

#endif //DATASETS_H