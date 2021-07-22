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
    int prev, next;

    int assignment = (1024 * 1024 * 1024);
    int *data = (int*) malloc(assignment * sizeof(int));
    double startTime, endTime;
    double elapsed;
   





  for(int i = 1; i < numranks; i++){

    if(rank == 0 ){
  prev = i;
  next = rank + i;
}
    else{
  prev = 0;
  next = 0;

}
    if(rank  == 0){
  startTime = MPI_Wtime();
        MPI_Send(data, assignment, MPI_INT, next, tag, MPI_COMM_WORLD);
  endTime = MPI_Wtime();
        elapsed = endTime - startTime;
  MPI_Recv(data, assignment, MPI_INT, prev, tag, MPI_COMM_WORLD,&stat);
        printf("Process %d received a gigabyte from rank %d and it took %f \n",rank, prev, elapsed);
    }
    else{
         MPI_Recv(data, assignment, MPI_INT, prev, tag, MPI_COMM_WORLD,&stat);
  startTime = MPI_Wtime();
         MPI_Send(data, assignment, MPI_INT, next, tag, MPI_COMM_WORLD);
  endTime = MPI_Wtime();
         elapsed = endTime - startTime;
         printf("Process %d received a gigabyte from rank %d and it took %f \n",rank, prev, elapsed);

    }

}
    MPI_Finalize();
}
