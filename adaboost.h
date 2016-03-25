#ifndef ADABOOST_H
#define ADABOOST_H

#include <iostream>
#include <vector>
#include <set>
#include "data.h"

const unsigned int NUM_EMOTIONS = 7;

class Adaboost {
  private:
    cv::Mat data; // Rows - samples, columns - features
    std::vector<cv::Mat> labels; // Index i is Mat of binary labels for emotion i
    std::set<int> weak_learners_indices;
  public:
    Adaboost(cv::Mat data_, cv::Mat labels_, unsigned int weak_learners,
        bool retrain);
    cv::Mat reduce_features(const cv::Mat &original_mat);
};

#endif
