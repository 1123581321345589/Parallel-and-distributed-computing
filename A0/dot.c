#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

double parallelDot(double* localx,double* localy,int n);
double serialDot(double* x, double* y,int n);

int main(int argc, char *argv[])  {
    int numtasks, rank, dest, source, rc, count, tag=1;  
    char inmsg, outmsg='x';
    MPI_Status Stat;   // required variable for receive routines

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank==0){
        printf("Testing Parallel Dot Product...\n");
    }
    //root creates arrays and then scatters to everyone
    int n=1000;
    int i;
    double* x;
    double* y;
    //have main allocate and fill the matrices
    if (rank == 0) {
        x=(double*) malloc(n*sizeof(double));
        y=(double*) malloc(n*sizeof(double));
        for(i=0;i<n;i++){
            x[i]=1.0;
            y[i]=1.0;
        }
    }
    //create the local matrices for the chuncks
    int localn=n/numtasks;
    double* localx = (double*) malloc(localn*sizeof(double));
    double* localy = (double*) malloc(localn*sizeof(double));

    //scatter to all
    MPI_Scatter(x,localn,MPI_DOUBLE,localx,localn,MPI_DOUBLE,0,MPI_COMM_WORLD);                  
    MPI_Scatter(y,localn,MPI_DOUBLE,localy,localn,MPI_DOUBLE,0,MPI_COMM_WORLD);

    //run the parallel dot product
    //all ranks must call this function, but only root will
    //have the correct final result
    double res = parallelDot(localx,localy,localn);
    
    //print result
    if(rank==0){
        if(res==(double)n){
            printf("Test Passed\n");
        }else{
            printf("Test Failed\n");
        }
    }
       
    //deallocate memory
    if(rank=0){
        free(x);
        free(y);
    }
    free(localx);
    free(localy);
        
    MPI_Finalize();
    
}

//serial version
double serialDot(double* x, double* y,int n){
    double sum=0.0;
    int i;
    for(i=0;i<n;i++){
        sum+=x[i]*y[i];
    }
    return sum;
}

//parallel version (uses serial version for local dot product)
double parallelDot(double* localx,double* localy,int n){
    double dot=0.0;
    double localDot=0.0;
    localDot=serialDot(localx,localy,n);
    MPI_Reduce(&localDot,&dot,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    return dot;
}
