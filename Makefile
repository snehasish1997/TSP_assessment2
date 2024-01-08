# Compiler definitions
CC=gcc
MPICC=mpicc
ICC=icc
MPIICC=mpiicc

# Compiler flags
CFLAGS=-fopenmp
LDFLAGS=-lm

# Source files
SOURCES=main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
MPI_SOURCES=main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c

# Executable names
OMP_EXEC=gomp-only
MPI_EXEC=gcomplete
IOMP_EXEC=iomp-only
IMPI_EXEC=icomplete

# Default target
all: $(OMP_EXEC) $(MPI_EXEC) $(IOMP_EXEC) $(IMPI_EXEC)

# OpenMP target with GNU compiler
$(OMP_EXEC): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# MPI target with GNU MPI compiler
$(MPI_EXEC): $(MPI_SOURCES)
	$(MPICC) -o $@ $^ $(LDFLAGS)

# OpenMP target with Intel compiler
$(IOMP_EXEC): $(SOURCES)
	$(ICC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# MPI target with Intel MPI compiler
$(IMPI_EXEC): $(MPI_SOURCES)
	$(MPIICC) -o $@ $^ $(LDFLAGS)

# Clean up
clean:
	rm -f $(OMP_EXEC) $(MPI_EXEC) $(IOMP_EXEC) $(IMPI_EXEC) *.o
