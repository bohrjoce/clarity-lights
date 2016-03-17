CC = g++

OPENCV_LIBS = -L/usr/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -lopencv_hal
LIBS = $(OPENCV_LIBS)
OPENCV_FLAGS = -I/usr/local/include/opencv -I/usr/local/include
CFLAGS = -g -Wall -Werror -std=c++11 $(OPENCV_FLAGS)

default: test

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
