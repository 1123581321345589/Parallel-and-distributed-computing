#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

    int rank, numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    double startfull=MPI_Wtime();

    int N;
    if(rank==0){
        N=500000000;
    }


    MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);

    if(N%numranks!=0){
        if(rank==0){
            printf("Size not divisible by ranks: %d %d\n",N,numranks);
        }
        MPI_Finalize();
        return 0;
    }

    int *a;
    int *b;
    double startalloc=MPI_Wtime();
    if(rank==0){
        a=(int*)malloc(N*sizeof(int));
        b=(int*)malloc(N*sizeof(int));
        for(int i=0;i<N;i++){
            a[i]=b[i]=1;
        }
    }
    double endalloc=MPI_Wtime();


    int myN=N/numranks;

    int sendcounts=myN; 


    int *mya=(int*)malloc(sendcounts*sizeof(int));
    int *myb=(int*)malloc(sendcounts*sizeof(int));

    double startscatter=MPI_Wtime();
    MPI_Scatter(a,sendcounts,MPI_INT,mya,sendcounts,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(b,sendcounts,MPI_INT,myb,sendcounts,MPI_INT,0,MPI_COMM_WORLD);
    double endscatter=MPI_Wtime();

    double startcomp=MPI_Wtime();
    int result=0;
    for(int i=0;i<sendcounts;i++){
        result+=mya[i]*myb[i];
    }
    double endcomp=MPI_Wtime();

    double startreduce=MPI_Wtime();
    MPI_Allreduce(MPI_IN_PLACE,&result,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
    double endreduce=MPI_Wtime();
    double endfull=MPI_Wtime();

    if(rank==0){
        printf("Rank: %d, Result: %d\n",rank,result);
        printf("Full time: %f\n",endfull-startfull);
        printf("Calc time: %f\n",endcomp-startcomp);
        printf("Alloc+Initalize time: %f\n",endalloc-startalloc);
        printf("Scatter time: %f\n",endscatter-startscatter);
        printf("Reduce time: %f\n",endreduce-startreduce);
    }

    MPI_Finalize();
}
