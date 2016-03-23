#include "Adaboost.h"

Adaboost::Adaboost(Mat data_, Mat labels_) : data(data_), labels(labels_) {
  binary_labels.push_back(Mat(0, 0, CV_32SC1));
  for (unsigned int i = 1; i < 8; ++i) {
    Mat current_labels = labels.clone();
    for (unsigned int j = 0; j < current_labels.rows; ++j) {
      if (current_labels.at<int>(j) != i) {
        current_labels.at<int>(j) = 0;
      } else {
        current_labels.at<int>(j) = 1;
      }
    }
    binary_labels.push_back(current_labels);
  }
}

Data feature_selection() {

}
