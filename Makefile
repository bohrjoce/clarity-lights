CC = g++

OPENCV_LIBS = -L/usr/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core 
BOOST_LIBS = -lboost_system -lboost_filesystem
LIBS = $(BOOST_LIBS) $(OPENCV_LIBS) -lm -ldl
OPENCV_FLAGS = -I/usr/local/include/opencv -I/usr/local/include
CFLAGS = -g -Wall -Werror -std=c++11 $(OPENCV_FLAGS)

default: svm

svm: svm.cc preprocess.cc CKTrainData.cc KDEFValidation.cc adaboost.cc
	$(CC) $(CFLAGS) svm.cc preprocess.cc CKTrainData.cc gabor_filter.cc  KDEFValidation.cc adaboost.cc -std=c++11 -o $@ $(LIBS)

preprocess.o: preprocess.cc preprocess.h
	$(CC) $(CFLAGS) preprocess.cc -c $(LIBS)

test: test.cc preprocess.o
	$(CC) $(CFLAGS) test.cc preprocess.o -o $@ $(LIBS)

filesystem: filesystem.cc CKTrainData.cc
	$(CC) filesystem.cc CKTrainData.cc -std=c++11 -o $@ $(BOOST_LIBS)

jaffe: JaffeData.cc preprocess.cc gabor_filter.cc
	$(CC) JaffeData.cc preprocess.cc gabor_filter.cc -std=c++11 -o $@ $(LIBS)

boost: adaboost.cc NuggetBoost.cc
	$(CC) adaboost.cc NuggetBoost.cc -std=c++11 -o $@ $(LIBS)

clean:
	rm -rf *.o svm test jaffe boost
