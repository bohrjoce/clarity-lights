#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>

#include "CKTrainData.h"
#include "adaboost.h"
#include "SVMOneVsAll.h"
#include "ConfusionMatrix.h"

using namespace cv;
using namespace ml;
using namespace std;

int main(int argc, char *argv[]) {

  float stddev = 2.0;
  double spacial_aspect = 2.0;
  double C = 10.0;
  unsigned int weak_learners = 92;
  cout << "stddev = " << stddev << endl;
  cout << "spacial_aspect = " << spacial_aspect << endl;
  cout << "C = " << C << endl;
  cout << "weak_learners = " << weak_learners << endl;

  CKTrainData ckdata(true, stddev, spacial_aspect);
  unsigned int num_people = ckdata.get_num_people();

  vector<vector<string>> filename_data = ckdata.get_filename_data();

  SVMOneVsAll svm(C); // init svm

  ConfusionMatrix confusion_matrix;

  vector<string> test_filenames;
  Data train(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));
  Data test(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));

  // Adaboost
  Data adaboost_data = ckdata.get_flat_data();
  Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t, weak_learners, false);

  for(unsigned int person = 0; person < num_people; ++person) {
//    cout << " on person: " << person << endl;

    ckdata.partition_LOO_data(train, test, person);

    if (train.x.rows <= 0 || test.x.rows <= 0) {
      continue;
    }

    Mat reduced_train_x = adaboost.reduce_features(train.x);
    Mat reduced_test_x = adaboost.reduce_features(test.x);

    svm.train(reduced_train_x, train.t);

    for (int i = 0; i < reduced_test_x.rows; ++i) {
      int response = svm.predict(reduced_test_x.row(i));
      int truth = test.t.at<int>(i);
      confusion_matrix.update(response, truth);
      if (response != truth) {
        // Do something with filename if incorrectly classified
        // cout << test_filenames[i] << endl;
        // Mat image = imread(test_filenames[i], CV_LOAD_IMAGE_COLOR);
        // namedWindow("Misclassified Image", WINDOW_AUTOSIZE);
        // imshow("Misclassified Image", image);
        // waitKey(0);
      }
    }
  }
  confusion_matrix.print();
  return 0;
}
