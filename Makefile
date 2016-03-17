CC = g++
LIBS = $(shell pkg-config --libs opencv)
CFLAGS = -g -Wall -Werror -std=c++11 $(shell pkg-config --cflags opencv)

default: filesystem

svm: svm.cc
	$(CC) $(CFLAGS) svm.cc -o $@ $(LIBS)

preprocess.o: preprocess.cc preprocess.h
	$(CC) $(CFLAGS) preprocess.cc -c $(LIBS)

test: test.cc preprocess.o
	$(CC) $(CFLAGS) test.cc preprocess.o -o $@ $(LIBS)

filesystem: filesystem.cc CKTrainData.cc
	$(CC) filesystem.cc CKTrainData.cc -std=c++11 -o $@ -lboost_system -lboost_filesystem

clean:
	rm -rf *.o svm test
