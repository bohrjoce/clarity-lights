#ifndef DATA_H
#define DATA_H

#include <opencv2/opencv.hpp>
#include <iostream>

struct Data {
  cv::Mat x;
  cv::Mat t;
  Data(cv::Mat x_, cv::Mat t_) : x(x_.clone()), t(t_.clone()) {}
};

#endif
