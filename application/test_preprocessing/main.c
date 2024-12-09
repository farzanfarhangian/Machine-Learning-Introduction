#include "Data_Preprocessing/preprocess.h"

int main()
{
    printf("Creating the dataset from csv file:\n");
    struct dataFrame * df = dataFrame_create_dataset("../../datasets/Data_preprocessing.csv");
    df->show(df);
    
    printf("\nGetting column index 0 from the dataset:\n");
    float * array = df->get_column(df, 0);
    dataFrame_show_array(array, df->row_size);

    struct dataFrame *df_train = NULL;
    struct dataFrame *df_test = NULL;
    printf("\nSplittng the dataset into train and test datasets:\n");
    df->split_trainAndTest(df, &df_train, &df_test, 0.5f);
    
    printf("\nTest dataset:\n");
    df_test->show(df_test);

    printf("\nTrain dataset:\n");
    df_train->show(df_train);

    
    dataFrame_destroy_dataset(df);
    dataFrame_destroy_dataset(df_test);
    dataFrame_destroy_dataset(df_train);
    printf("\nAll datasets are destroyed.\n");
    
    return 0;
}