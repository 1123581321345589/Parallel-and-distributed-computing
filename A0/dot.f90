module myDot
    implicit none
    include 'mpif.h'

    contains

    real(kind=8) function serialDot(x,y) result(sum)
        implicit none
        real(kind=8), dimension(:) :: x,y
        integer :: i

        if(size(x) .ne.  size(y)) then
            sum=0
        else

            do i=1,size(x)
                sum=sum+x(i)*y(i)
            enddo
        endif

    end function serialDot

    real(kind=8) function parallelDot(x,y) result(dot)
        implicit none
        include 'mpif.h'
        real(kind=8) :: mysum
        integer ierr,i
        real(kind=8), dimension(:) :: x,y
        if(size(x).ne.size(y)) then
            dot=0
        else
            mysum = serialDot(x,y)
            call MPI_REDUCE(mysum,dot,1,MPI_DOUBLE_PRECISION,MPI_SUM,0,MPI_COMM_WORLD,ierr)
        endif

    end function parallelDot
end module myDot



program dot
    use myDot
    implicit none
    integer numtasks, rank, dest, source,count, tag, ierr
    integer stat(MPI_STATUS_SIZE)   !required variable for receive routines
    real(kind=8), allocatable, dimension(:) :: x,y,localx,localy
    integer n,localn,i
    real(kind=8) :: res

    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)
    if(rank==0) then
        WRITE(*,'(A)') "Testing Parallel Dot Product..."
    endif

    n=1000
    localn=n/numtasks

    !root allocates the inital arrays
    if(rank==0) then
        allocate(x(n),y(n))
        do i=1,n
            x(i)=1.0
            y(i)=1.0
        enddo
    endif
    !everyone allocates their local arrays
    allocate(localx(localn),localy(localn))

    !scatter
    call MPI_SCATTER(x,localn,MPI_DOUBLE_PRECISION,localx,localn,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
    call MPI_SCATTER(y,localn,MPI_DOUBLE_PRECISION,localy,localn,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)

    !call parallel dot product (includes the mpi reduce)
    res=parallelDot(localx,localy) 
    

    if(rank==0) then
        if(res==n) then
            Write(*,'(A)') "Test Passed"
        else
            Write(*,'(A)') "Test Failed"
        endif
        Write(*,*)
    endif


    if(allocated(x)) deallocate(x)
    if(allocated(y)) deallocate(y)
    if(allocated(localx)) deallocate(localx)
    if(allocated(localy)) deallocate(localy)
    call MPI_FINALIZE(ierr)

end program dot
