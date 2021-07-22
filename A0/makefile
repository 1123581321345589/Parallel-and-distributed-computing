FC = mpif90
LD = mpif90
CC = mpicc
LDC = mpicc
LD_FLAGS = -fopenmp
FLAGS = -fopenmp
RM= /bin/rm

CPROGS = pass.cx host.cx dot.cx
FPROGS = pass.x host.x dot.x

OBJSF=$(FPROGS:%.x=%F.o)
OBJSC=$(CPROGS:%.cx=%C.o) 

#all rule
all: $(CPROGS) $(FPROGS)

#$(FPROGS):$(OBJSF) 	
#	$(LD) $(LD_FLAGS) $@ -o $@

#$(CPROGS):$(OBJSC)
#	$(LDC) $(LD_FLAGS) $@ -o $@

pass.cx:passC.o
	$(LDC) $(LD_FLAGS) $< -o $@
host.cx:hostC.o
	$(LDC) $(LD_FLAGS) $< -o $@
dot.cx:dotC.o
	$(LDC) $(LD_FLAGS) $< -o $@
pass.x:passF.o
	$(LD) $(LD_FLAGS) $< -o $@
host.x:hostF.o
	$(LD) $(LD_FLAGS) $< -o $@
dot.x:dotF.o
	$(LD) $(LD_FLAGS) $< -o $@

%F.o: %.f90
	$(FC) $(FLAGS) -c $^ -o $@

%C.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@



#clean rule
clean:
	$(RM) -rf *.o *.x *.cx *.mod

