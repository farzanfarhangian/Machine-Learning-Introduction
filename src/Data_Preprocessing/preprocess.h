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
struct dataFrame* preprocessing_get_dataset(const char * fileAddress);
void preprocessing_show_dataset(struct dataFrame * dataf);