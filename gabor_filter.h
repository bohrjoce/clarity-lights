#ifndef GABOR_FILTER_H
#define GABOR_FILTER_H

#define pi 3.14
#include <set>
using namespace cv;
//! [namespace]

using namespace std;

//pass the address of weak_learners_indicies
//sample call:
//        image = ImageToFV(image,stddev,31,2.0,true,&(adaboost.weak_learners_indices));

Mat ImageToFV(Mat inputImage, float stddev = 2.0, int filtSize = 31, double spacial_aspect = 2.0,bool visualize = false, set<int> *weak_learners_indices= NULL); 
#endif
