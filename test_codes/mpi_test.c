#include <stdio.h>
#include<math.h>
#include<sys/time.h>
#include<stdlib.h>
#include<mpi.h>
double a[360];
int main(int argc, char **argv){
    int myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    int row = 360 / numprocs;
    double al[row];
    MPI_Scatter(a, row, MPI_DOUBLE, al,row,MPI_DOUBLE,0,MPI_COMM_WORLD);
    for(int i=0;i<row&&i<360; i++) al[i] = myid;
    MPI_Gather(al,row,MPI_DOUBLE,a,row,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Finalize();  
    for(int i=0; i<360; i++) printf("%lf, ", a[i]);
    printf("¥n %d ¥n", numprocs);
    return 0;
}
