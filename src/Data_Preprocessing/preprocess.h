#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/*
 * @brief dataFrame dataset that contains most of functions to
 * work with the large datasets.
*/
struct dataFrame
{
    float ** data;
    int row_size;
    int col_size;

    // show the dataset elements
    void (*show)(struct dataFrame * data);
    
    // get a column from the dataset
    float* (*get_column)(struct dataFrame * dataf, uint8_t columnIndex);
    
    // get any-dimension submatrix from the dataset and allocate meory for that (returns another dataset)
    struct dataFrame* (*get_subDataFrame)(struct dataFrame * dataf, uint8_t rowIndex_start, uint8_t rowIndex_end, uint8_t columnIndex_start, uint8_t columnIndex_end);
    
    // split the dataset to two datasets and allocate memory for both of them
    // Note: it does not have a randomizer and starts from the index 0 to get the test dataset
    void (*split_trainAndTest)(struct dataFrame * dataf, struct dataFrame ** train_df, struct dataFrame ** test_df, float test_percentage);
};

/*
 * @brief Create a dataset and allocate memory for it from a csv file
 * Note: the csv file can contain numerci and string values and this function
 * will encode the strings to an integer code and returns matrix of float variables.
 * 
 * PS: for now, we do not support empty elements in the csv file. 
*/
struct dataFrame* dataFrame_create_dataset(const char * fileAddress);

/*
 * @brief Destrying a dataset and free up the memory
*/
void dataFrame_destroy_dataset(struct dataFrame * dataf);

/*
 * @brief show a give array
*/
void dataFrame_show_array(float * array, uint8_t size);