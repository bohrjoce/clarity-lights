CC = g++
LIBS = $(shell pkg-config --libs opencv)
CFLAGS = -g -Wall -Werror $(shell pkg-config --cflags opencv)

default: svm

svm: svm.cc
	$(CC) $(CFLAGS) svm.cc -o $@ $(LIBS)

preprocess: preprocess.cc
	$(CC) $(CFLAGS) preprocess.cc -c $(LIBS)

test: test.cc preprocess.o
	$(CC) $(CFLAGS) test.cc preprocess.cc -o $@ $(LIBS)

clean:
	rm -rf *.o svm test
