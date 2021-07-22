#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>


int	 main(int argc, char** argv){
    int rank, numranks;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    double startfull=MPI_Wtime();

    int N; // size of our vectors
    if(rank==0) {
        N=500000000;     // Only Rank 0 knows the size of the vectors
    }

    /*
     * Tell everyone how large the vectors are.
     * 1st. Since N is an int, we need to send  &N (this is our data)
     * 2nd. The data contains one single item (1)
     * 3rd. The single item is of type int (MPI_INT)
     * 4th. Rank 0 performs the Broadcasting, everyone else receives.
     *      No if statements! Everyone must call this Bcast.
     *      Rank 0 SENDs the data and everyone else RECEIVEs the data
     *      and places the data in the variable N.
     * 5th. Our communicator.
     */
     MPI_Bcast( &N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int *a;  // Our first Vector
    int *b;  // Our second Vector

    double startalloc=MPI_Wtime();
    /*
     * Everyone knows about the variables a and b (declared above)
     * but only Rank 0 allocates space for them, and then
     * initializes them in the "for" loop.
     */
    if( rank == 0 ){

        a = (int*)malloc( N*sizeof(int));

        b = (int*)malloc( N*sizeof(int));

        for(int i=0; i<N; i++){

            a[i] = b[i] = 2;

        }
    }

    double endalloc=MPI_Wtime();


    //N for each rank
    /*
     * How large each chunk will be?
     */
    int myN= N/numranks;
    /*
     * Allocates counts and displacements for all ranks
     */
    int *sendcounts=(int*)malloc(numranks*sizeof(int));
    int *disp=(int*)malloc(numranks*sizeof(int));

    /*
     * Compute Counts
     */
    for(int i=0; i<numranks; i++){

        sendcounts[i]=myN;

    }
    //last rank gets all the leftover
    sendcounts[numranks-1]+=N-myN*numranks;
    /*
     * Every Rank will run this.
     * Print number of elements that each rank gets
     */
    printf("Rank %d: Number of Elements: %d\n",rank,sendcounts[rank]);
    /*
     * Compute displacements
     */
    disp[0]= 0;
    for(int i=1;i<numranks;i++){

        disp[i]=disp[i-1]+sendcounts[i-1];

    }


    /*
     * All nodes will receive a partial vector data to perform
     * the dot product.  We need to allocate this space for every node/Rank
     * We need to do this for vector 'a' and vector 'b'.
     */
    int *mya = (int*) malloc(sendcounts[rank]*sizeof(int));
    int *myb = (int*) malloc(sendcounts[rank]*sizeof(int));
    /*
     * Scatterv both vectors
     * Everyone runs these two Scatterv()
     * Rank 0 performs the Scattering of the data (gets vectors 'a' and 'b'
     *        splits them up based on the sendcounts and send the chunks to
     *        everyother rank.  Everyother rank will receive the scattered data
     *        into the mya and myb buffers.
     *
     */

    double startscatter=MPI_Wtime();
    MPI_Scatterv(a,sendcounts,disp,MPI_INT,mya,sendcounts[rank],MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatterv(b,sendcounts,disp,MPI_INT,myb,sendcounts[rank],MPI_INT,0,MPI_COMM_WORLD);
    double endscatter=MPI_Wtime();

    /*
     * Do dot product
     * PLEASE NOTE HERE.  The computation of this partial dot product is the same.
     * However, each rank is now working on smaller vectors,
     * not (a dot b)  but rather (mya dot myb)
     * THIS IS WHERE PARALLELISM IMPROVES PERFORMANCE (same problem but smaller dataset)
     */
    double startcomp=MPI_Wtime();

    int result=0;
    for(int i=0;i<sendcounts[rank];i++){
        result+=mya[i]*myb[i];
    }

    double endcomp=MPI_Wtime();

    /*
     * Now that every node computed a partial dot product, and this partial dot product
     * is stored in the variable 'result' we need to send all these partial results
     * to rank 0 who can collect them all and compute the final result by summ them up.
     * We can use MPI_Reduce or MPI_Allreduce to send the final result to everyone.
     */
    double startreduce=MPI_Wtime();
    MPI_Allreduce(MPI_IN_PLACE,&result,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
    double endreduce=MPI_Wtime();
    double endfull=MPI_Wtime();

    printf("Rank: %d, Result: %d\n",rank,result);


    if(rank==0){
        printf("Full time: %f\n",endfull-startfull);
        printf("Calc time: %f\n",endcomp-startcomp);
        printf("Alloc+Initalize time: %f\n",endalloc-startalloc);
        printf("Scatter time: %f\n",endscatter-startscatter);
        printf("Reduce time: %f\n",endreduce-startreduce);
    }


    MPI_Finalize();
}

