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

/**
 * Parses an array of integers from a CSV row
 *
 * @param csv_row_buffer Pointer to the buffer containing CSV data
 * @param img_size Number of integers to parse from the CSV row
 * @return Pointer to an array of parsed integers (must be freed by caller)
 */
int *parse_ints_from_csvrow(char *csv_row_buffer, int img_size)
{
    int buff_itr = 0;
    int *parsed_array = (int *)malloc(img_size * sizeof(int));
    for (int i = 0; i < img_size; i++)
    {
        int temp_itr = 0;
        char temp_buff[32];
        memset(temp_buff, 0, sizeof(temp_buff));
        while (csv_row_buffer[buff_itr] != ',' &&
               csv_row_buffer[buff_itr] != '\0' &&
               csv_row_buffer[buff_itr] != '\n' &&
               csv_row_buffer[buff_itr] != '\r' &&
               temp_itr < (int)(sizeof(temp_buff) - 1))
        { // Leave space for null terminator
            temp_buff[temp_itr++] = csv_row_buffer[buff_itr++];
        }

        parsed_array[i] = atoi(temp_buff);
        if (csv_row_buffer[buff_itr] == '\0' || csv_row_buffer[buff_itr] == '\n' || csv_row_buffer[buff_itr] == '\r')
        {
            break;
        }

        buff_itr++;
    }

    return parsed_array;
}

/**
 * Frees all memory allocated for a linked list of images
 *
 * @param head Pointer to the head of the linked list
 * @return Nothing
 */
void free_image_linked_list(image_list_t *head)
{
    image_list_t *cur = head;
    while (cur)
    {
        image_list_t *next = cur->next;
        if (cur->image.image_data)
        {
            free(cur->image.image_data);
        }
        free(cur);
        cur = next;
    }
}

/**
 * Parses an image row from a CSV buffer
 *
 * @param buffer Pointer to the buffer containing CSV data
 * @param img_height Height of the image in pixels
 * @param img_width Width of the image in pixels
 * @return An image_t structure containing the parsed image data
 */
image_t parse_img_row(char *buffer, int img_height, int img_width)
{
    image_t image;
    image.image_height = img_height;
    image.image_width = img_width;
    int img_size = (img_height * img_width) + 1;
    int *parsed_int_array = parse_ints_from_csvrow(buffer, img_size);
    image.image_data = (int *)malloc(img_height * img_width * sizeof(int));

    for (int i = 0; i < img_size; i++)
    {
        if (i == 0)
        {
            image.label = parsed_int_array[i];
        }
        else
        {
            image.image_data[i - 1] = parsed_int_array[i];
        }
    }

    free(parsed_int_array);
    return image;
}

/**
 * Builds a linked list of images from a CSV file
 *
 * @param fp File pointer to an opened CSV file
 * @param img_height Height of each image in pixels
 * @param img_width Width of each image in pixels
 * @param out_count Pointer to store the number of images read
 * @return Pointer to the head of the created linked list
 */
image_list_t *build_image_dynamic_list(FILE *fp, int img_height, int img_width, int *out_count)
{
    image_list_t *head = NULL;
    image_list_t *recent = NULL;
    int row_idx = 0;
    int count = 0;

    const int buffer_size = 8192;
    char *buffer = (char *)malloc(buffer_size);
    while (fgets(buffer, buffer_size, fp) != NULL)
    {
        if (row_idx++ == 0)
            continue; // skip the csv header

        image_list_t *node = (image_list_t *)malloc(sizeof(image_list_t));
        node->image = parse_img_row(buffer, img_height, img_width);
        node->next = NULL;

        if (!head)
        {
            head = recent = node;
        }
        else
        {
            recent->next = node;
            recent = node;
        }

        count++;
    }

    *out_count = count;
    free(buffer);
    return head;
}

/**
 * Converts a linked list of images to an array in an imageset structure
 *
 * @param head Pointer to the head of the linked list
 * @param count Number of images in the linked list
 * @return A pointer to the array of parsed images
 */
image_t *list_to_imageset(image_list_t *head, int count)
{
    image_list_t *cur = head;
    image_t *image_array = (image_t *)malloc(count * sizeof(image_t));

    for (int i = 0; i < count && cur; i++)
    {
        image_array[i] = cur->image;
        image_list_t *prev = cur;
        cur = cur->next;
        free(prev);
    }

    return image_array;
}

