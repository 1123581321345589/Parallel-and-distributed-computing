#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

    MPI_Init(&argc, &argv);
    int rank,numranks;
    MPI_Comm_size(MPI_COMM_WORLD,&numranks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status stat;
    int tag=0;

    int giga = (1024 * 1024 * 1024);
    int *data = (int*) malloc(giga * sizeof(int));
    double startTime, endTime;
    double dTime;
    int prev, next;

    if(rank == 0){
  prev = numranks - 1;
  next = (rank + 1) % numranks;
}
    else {
  prev = rank - 1;
  next = (rank + 1) % numranks;
    }

    if(rank  == 0){
  startTime = MPI_Wtime();
  MPI_Send(data, giga, MPI_INT, next, tag, MPI_COMM_WORLD);
  endTime = MPI_Wtime();
  dTime = endTime - startTime;
        MPI_Recv(data, giga, MPI_INT, prev, tag, MPI_COMM_WORLD,&stat);
      printf("Process %d received a gigabyte from rank %d and it took %f \n",rank, prev, dTime);
    }
    else{
      MPI_Recv(data, giga, MPI_INT, prev, tag, MPI_COMM_WORLD,&stat);
  startTime = MPI_Wtime();
  MPI_Send(data, giga, MPI_INT, next, tag, MPI_COMM_WORLD);
  endTime = MPI_Wtime();
         dTime = endTime - startTime;
         printf("Process %d received one  gigabyte from rank %d and it took %f \n",rank, prev, dTime);

    }
    MPI_Finalize();
}
