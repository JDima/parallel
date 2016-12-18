CC     = mpicc
CFLAGS = -c -O3
# FC     = gfortran
# FFLAGS = -fopenmp -lm -O3
LDFLAGS= -lm
SOURCES=seidel.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=seidel

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o seidel

default: $(EXECUTABLE)

run:
	seidel $(ARGS)
