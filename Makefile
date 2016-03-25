CC = g++

OPENCV_LIBS = -L/usr/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core
BOOST_LIBS = -lboost_system -lboost_filesystem
LIBS = $(BOOST_LIBS) $(OPENCV_LIBS) -lm -ldl
OPENCV_FLAGS = -I/usr/local/include/opencv -I/usr/local/include
CFLAGS = -g -Wall -Werror -std=c++11 $(OPENCV_FLAGS)

default: LOOCValidation KDEFValidation JAFFEValidation

LOOCValidation: LOOCValidation.cc preprocess.cc CKTrainData.cc adaboost.cc
	$(CC) $(CFLAGS) LOOCValidation.cc preprocess.cc CKTrainData.cc gabor_filter.cc  adaboost.cc -std=c++11 -o $@ $(LIBS)

KDEFValidation: KDEFValidation.cc preprocess.cc CKTrainData.cc adaboost.cc
	$(CC) $(CFLAGS) KDEFValidation.cc preprocess.cc CKTrainData.cc gabor_filter.cc  adaboost.cc -std=c++11 -o $@ $(LIBS)

JAFFEValidation: JAFFEValidation.cc preprocess.cc CKTrainData.cc adaboost.cc
	$(CC) $(CFLAGS) JAFFEValidation.cc preprocess.cc CKTrainData.cc gabor_filter.cc  adaboost.cc -std=c++11 -o $@ $(LIBS)

preprocess.o: preprocess.cc preprocess.h
	$(CC) $(CFLAGS) preprocess.cc -c $(LIBS)

test: test.cc preprocess.o
	$(CC) $(CFLAGS) test.cc preprocess.o -o $@ $(LIBS)

filesystem: filesystem.cc CKTrainData.cc
	$(CC) filesystem.cc CKTrainData.cc -std=c++11 -o $@ $(BOOST_LIBS)

clean:
	rm -rf *.o LOOCValidation KDEFValidation JAFFEValidation
