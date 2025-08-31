/*
 * ============================================================================
 *
 *       Filename:  datasets.c
 *
 *    Description:  Tools for working with cNeuro Datasets.
 *
 *        Created:  30/08/2025 17:31:52
 *       Compiler:  gcc
 *
 *         Author:  Joseph York
 *
 * ============================================================================
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "datasets.h"

void print_imageset_info(imageset_t imageset) {
    printf("==============================\n");
    printf("Image Dataset:\n");
    printf("  Dataset Length: %i\n", imageset.length);
    printf("  Image Size: %i by %i\n",
        imageset.image_height, imageset.image_width);
    printf("==============================\n");
}

void print_imageset_head_image(imageset_t imageset) {
    image_t head_image = imageset.images[0];
    print_image(head_image);
}

int* parse_ints_from_csvrow(char* csv_row_buffer, int img_size)
{
    int buff_itr = 0;
    int* parsed_array = (int *)malloc(img_size * sizeof(int));
    for (int i = 0; i < img_size; i++) {
        char temp_buff[10];
        int temp_itr = 0;

        memset(temp_buff, 0, sizeof(temp_buff));
        while (csv_row_buffer[buff_itr] != ',') {
            temp_buff[temp_itr++] = csv_row_buffer[buff_itr++];
        }

        parsed_array[i] = atoi(temp_buff);
        buff_itr++;
    }

    return parsed_array;
}

/* Take in a char buffer of arbitrary size and parse into an image type */
image_t parse_img_row(char *buffer, int img_height, int img_width) {
    image_t image;
    image.image_height = img_height;
    image.image_width = img_width;

    int img_size = (img_height * img_width) + 1;
    int* parsed_int_array = parse_ints_from_csvrow(buffer, img_size);
    image.image_data = (int *)malloc(img_height * img_width * sizeof(int));
    for (int i = 0; i < img_size; i++) {
        if (i == 0) {
            image.label = parsed_int_array[i];
        }
        else {
            image.image_data[i - 1] = parsed_int_array[i];
        }
    }

    return image;
}

imageset_t read_imageset_csv(char* file_path, int img_height, int img_width) {
    int row_idx = 0;
    int total_images = 0;  // Initialize to 0
    imageset_t output_imageset;

    FILE *file_pointer = fopen(file_path, "r");
    if (file_pointer == NULL) {
        perror("Error opening file");
        // Return an empty imageset on error
        output_imageset.length = 0;
        output_imageset.images = NULL;
        return output_imageset;
    }

    int buffer_size = (8192); // arbitrary size
    char *buffer = (char *)malloc(buffer_size * sizeof(char));

    // Init the head node of our linked list
    int is_initalized = 0;
    image_list_t *head_node = NULL;
    image_list_t *recent_node = NULL;

    output_imageset.image_height = img_height;
    output_imageset.image_width = img_width;

    while (fgets(buffer, buffer_size, file_pointer) != NULL) {
        if (row_idx == 0) { // We skip the first row of the CSV due to labels
            row_idx++;
            continue;
        }

        image_list_t *current_node = (image_list_t *)malloc(sizeof(image_list_t));
        if (current_node == NULL) {
            perror("Memory allocation failed");
            break;
        }

        current_node->image = parse_img_row(buffer, img_height, img_width);
        current_node->next = NULL;

        if (is_initalized == 0) {
            head_node = current_node;
            recent_node = head_node;
            is_initalized = 1;
        }
        else {
            recent_node->next = current_node;
            recent_node = current_node;
        }

        total_images++;
        row_idx++;
    }

    fclose(file_pointer);

    output_imageset.length = total_images;

    output_imageset.images = (image_t *)malloc(total_images * sizeof(image_t));
    if (output_imageset.images == NULL && total_images > 0) {
        perror("Memory allocation failed");
        image_list_t *current = head_node;
        while (current != NULL) {
            image_list_t *temp = current;
            current = current->next;
            free(temp);
        }
        free(buffer);
        output_imageset.length = 0;
        return output_imageset;
    }

    image_list_t *current_node = head_node;
    image_list_t *last_node = NULL;
    for (int i = 0; i < total_images; i++) {
        if (current_node == NULL) break;  // Safety check
        output_imageset.images[i] = current_node->image;
        last_node = current_node;
        current_node = current_node->next;
        free(last_node);
    }

    free(buffer);
    return output_imageset;
}

/* Take in an image and print a visualization to the console */
void print_image(image_t image) {
    printf("\nImage label = %i\n", image.label);
    printf("Image Contents:\n");
    for (int i = 0; i < image.image_height; i++) {
        for (int j = 0; j < image.image_width; j++) {
            int img_val = image.image_data[(i * image.image_height) + j];
            if (img_val < 64) {
                printf("\xE2\x96\x91\xE2\x96\x91"); // light square unicode
            }
            else if ((128 > img_val) && (img_val >= 64)) {
                printf("\xE2\x96\x92\xE2\x96\x92"); // medium square unicode
            }
            else if ((192 > img_val) && (img_val >= 128)) {
                printf("\xE2\x96\x93\xE2\x96\x93"); // dark square unicode
            }
            else {
                printf("\xE2\x96\x88\xE2\x96\x88"); // filled square unicode
            }
        }

        printf("\n");
    }
}

void print_image_properties(image_t image) {
    printf("==============================\n");
    printf("Image:\n");
    printf("  Image Size: %i by %i\n",
        image.image_height, image.image_width);
    printf("  Image Label: %i\n", image.label);
    print_image(image);
    printf("\n");
    printf("==============================\n");
}

int* get_image_1d_tensor(image_t image) {
    int* tensor = (int *)malloc(image.image_height * image.image_width * sizeof(int));
    for (int i = 0; i < image.image_height; i++) {
        for (int j = 0; j < image.image_width; j++) {
            tensor[(i * image.image_width) + j] = image.image_data[(i * image.image_width) + j];
        }
    }

    return tensor;
}