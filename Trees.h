#ifndef TREES_H
#define TREES_H

#include <vector>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "adaboost.h" // for NUM_EMOTIONS


class Trees{
  private:
    cv::Ptr<cv::ml::DTrees> tree;

  public:
    Trees();
    void train(cv::Mat train_x, cv::Mat train_t);
    int predict(cv::Mat test_x);
    void save(std::string base_dir);
    void load(std::string base_dir);

};

#endif 
