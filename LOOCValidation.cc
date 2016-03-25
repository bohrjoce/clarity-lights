#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>

#include "CKTrainData.h"
#include "preprocess.h"
#include "gabor_filter.h"
#include "adaboost.h"

using namespace cv;
using namespace ml;
using namespace std;

int main(int argc, char *argv[]) {

  float stddev = atof(argv[1]);

  CKTrainData ckdata(true, stddev);
  vector<Data> people_data = ckdata.get_people_data();

  // init svm
  Ptr<SVM> svm = SVM::create();
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);

  int correct = 0;
  double accuracy = 0;
  int total = 0;
  Data train(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));
  Data test(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));

  // Adaboost
  Data adaboost_data = ckdata.get_flat_data();
  Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t, 82, false);

  for(unsigned int i = 0; i < people_data.size(); ++i) {
    cout << " on person: " << i << endl;
    train.x.release();
    train.t.release();
    test.x.release();
    test.t.release();

    for (unsigned int j = 0; j < people_data.size(); ++j){
      if (j == i) {
        test.x.push_back(people_data[j].x);
        test.t.push_back(people_data[j].t);
      } else {
        train.x.push_back(people_data[j].x);
        train.t.push_back(people_data[j].t);
      }
    }

    if (train.x.rows <= 0 || test.x.rows <= 0) {
      cout << "Empty train or test set." << endl;
      continue;
    }

    Mat reduced_train_x = adaboost.reduce_features(train.x);
    Mat reduced_test_x = adaboost.reduce_features(test.x);

    svm->train(reduced_train_x, ROW_SAMPLE, train.t);

    for (int i = 0; i < reduced_test_x.rows; ++i) {
      int response = svm->predict(reduced_test_x.row(i));
      cout << response << " vs " << (int)test.t.at<int>(i) << endl;
      if (response == (int)test.t.at<int>(i)) ++correct;
    }
    total += reduced_test_x.rows;
    accuracy = (double)correct/(double)total;
    cout << " current acc: " << accuracy << endl;
  }
  accuracy = (double)correct/(double)total;

  cout << "accuracy : " << accuracy << endl;

  return 0;
}
