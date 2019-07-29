#include <stdio.h>
#include<math.h>
#include<sys/time.h>
#include<stdlib.h>
#include<omp.h>

#define SIZE 2048
#define BLOCK 4
#define PADDING 1

double a[SIZE+PADDING][SIZE+PADDING], b[SIZE+PADDING][SIZE+PADDING], c[SIZE+PADDING][SIZE+PADDING];

int get_time(){
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    return  start_time.tv_sec*1000*1000 + start_time.tv_usec;
}
int init_mat(){
    for(int i=0;i<SIZE;i++) for(int j=0;j<SIZE;j++){
        a[i][j] = (((double)rand() / ((double)RAND_MAX + 1)) * 100.0) - 50.0;
    }
    for(int i=0;i<SIZE;i++) for(int j=0;j<SIZE;j++){
        b[i][j] = (((double)rand() / ((double)RAND_MAX + 1)) * 100.0) - 50.0;
    }
    for(int i=0;i<SIZE;i++) for(int j=0;j<SIZE;j++){
        c[i][j] = 0.0;
    }
    return 0;
}

int verifier(){
    for(int itr=0; itr<5;itr++){
        int i = rand()%SIZE, j = rand() % SIZE;
        double tc = 0;
        for(int k=0;k<SIZE;k++){
            tc += a[i][k] * b[k][j];
        }
        if(fabs(c[i][j] - tc) > 1e-6){
            printf("NOT verified.\n");
            return 1;
        }
    }
    printf("Verified\n");
    return 0;
}
void swap(double *x, double *y){
    double tem = *x;
    *x = *y;
    *y = tem;
}

int transpose_b(){
    #pragma omp parallel for
    for(int i=0; i<SIZE; i++){
        for(int j=i+1;j<SIZE;j++){
            swap(&b[i][j], &b[j][i]);
        }
    }
    return 0;
}

int transpose_a(){
    #pragma omp parallel for
    for(int i=0; i<SIZE; i++){
        for(int j=i+1;j<SIZE;j++){
            swap(&a[i][j], &a[j][i]);
        }
    }
    return 0;
}

int main(){
    int start_time, end_time;
    double clps_time;
    init_mat();
    omp_set_num_threads(64);
    start_time = get_time(); // start_matmul section

    transpose_b();

    #pragma omp parallel
    {
        int iw = (omp_get_thread_num()/8) * 256;
        int jw = (omp_get_thread_num()%8) * 256;
        for(int i=0 + iw; i + iw < 256;i+=BLOCK){
            for(int j=0 + jw; j+jw<256;j+=BLOCK){
                for(int k=0;k<SIZE;k++){
                    for(int l=0; l < BLOCK*BLOCK; l++){
                        c[i + (l%BLOCK)][j + (l/BLOCK)] += a[i + (l%BLOCK)][k] * b[j + (l/BLOCK)][k];
                    }
                }
            }
        }
    }
    
    transpose_b();

    end_time = get_time();//end_matmul section
    clps_time = (end_time - start_time) / 1e6;
    printf("time: %.6lf\n", clps_time);
    verifier();
}