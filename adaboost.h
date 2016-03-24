#ifndef ADABOOST_H
#define ADABOOST_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

const unsigned int NUM_EMOTIONS = 7;

// struct Data {
//   cv::Mat x;
//   cv::Mat t;
// };

class Adaboost {
  private:
    cv::Mat data; // Rows - samples, columns - features
    std::vector<cv::Mat> labels; // Index i is Mat of binary labels for emotion i
  public:
    Adaboost(cv::Mat data_, cv::Mat labels_);
    cv::Mat feature_selection(unsigned int weak_learners);
};

#endif