/**
 * Reads an image dataset from a CSV file
 *
 * @param file_path Path to the CSV file containing image data
 * @param img_height Height of each image in pixels
 * @param img_width Width of each image in pixels
 * @return An imageset_t structure containing the parsed imageset
 */
imageset_t read_imageset_csv(char *file_path, int img_height, int img_width)
{
    int count = 0;
    imageset_t imageset;
    FILE *fp = fopen(file_path, "r");

    imageset.image_height = img_height;
    imageset.image_width = img_width;
    imageset.length = 0;
    imageset.images = NULL;

    if (fp == NULL)
    {
        return imageset;
    }

    image_list_t *head = build_image_dynamic_list(fp, img_height, img_width, &count);
    imageset.images = list_to_imageset(head, count);
    imageset.length = count;

    fclose(fp);
    return imageset;
}

/**
 * Converts an image to a 1Dtensor
 *
 * @param image The image to convert to a tensor
 * @return Pointer to a newly allocated 1D tensor array (freed by caller)
 */
int *get_image_1d_tensor(image_t image)
{
    if (image.image_data == NULL)
    {
        return NULL;
    }

    int img_size = image.image_height * image.image_width;
    int *tensor = (int *)malloc(img_size * sizeof(int));
    memcpy(tensor, image.image_data, img_size * sizeof(int));

    return tensor;
}

/**
 * Prints a visual representation of an image to the console
 *
 * @param image The image to be displayed
 */
void print_image(image_t image)
{
    printf("┌─────────────────────────────────┐\n");
    printf("│          IMAGE CONTENT          │\n");
    printf("├─────────────────────────────────┤\n");
    printf("│  Label: %-22d  │\n", image.label);
    printf("└─────────────────────────────────┘\n");

    if (image.image_data == NULL)
    {
        printf("Error: Image data is NULL\n");
        return;
    }

    for (int i = 0; i < image.image_height; i++)
    {
        for (int j = 0; j < image.image_width; j++)
        {
            int img_val = image.image_data[(i * image.image_width) + j];
            if (img_val < 64)
            {
                printf("\xE2\x96\x91\xE2\x96\x91"); // light-shade
            }
            else if (img_val < 128)
            {
                printf("\xE2\x96\x92\xE2\x96\x92"); // medium-shade
            }
            else if (img_val < 192)
            {
                printf("\xE2\x96\x93\xE2\x96\x93"); // dark-shade
            }
            else
            {
                printf("\xE2\x96\x88\xE2\x96\x88"); // solid-shade
            }
        }
        printf("\n");
    }
}

/**
 * Prints the properties and visual representation of an image
 *
 * @param image The image whose properties are to be displayed
 */
void print_image_properties(image_t image)
{
    printf("┌─────────────────────────────────┐\n");
    printf("│         IMAGE PROPERTIES        │\n");
    printf("├─────────────────────────────────┤\n");
    printf("│  Dimensions: %3d x %-10d  │\n",
           image.image_height, image.image_width);
    printf("│  Label:      %-15d  │\n", image.label);
    printf("└─────────────────────────────────┘\n");

    print_image(image);
}

/**
 * Prints information about an image dataset
 *
 * @param imageset The image dataset to display information about
 * @return Nothing
 */
void print_imageset_info(imageset_t imageset)
{
    printf("┌─────────────────────────────────┐\n");
    printf("│          IMAGE DATASET          │\n");
    printf("├─────────────────────────────────┤\n");
    printf("│  Total Images: %-15d  │\n", imageset.length);
    printf("│  Dimensions:  %3d x %-10d  │\n",
           imageset.image_height, imageset.image_width);
    printf("└─────────────────────────────────┘\n");
}

/**
 * Prints the first image in the dataset
 *
 * @param imageset The image dataset containing the image to display
 * @return Nothing
 */
void print_imageset_head_image(imageset_t imageset)
{
    if (imageset.length == 0 || imageset.images == NULL)
    {
        printf("Error: Empty image dataset\n");
        return;
    }
    image_t head_image = imageset.images[0];
    print_image(head_image);
}