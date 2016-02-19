CC = g++
LIBS = $(shell pkg-config --libs opencv)
CFLAGS = -g -Wall -Werror $(shell pkg-config --cflags opencv)

default: svm

svm: svm.cc
	$(CC) $(CFLAGS) svm.cc -o $@ $(LIBS)

clean:
	rm -rf svm
