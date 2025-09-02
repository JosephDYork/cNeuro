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
#include "datasets.h"

int main(int argc, char *argv[])
{
    options_t options;
    options_parser(argc, argv, &options);

    imageset_t train_set = read_imageset_csv("data/mnist_train.csv", 28, 28);
    print_imageset_info(train_set);
    print_imageset_head_image(train_set);
}