CC     = g++
CFLAGS = -c -Wall -fopenmp -lm -O3
# FC     = gfortran
# FFLAGS = -fopenmp -lm -O3
LDFLAGS= -g -fopenmp
SOURCES=seidel.cpp
OBJECTS=$(SOURCES:.cpp=.o)
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
