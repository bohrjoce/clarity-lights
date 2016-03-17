#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <stdio.h>
#include <opencv2/opencv.hpp>

// Params:   imagePath - path to an image
//           output - Mat of 48x48 face detected
// Returns:  0 if face detected, -1 if no face detected      
int preprocess(std::string imagePath, cv::Mat &output);
 
#endif