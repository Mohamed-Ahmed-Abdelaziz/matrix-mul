#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

FILE* fp1;
FILE* fp2;
FILE* fp3;
FILE* fp4;
FILE* fp5;
char filename1[30] = "";
char filename2[30] = "";
char filename3[30] = "";
char filename4[30] = "";
char filename5[30] = "";
int a[20][20], b[20][20], c[20][20];
int a_row, a_col, b_row, b_col, c_row, c_col;
int tot = 0;
struct timeval stop, start;
pthread_t th[20];
pthread_t th2[20][20];
pthread_t th3;

void* method1(){
    for (int i = 0; i < a_row; i++) {
      for (int j = 0; j < b_col; j++) {
        for (int k = 0; k < a_col; k++) {
          tot = tot + a[i][k] * b[k][j];
        }
        c[i][j] = tot;
        tot = 0;
      }
    }
}
void* method2(void* row_number){
    int row_n = *(int*)row_number;
    int sum;
    for(int j = 0; j < b_col; ++j){
        sum = 0;
        for(int k = 0; k < b_row; ++k){
            sum = sum + a[row_n][k] * b[k][j];
        }
        c[row_n][j] = sum;
    }
    free(row_number);
}
void* method3(void* args){
    int row_n = *(int*)args;
    int col_n = *((int*)args + 1);
    int sum = 0;
    for(int k = 0; k < a_col; ++k){
        sum = sum + a[row_n][k] * b[k][col_n];
    }
    c[row_n][col_n] = sum;
    free(args);
}

int main(int argc, char* argv[])
{
    if(argc == 1){
        strcat(filename1, "a.txt");
        strcat(filename2, "b.txt");
        strcat(filename3, "c_per_matrix.txt");
        strcat(filename4, "c_per_row.txt");
        strcat(filename5, "c_per_element.txt");
    }else{
        strcat(filename1, argv[1]);
        strcat(filename1, ".txt");

        strcat(filename2, argv[2]);
        strcat(filename2, ".txt");

        strcat(filename3, argv[3]);
        strcat(filename3, "_per_matrix.txt");

        strcat(filename4, argv[3]);
        strcat(filename4, "_per_row.txt");

        strcat(filename5, argv[3]);
        strcat(filename5, "_per_element.txt");
    }


    fp1 = fopen(filename1, "r");
    if(fp1 == NULL){
        printf("there is a problema\n");
        return;
    }
    for(int i = 0; i < 4; ++i) getc(fp1);
    fscanf(fp1, "%d", &a_row);
    for(int i = 0; i < 5; ++i) getc(fp1);
    fscanf(fp1, "%d", &a_col);

    for(int i = 0; i < a_row; i++){
        for(int j = 0; j < a_col; j++){
            fscanf(fp1, "%d", &a[i][j]);
        }
    }
    fclose(fp1);

    fp2 = fopen(filename2, "r");
    if(fp2 == NULL){
        printf("there is a problem\n");
        return;
    }
    for(int i = 0; i < 4; ++i) getc(fp2);
    fscanf(fp2, "%d", &b_row);
    for(int i = 0; i < 5; ++i) getc(fp2);
    fscanf(fp2, "%d", &b_col);

    for(int i = 0; i < b_row; i++){
        for(int j = 0; j < b_col; j++){
            fscanf(fp2, "%d", &b[i][j]);
        }
    }
    fclose(fp2);

    //------------- method 1 ------------
    gettimeofday(&start, NULL);

    if(pthread_create(&th3, NULL, &method1, NULL)) return 1;
    if(pthread_join(th3, NULL)) return 1;

    gettimeofday(&stop, NULL);
    printf("Time taken by first method = %lu \n", stop.tv_usec - start.tv_usec);

    fp3 = fopen(filename3, "w");
    if(fp3 == NULL){
        printf("there is a problema\n");
        return;
    }
    fprintf(fp3, "Method: a thread per matrix\n");
    fprintf(fp3, "row=%d col=%d\n", a_row, b_col);
    for(int i = 0; i < a_row; i++){
        for(int j = 0; j < b_col; j++){
            fprintf(fp3, "%d ", c[i][j]);
        }
        fprintf(fp3, "\n");
    }
    fclose(fp3);

    //----------------- method 2------------------
    gettimeofday(&start, NULL);
    int i;
    for(i = 0; i < a_row; ++i){
        int* a = malloc(sizeof(int));
        *a = i;
        if(pthread_create(&th[i], NULL, &method2, a)) return 1;
    }
    for(i = 0; i < a_row; ++i){
        if(pthread_join(th[i], NULL)) return 1;
    }

    gettimeofday(&stop, NULL);
    printf("Time taken by second method = %lu \n", stop.tv_usec - start.tv_usec);

    fp4 = fopen(filename4, "w");
    if(fp4 == NULL){
        printf("there is a problema\n");
        return;
    }
    fprintf(fp4, "Method: a thread per row\n");
    fprintf(fp4, "row=%d col=%d\n", a_row, b_col);
    for(int i = 0; i < a_row; i++){
        for(int j = 0; j < b_col; j++){
            fprintf(fp4, "%d ", c[i][j]);
        }
        fprintf(fp4, "\n");
    }
    fclose(fp4);

    //------------------- method 3------------------------
    gettimeofday(&start, NULL);
    i = 0;
    int j = 0;
    for(i = 0; i < a_row; ++i){
        for(j = 0; j < a_col; ++j){
            int* a = (int*)malloc(2 * sizeof(int));
            a[0] = i;
            a[1] = j;
            if(pthread_create(&th2[i][j], NULL, &method3, a)) return 1;
        }
    }
    for(i = 0; i < a_row; ++i){
        for(j = 0; j < a_col; ++j){
            if(pthread_join(th2[i][j], NULL)) return 1;
        }
    }

    gettimeofday(&stop, NULL);
    printf("Time taken by third method = %lu \n", stop.tv_usec - start.tv_usec);

    fp5 = fopen(filename5, "w");
    if(fp5 == NULL){
        printf("there is a problema\n");
        return;
    }
    fprintf(fp5, "Method: a thread per element\n");
    fprintf(fp5, "row=%d col=%d\n", a_row, b_col);
    for(int i = 0; i < a_row; i++){
        for(int j = 0; j < b_col; j++){
            fprintf(fp5, "%d ", c[i][j]);
        }
        fprintf(fp5, "\n");
    }
    fclose(fp5);

    return 0;
}
