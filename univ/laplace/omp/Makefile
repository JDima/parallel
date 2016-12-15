CC     = g++
CFLAGS = -c -fopenmp -lm -O3
# FC     = gfortran
# FFLAGS = -fopenmp -lm -O3
LDFLAGS= -g -fopenmp
SOURCES=laplace.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=laplace

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o laplace

default: $(EXECUTABLE)

run:
	seidel $(ARGS)
