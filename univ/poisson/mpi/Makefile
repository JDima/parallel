CC     = gcc
CFLAGS = -c -O3
# FC     = gfortran
# FFLAGS = -fopenmp -lm -O3
LDFLAGS=
SOURCES=poisson.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=poisson

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o poisson

default: $(EXECUTABLE)

run:
	seidel $(ARGS)
