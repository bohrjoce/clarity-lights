CC = g++
LIBS = $(shell pkg-config --libs opencv)
CFLAGS = -g -Wall -Werror $(shell pkg-config --cflags opencv)

default: svm

svm: svm.cc
	$(CC) $(CFLAGS) svm.cc -o $@ $(LIBS)

preprocess.o: preprocess.cc preprocess.h
	$(CC) $(CFLAGS) preprocess.cc -c $(LIBS)

test: test.cc preprocess.o
	$(CC) $(CFLAGS) test.cc preprocess.o -o $@ $(LIBS)

clean:
	rm -rf *.o svm test
