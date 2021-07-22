program simple

   ! required MPI include file
   include 'mpif.h'

   integer numtasks, rank, len, ierr  
   character(MPI_MAX_PROCESSOR_NAME) hostname

   ! initialize MPI
   call MPI_INIT(ierr)

   ! get number of tasks
   call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)

   ! get my rank
   call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)

   ! this one is obvious
   call MPI_GET_PROCESSOR_NAME(hostname, len, ierr)
   write(*,'(A,I3,A,I3,A,A)') 'Number of tasks=',numtasks,' My rank=',rank,' Running on ',trim(hostname)

   ! done with MPI
   call MPI_FINALIZE(ierr)

end program simple


