EXECUTABLE = simplex
OBJS = main.o matrix.o tableau.o simplex.o dual.o

CC = g++
CFLAGS = -ggdb -c -Wall -O3

all: simplex

simplex: $(OBJS)
	$(CC) $(OBJS) -o $(EXECUTABLE)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)
