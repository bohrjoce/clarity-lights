CC = g++
LIBS = $(shell pkg-config --libs opencv)
CFLAGS = -g -Wall -Werror $(shell pkg-config --cflags opencv)

default: filesystem

svm: svm.cc
	$(CC) $(CFLAGS) svm.cc -o $@ $(LIBS)

preprocess: preprocess.cc
	$(CC) $(CFLAGS) preprocess.cc -o $@ $(LIBS)

filesystem: main.cc
	$(CC) main.cc -std=c++11 -o $@ -lboost_system -lboost_filesystem

clean:
	rm -rf svm preprocess
