/*
 * ============================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main file of the project
 *
 *        Created:  30/08/2025 13:36:01
 *       Compiler:  gcc
 *
 *         Author:  Joseph York
 *
 * ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "colors.h"

struct mnist_image {
    int label;
    int data[784];
};

typedef struct mnist_image mnist_image_t;

mnist_image_t parse_img_data(char buffer[]) {
    int buff_itr = 0;
    int parsed_int_array[785];

    for (int i = 0; i < 785; i++) {
        char sub_buff[10];
        int sub_buff_itr = 0;

        memset(sub_buff, 0, sizeof(sub_buff));
        while (buffer[buff_itr] != ',') {
            sub_buff[sub_buff_itr] = buffer[buff_itr];
            sub_buff_itr++;
            buff_itr++;
        }

        parsed_int_array[i] = atoi(sub_buff);
        buff_itr++;
    }

    mnist_image_t image;
    image.label = buffer[0];
    for (int i = 0; i < 785; i++) {
        if (i == 0) {
            image.label = parsed_int_array[i];
        }

        image.data[i - 1] = parsed_int_array[i];
    }

    return image;
}

void print_mnist_image(mnist_image_t mnist_image) {
    printf("\nImage label = %i\n", mnist_image.label);
    printf("Image Contents:\n");
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            int img_val = mnist_image.data[i*28 + j];

            if (img_val < 64) {
                printf("\xE2\x96\x91"); // light square
                printf("\xE2\x96\x91");
            }
            else if ((128 > img_val) && (img_val >= 64)) {
                printf("\xE2\x96\x92"); // medium square
                printf("\xE2\x96\x92");
            }
            else if ((192 > img_val) && (img_val >= 128)) {
                printf("\xE2\x96\x93"); // dark square
                printf("\xE2\x96\x93");
            }
            else {
                printf("\xE2\x96\x88"); // filled square
                printf("\xE2\x96\x88");
            }
        }
        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    FILE *fp;
    options_t options;
    mnist_image_t image;
    options_parser(argc, argv, &options);

    int row_count = 0;
    char buffer[8192];

    fp = fopen("data/mnist_train.csv", "r");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    printf("File has been properly opened, starting parse...\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if(row_count == 0){
            printf("Skipping row one...\n");
        }
        else if (row_count == 1 || row_count == 2 || row_count == 3) {
            image = parse_img_data(buffer);
            print_mnist_image(image);
        }
        else {
            return EXIT_SUCCESS;
        }

        row_count++;
    }

    fclose(fp);
    return EXIT_FAILURE;
}