CC     = mpicc
CFLAGS = -c -O3
LDFLAGS= -lm
SOURCES=poisson.c
OBJECTS=$(SOURCES:.c=.o)
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
	poisson $(ARGS)
