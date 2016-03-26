#ifndef GABOR_FILTER_H
#define GABOR_FILTER_H

#define pi 3.14
using namespace cv;
//! [namespace]

using namespace std;

Mat ImageToFV(Mat inputImage, float stddev = 8.0, int filtSize = 31,bool visualize = true);

#endif
