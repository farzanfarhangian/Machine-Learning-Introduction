#include "Data_Preprocessing/preprocess.h"

int main()
{
    struct dataFrame * df = preprocessing_create_dataset("../../datasets/Data_preprocessing.csv");
    
    printf("size: %d, %d\n", df->row_size, df->col_size);

    preprocessing_show_dataset(df);
    
    return 0;
}