#include <stdio.h>
#include <stdlib.h>
#include "preprocess.h"

static int _preprocessing_get_dataset_size(int fd, int * row_size, int * col_size)
{
    char buffer[1024];
    int bytesRead;
    int col_cnt = 0;
    int row_cnt = 0;

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

struct dataFrame* preprocessing_get_dataset(const char * fileAddress)
{
    int fd = open(fileAddress, O_RDONLY);
    if (fd == -1)
    {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    // allocate memory for the struct dataframe
    struct dataFrame *df = (struct dataFrame *)malloc(sizeof(struct dataFrame));
    _preprocessing_get_dataset_size(fd, &df->row_size, &df->col_size);

    // allocate memory for the 2D matrix 
    df->data = (float **)malloc(df->row_size * sizeof(float *));
    for(int i=0; i< df->row_size; i++)
    {
        df->data[i] = (float *)malloc(df->col_size * sizeof(float));
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
        printf("%s\n", buffer);
        for(i = 0; i < bytesRead; i++)
        {
            if (buffer[i] == '\n')
            {
                memcpy(lineBuffer, &buffer[startIndex], i - startIndex);
                lineBuffer[i - startIndex] = '\0';

                if(!row)
                {
                    row++;
                    startIndex = i+1;
                    continue;
                }
                
                // separete the received line
                int col = 0;
                char *token = strtok(lineBuffer, ",");
                while (token) 
                {
                    df->data[row-1][col] = strtof(token, NULL);
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
        printf("line %d: %s\n", row, lineBuffer);
        
        // separete the received line
        int col = 0;
        char *token = strtok(lineBuffer, ",");
        while (token) 
        {
            df->data[row-1][col] = strtof(token, NULL);
            col++;
            token = strtok(NULL, ",");
        }
    }

    close(fd);

    return df;
}

void preprocessing_show_dataset(struct dataFrame * dataf)
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

/*preprocessing_dataset_info()
{
    
}*/

void preprocessing_test(void)
{
    printf("hello from preprocessing\n");
}
