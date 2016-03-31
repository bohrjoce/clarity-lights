#include "SVMOneVsAll.h"

using namespace cv;
using namespace ml;
using namespace std;

SVMOneVsAll::SVMOneVsAll(double C) {
  svm = {SVM::create(), SVM::create(), SVM::create(),
      SVM::create(), SVM::create(), SVM::create(), SVM::create()};
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    svm[i]->setType(SVM::C_SVC);
    svm[i]->setKernel(SVM::LINEAR);
    svm[i]->setC(C);
  }
}

void SVMOneVsAll::train(Mat train_x, Mat train_t) {
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    Mat class_train_t = train_t.clone();
    for (int j = 0; j < class_train_t.rows; ++j) {
      if (class_train_t.at<int>(j) == (int)(i+1)) {
        class_train_t.at<int>(j) = 1;
      } else {
        class_train_t.at<int>(j) = 0;
      }
    }
    svm[i]->train(train_x, ROW_SAMPLE, class_train_t);
  }
}

int SVMOneVsAll::predict(Mat test_x) {
  vector<double> classify = {0, 0, 0, 0, 0, 0, 0};
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    classify[i] = svm[i]->predict(test_x, noArray(), StatModel::RAW_OUTPUT);
  }
  double min = classify[0];
  int min_index = 0;
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    if (classify[i] < min) {
      min = classify[i];
      min_index = i;
    }
  }
  return min_index+1;
}

vector<double> SVMOneVsAll::raw_predict(Mat test_x) {
  vector<double> classify = {0, 0, 0, 0, 0, 0, 0};
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    classify[i] = svm[i]->predict(test_x, noArray(), StatModel::RAW_OUTPUT);
  }
  return classify;
}

void SVMOneVsAll::save() {
  string savedir = "res/svm";
  for (unsigned int i = 0; i < svm.size(); ++i) {
    string savefile = savedir + to_string(i) + ".xml";
    svm[i]->save(savefile);
  }
}

void SVMOneVsAll::load() {
  string loaddir = "res/svm";
  for (unsigned int i = 0; i < svm.size(); ++i) {
    string loadfile = loaddir + to_string(i) + ".xml";
    svm[i] = StatModel::load<SVM>(loadfile);
  }
}
