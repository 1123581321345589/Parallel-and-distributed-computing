#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void assignMatrix(double* mat, double* vec, int n);
void printMatVec(double* mat, double* vec, double* res, int n);
double* mvp(double* mat, double* vec, int n, int m);

int main(int argc, char *argv[]){

    MPI_Init(&argc, &argv);
    int rank,numranks;
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    double *vec;    // The Vector
    double *mat;    // The Matrix
    double *res;    // The Result
    int N=500;    // Dimension 

    double FLOP = 2*N*N-N;


    /*
     *  ALLOCATE SPACE
     */
    if(rank==0){
        mat = malloc(N * N * sizeof(double)); // allocate N*N doubles
        res = malloc(N * sizeof(double));   // allocate space for the result
    }
    vec = malloc(N * sizeof(double));   //TODO3


    /*
     * COMPUTE WHAT EACH RANK SHOULD DO
     */
    int myrows=0;       

    if( N % numranks!=0 ){
        if(rank==0){
            printf("Matrix size is not divisible by ranks: %d %d\n", N , numranks);
        }
        MPI_Finalize();
        return 0;
    }

    myrows = N / numranks;          // if N is 30,000 then myrows is 30000/10 = 3000 (for 10 rank configuration)
    printf("Rank %d, myrows=%d\n", rank, myrows);

    
    double startFull=MPI_Wtime();

    /*
     * ALL NODES ALLOCATE SPACE FOR PARTIAL RESULTS
     */
    double *mymat = (double*) malloc(myrows * N * sizeof(double));  // Height is 3,000  and  Width is 30,000


    /*
     * ONLY ROOT ASSIGNS MATRIX (why?)
     */
    if( rank==0 ) {
        assignMatrix(mat,vec,N);
    }
    double startcomm=MPI_Wtime();

    /* 
     * SCATTER MATRIX
     * -----------------------------------------------------------------
     * 1st: the entire matrix
     * 2nd: scatter chunk, how much each node will get
     * 3rd: the datatype (what datatypes we are sending)
     * 4th: receiving buffer. Someone scutters the data, others receive the scattered data *into this buffer*
     * 5th: how much are we going to receive? (must be equal or greater than the size of mymat)
     * 6th: What things are we receiving? Doubles.
     * 7th: WHO SCATERS THE DATA? This Rank!  Everyone else is a receiver (including ourselves).
     * 8th: Our Communicator.
     */
    MPI_Scatter(mat, myrows*N, MPI_DOUBLE,mymat, myrows*N, MPI_DOUBLE, 0, MPI_COMM_WORLD ); // 4

    /*
     * EVERYONE NEEDS THE VECTOR (How else are you going to perform the multiplication?)
     */
    MPI_Bcast(vec,N,MPI_DOUBLE, 0,MPI_COMM_WORLD);   ///   Broadcast the vec to everyone
    double endcomm=MPI_Wtime();


    /*
     * EVERYONE PERFORMS THE MULTIPLICATION 
     * The multiplication is the same **BUT** on a different subset of the matrix.
     */
    double start=MPI_Wtime();
    double* myres = mvp(mymat,vec,N,myrows);  // The matrix looks like  3000 x 30000 instead of 30000 x 30000
    double end=MPI_Wtime();

    
    /*
     * GATHER THE PARTIAL RESULTS
     * -----------------------------------------------------------------
     * Now each node computed partial products.
     * Each node needs to send the results: (partial matrix * vector products to Rank 0)
     * -----------------------------------------------------------------
     * 1st: my portion of the computation.
     * 2nd: How large is the buffer holding my computation  matrixOf[3000 x 30000] * vectorOf[30000] = resultOf[3000]
     * 3rd: What datatypes we are sending? (doubles)
     * 4th: The Rank the performs the "Gathering", Gathers the sub-results into this array
     * 5th: Total number of items we are going to gather from each single node
     * 6th: Datatype of the items we are receiving.
     * 7th: WHO PERFORMS THE Gathering? Who is the receiver? This Rank, Everyone else is a sender.
     * 8th: Our Communicator
     */
    
    double startgather=MPI_Wtime();
    MPI_Gather(myres, myrows,MPI_DOUBLE, res, myrows,MPI_DOUBLE,0,MPI_COMM_WORLD );
    double endgather=MPI_Wtime();


    double endFull=MPI_Wtime();
    if(rank==0){
        printf("Num Ranks=%d\n",numranks);
        printf("MVP Runtime: %f\n",end-start);
        printf("Initial Communication: %f\n",endcomm-startcomm);
        printf("Final Gather: %f\n",endgather-startgather);
        printf("Full time: %f\n",endFull-startFull);

        printf("FLOPS: %f\n", FLOP/(end-start));

        free(mat);
        free(res);
    }

    MPI_Finalize();


    free(mymat);
    free(vec);
    free(myres);

    return 0;
}


double* mvp(double* mat, double* vec, int n, int m){
    double *resms = (double*) malloc(m*sizeof(double));
    double sum=0;

    for(int i=0;i<m;i++){
       sum=0;
        for(int j=0;j<n;j++){
            sum+=mat[i*n+j]*vec[j];
        } 
       resms[i]=sum;
    }
    

    return resms;
}

void assignMatrix(double* mat, double* vec, int nms){

    for(int i=0;i<nms;i++){
        vec[i]=1;
        for(int j=0;j<nms;j++){
            if(i==j) mat[i*nms+j]=2;
            else if(j==i+1) mat[i*nms+j]=1;
            else if(j==i-1) mat[i*nms+j]=1;
            else mat[i*nms+j]=0;
        }
    }
    
}

void printMatVec(double* mat, double* vec, double* res, int n){
    for (int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%5.2f ",mat[i*n+j]);
        }
        if(i==n/2) printf("  %5.2f  =  %5.2f \n",vec[i],res[i]);
        else printf("  %5.2f     %5.2f\n",vec[i],res[i]);
    }
}

