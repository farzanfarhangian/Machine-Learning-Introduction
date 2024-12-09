#include <stdio.h>
#include <stdlib.h>
#include "preprocess.h"

typedef struct 
{
    char **strings;
    float *values;
    int count;
} StringMap;

static int index_of(int *arr, int size, int value) 
{
    for (int i = 0; i < size; i++) 
    {
        if (arr[i] == value) 
        {
            return i;  // Return the index if found
        }
    }
    return -1;  // Return -1 if value is not found
}

static StringMap *_create_string_map() 
{
    StringMap *map = (StringMap *)malloc(sizeof(StringMap));
    map->strings = NULL;
    map->values = NULL;
    map->count = 0;

    return map;
}

// Free the memory for a StringMap
static void _free_string_map(StringMap *map) 
{
    for (int i = 0; i < map->count; i++) 
    {
        free(map->strings[i]);
    }
    free(map->strings);
    free(map->values);
    free(map);
}

static float _add_to_string_map(StringMap *map, const char *str) 
{
    for (int i = 0; i < map->count; i++) 
    {
        if (strcmp(map->strings[i], str) == 0) 
        {
            return map->values[i]; // String already exists
        }
    }

    // Add a new string
    map->count++;
    map->strings = (char **)realloc(map->strings, map->count * sizeof(char *));
    map->values = (float *)realloc(map->values, map->count * sizeof(float));

    map->strings[map->count - 1] = strdup(str);
    map->values[map->count - 1] = (float)(map->count - 1);

    return map->values[map->count - 1];
}

static int _dataFrame_get_dataset_size(int fd, int * row_size, int * col_size)
{
    char buffer[1024];
    int bytesRead;
    int col_cnt = 0;
    int row_cnt = 0;
    lseek(fd, 0, SEEK_SET);

    while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        for(int i =0; i < bytesRead; i++)
        {
            if(buffer[i] == ',' && !row_cnt)
            {
                col_cnt++;
            }
            if(buffer[i] == '\n')
            {
                if(!row_cnt)
                {
                    col_cnt++;
                } 
                row_cnt++;
            }
        }
    }

    *row_size = row_cnt;
    *col_size = col_cnt;

    return 0;
}

static int * _dataFrame_get_dataset_string_columns(int fd, int * count)
{
    char buffer[1024];
    char lineBuffer[1024];
    int bytesRead;
    int startIndex = 0;
    int count_lines = 0;
    int num_str_col = 0;
    lseek(fd, 0, SEEK_SET);

    while(count_lines<2)
    {
        bytesRead = read(fd, buffer, sizeof(buffer));
        if(bytesRead<=0)
        {
            return 0;
        }

        for(int i =0; i < bytesRead; i++)
        {
            if(buffer[i] == '\n')
            {
                count_lines++;
                if(count_lines == 2)
                {
                    memcpy(lineBuffer, &buffer[startIndex], i - startIndex);
                    lineBuffer[i - startIndex] = '\0';
                }
                else
                {
                    startIndex = i+1;
                }
            }
        }
    }
    char copy_lineBuffer[1024];
    memcpy(copy_lineBuffer, lineBuffer, sizeof(lineBuffer));

    char *token = strtok(lineBuffer, ",");
    while (token) 
    {
        if (strspn(token, "0123456789.") != strlen(token))
        {
            num_str_col++;
        }        
        token = strtok(NULL, ",");
    }

    int *indexes =  (int *)malloc(num_str_col * sizeof(int));

    token = strtok(copy_lineBuffer, ",");
    int col = 0;
    int d = 0; 
    while (token) 
    {
        if (strspn(token, "0123456789.") != strlen(token))
        {
            indexes[d] = col;
            d++;
        }        
        token = strtok(NULL, ",");
        col++;
    }

    *count = num_str_col;
    return indexes;
}

void dataFrame_show_dataset(struct dataFrame * dataf)
{
    for (int i=0; i<dataf->row_size; i++)
    {
        for(int j=0; j<dataf->col_size; j++)
        {
            printf("%lf", dataf->data[i][j]);
            if(j != dataf->col_size - 1)
            {
                printf(", ");
            }
        }
        printf("\n");
    }
}

static float * dataFrame_get_single_column(struct dataFrame * dataf, uint8_t columnIndex)
{
    if(columnIndex > dataf->col_size-1)
    {
        return NULL;
    }

    float * array = (float *)malloc( dataf->row_size * sizeof(float));
    for (int i = 0; i < dataf->row_size; i++)
    {
        array[i] = dataf->data[i][columnIndex];
    }

    return array;
}

void dataDrame_split_trainAndTest(struct dataFrame * dataf, struct dataFrame ** train_df, struct dataFrame ** test_df, float test_percentage)
{
    int row_test = (int)(dataf->row_size * test_percentage);

    *test_df = dataf->get_subDataFrame(dataf, 0, row_test-1, 0, dataf->col_size - 1);
    *train_df = dataf->get_subDataFrame(dataf, row_test, dataf->row_size-1, 0, dataf->col_size - 1);
}

