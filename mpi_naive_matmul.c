#include <stdio.h>
#include<math.h>
#include<sys/time.h>
#include<stdlib.h>
#include<mpi.h>

#define SIZE 512

double a[SIZE][SIZE], b[SIZE][SIZE], c[SIZE][SIZE];

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

int main(int argc, char **argv){
    int start_time, end_time;
    double clps_time;
    int myid, numprocs;
    if(myid==1) init_mat();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    if(myid==0) start_time = get_time(); // start_matmul section
    int rows; rows = SIZE / numprocs;
    double al[rows][SIZE];
    double cl[rows][SIZE];

    MPI_Scatter(a, rows*SIZE, MPI_DOUBLE, al,rows*SIZE,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(& b[0][0], SIZE*SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for(int i=0;i<rows;i++){
        for(int j=0;j<SIZE;j++){
            cl[i][j] = 0.0;
        }
    }
    for(int i=0;i<rows;i++){
        for(int j=0;j<SIZE;j++){
            for(int k=0;k<SIZE;k++){
                cl[i][j] += al[i][k] * b[k][j];
            }
        }
    }
    MPI_Gather(cl,rows*SIZE,MPI_DOUBLE,c,rows*SIZE,MPI_DOUBLE,0,MPI_COMM_WORLD);
    if(myid==0){
        end_time = get_time();//end_matmul section
        clps_time = (end_time - start_time) / 1e6;
        printf("time: %.6lf\n", clps_time);
        verifier();
    }
    MPI_Finalize();
    return 0;

}