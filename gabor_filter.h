#ifndef GABOR_FILTER_H
#define GABOR_FILTER_H

#define pi 3.14
using namespace cv;
//! [namespace]

using namespace std;

Mat ImageToFV(Mat inputImage, float stddev = 2.0, int filtSize = 31, double spacial_aspect = 2.0,bool visualize = true);

#endif