static struct dataFrame* dataFrame_get_submatrix(struct dataFrame * dataf, uint8_t rowIndex_start, uint8_t rowIndex_end, uint8_t columnIndex_start, uint8_t columnIndex_end)
{
    if(columnIndex_start > dataf->col_size-1 || columnIndex_end > dataf->col_size-1 ||
        columnIndex_start >= columnIndex_end)
    {
        return NULL;
    }
    if(rowIndex_start > dataf->row_size-1 || rowIndex_end > dataf->row_size-1 ||
        rowIndex_start >= rowIndex_end)
    {
       return NULL;
    }

    struct dataFrame *new_df = (struct dataFrame *)malloc(sizeof(struct dataFrame));
    new_df->row_size = rowIndex_end - rowIndex_start + 1;
    new_df->col_size = columnIndex_end - columnIndex_start + 1;
    
    new_df->data = (float **)malloc(new_df->row_size * sizeof(float *));
    for (int i = 0; i < new_df->row_size; i++) 
    {
        new_df->data[i] = (float *)malloc((new_df->col_size) * sizeof(float));
    }

    int m = 0;
    for (int i = rowIndex_start; i <= rowIndex_end; i++) 
    {
        int n = 0;
        for (int j = columnIndex_start; j <= columnIndex_end; j++) 
        {
            new_df->data[m][n] = dataf->data[i][j];
            n++;
        }
        m++;
    }

    new_df->show = dataFrame_show_dataset;
    new_df->get_column = dataFrame_get_single_column;
    new_df->get_subDataFrame = dataFrame_get_submatrix;
    new_df->split_trainAndTest = dataDrame_split_trainAndTest;

    return new_df;
}

struct dataFrame* dataFrame_create_dataset(const char * fileAddress)
{
    int fd = open(fileAddress, O_RDONLY);
    if (fd == -1)
    {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    // allocate memory for the struct dataframe
    struct dataFrame *df = (struct dataFrame *)malloc(sizeof(struct dataFrame));
    _dataFrame_get_dataset_size(fd, &df->row_size, &df->col_size);
    int num_str_col = 0;
    int * index_string_cols = _dataFrame_get_dataset_string_columns(fd, &num_str_col);

    // allocate memory for the 2D matrix
    df->data = (float **)malloc(df->row_size * sizeof(float *));
    for(int i=0; i< df->row_size; i++)
    {
        df->data[i] = (float *)malloc(df->col_size * sizeof(float));
    }

    StringMap **maps = (StringMap **)malloc(num_str_col * sizeof(StringMap *));
    for(int i=0; i<num_str_col; i++)
    {
        maps[i] = _create_string_map();
    }

    // read and save the data from CSV file to the data frame structure
    char buffer[1024];
    int bytesRead;
    char lineBuffer[1024];
    int row = 0;
    int startIndex = 0;
    int i = 0;
    lseek(fd, 0, SEEK_SET);
    while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        for(i = 0; i < bytesRead; i++)
        {
            if (buffer[i] == '\n')
            {
                memcpy(lineBuffer, &buffer[startIndex], i - startIndex);
                lineBuffer[i - startIndex -1] = '\0';

                if(!row)
                {
                    row++;
                    startIndex = i+1;
                    continue;
                }
                
                // separete the received line
                int col = 0;
                char *token = strtok(lineBuffer, ",");
                while (token != NULL) 
                {
                    if (strspn(token, "0123456789.") == strlen(token))
                    {
                        df->data[row-1][col] = strtof(token, NULL);
                    }
                    else
                    {
                        df->data[row-1][col] = _add_to_string_map(maps[index_of(index_string_cols, num_str_col, col)], token);
                    }
                    
                    col++;
                    token = strtok(NULL, ",");
                }
                row++;
                startIndex = i+1;
            }
        }
        
        // also read last line
        memcpy(lineBuffer, &buffer[startIndex], i - startIndex);     
        lineBuffer[i - startIndex] = '\0';
        
        // separete the received line
        int col = 0;
        char *token = strtok(lineBuffer, ",");
        while (token) 
        {
            if (strspn(token, "0123456789.") == strlen(token))
            {
                df->data[row-1][col] = strtof(token, NULL);
            }
            else
            {
                df->data[row-1][col] = _add_to_string_map(maps[index_of(index_string_cols, num_str_col, col)], token);
            }
            col++;
            token = strtok(NULL, ",");
        }
    }

    close(fd);

    for(int i=0; i<num_str_col; i++)
    {
        _free_string_map(maps[i]);
    }

    df->show = dataFrame_show_dataset;
    df->get_column = dataFrame_get_single_column;
    df->get_subDataFrame = dataFrame_get_submatrix;
    df->split_trainAndTest = dataDrame_split_trainAndTest;

    return df;
}

void dataFrame_destroy_dataset(struct dataFrame * dataf)
{
    if (dataf) 
    {
        if (dataf->data) 
        {
            for (int i = 0; i < dataf->row_size; i++) 
            {
                free(dataf->data[i]);
            }
            free(dataf->data);
        }
        free(dataf);
    }
}

void dataFrame_show_array(float * array, uint8_t size)
{
    for (int i=0; i<size; i++)
    {
        printf("%lf", array[i]);
        if(i != size - 1)
        {
            printf(", ");
        }
    }
    printf("\n");
}