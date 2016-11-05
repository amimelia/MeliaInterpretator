CC=gcc
CFLAGS= -g -Wall -std=gnu99 -Wpointer-arith -I.
HEADERS = vector.h hashset.h environment.h reader.h evaluator.h
OBJECTS = starter.o vector.o hashset.o environment.o reader.o evaluator.o

default: starter

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

starter: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f starter
