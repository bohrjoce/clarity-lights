#ifndef ADABOOST_H
#define ADABOOST_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>

const unsigned int NUM_EMOTIONS = 7;

class Adaboost {
  private:
    cv::Mat data; // Rows - samples, columns - features
    std::vector<cv::Mat> labels; // Index i is Mat of binary labels for emotion i
  public:
    std::set<int> weak_learners_indices;
    Adaboost(cv::Mat data_, cv::Mat labels_, unsigned int weak_learners = 82,
        bool retrain = true);
    Adaboost(std::string loadfile);
    cv::Mat reduce_features(const cv::Mat &original_mat);
    void save_features(std::string savefile);
};

#endif
