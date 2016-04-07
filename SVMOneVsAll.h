#ifndef SVMONEVSALL_H
#define SVMONEVSALL_H

#include <vector>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "adaboost.h" // for NUM_EMOTIONS

class SVMOneVsAll {
  private:
    std::vector<cv::Ptr<cv::ml::SVM>> svm;

  public:
    SVMOneVsAll(double C = 10.0);
    void train(cv::Mat train_x, cv::Mat train_t);
    int predict(cv::Mat test_x);
    cv::Mat create_svm_features(cv::Mat test_x);
    std::vector<double> raw_predict(cv::Mat test_x);
    void save(std::string base_dir);
    void load(std::string base_dir);
};

#endif
