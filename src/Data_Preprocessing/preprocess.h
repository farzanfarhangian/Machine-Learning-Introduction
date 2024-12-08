#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

struct dataFrame
{
    float ** data;
    int row_size;
    int col_size;
};

void preprocessing_test(void);
struct dataFrame* preprocessing_create_dataset(const char * fileAddress);

void preprocessing_show_dataset(struct dataFrame * dataf);
void preprocessing_show_array(float * array, uint8_t size);

float * preprocessing_get_single_column(struct dataFrame * dataf, uint8_t columnIndex);
struct dataFrame* preprocessing_get_submatrix(struct dataFrame * dataf, uint8_t columnIndex_start, uint8_t columnIndex_end);