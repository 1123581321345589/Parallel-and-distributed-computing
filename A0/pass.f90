program ping
    implicit none
    include 'mpif.h'

    integer :: numtasks, rank, dest, source,count, tag, ierr
    integer :: stat(MPI_STATUS_SIZE)   !required variable for receive routines
    integer :: inmsg,outmsg
    outmsg = 0
    tag = 1
    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)

    ! task 0 sends to task 1 and waits to receive a return message
    if (rank==0) then
    write(*,*)
    Write(*,'(A)') 'Testing MPI Message Passing...'
        dest = 1
        source = 1
    call MPI_SEND(outmsg, 1, MPI_INTEGER, dest, tag, MPI_COMM_WORLD, ierr)
    call MPI_RECV(inmsg, 1, MPI_INTEGER, source, tag, MPI_COMM_WORLD, stat,ierr)

    ! task 1 waits for task 0 message then returns a message
    else if (rank==1) then
        dest = 0
        source= 0 
        call MPI_RECV(inmsg, 1, MPI_INTEGER, source, tag, MPI_COMM_WORLD, stat,ierr)
        outmsg=inmsg+1
        call MPI_SEND(outmsg, 1, MPI_INTEGER, dest, tag, MPI_COMM_WORLD, ierr)
    endif

    ! query recieve Stat variable and print message details
    call MPI_GET_COUNT(stat, MPI_CHARACTER, count, ierr)
    if(rank==0) then
        if(inmsg==1) then
            WRITE(*,'(A)') 'Message Pass Succeeded'
        else
            WRITE(*,'(A)') 'Message Pass Failed'
        endif
        write(*,*)
    endif
    !WRITE(*,*)  'Task ',rank,': Received', count, 'char(s) from task', stat(MPI_SOURCE),'with tag',stat(MPI_TAG)

    call MPI_FINALIZE(ierr)

end program ping
