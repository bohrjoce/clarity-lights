#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>

#include "CKTrainData.h"
#include "adaboost.h"
#include "SVMOneVsAll.h"
#include "ConfusionMatrix.h"
#include "JAFFEValidation.h"
#include "KDEFValidation.h"

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

  JaffeImages jaffedata;
  CKTrainData ckdata(true, stddev, spacial_aspect);
  KDEFValidation kdefdata;
  vector<Data> ck_people_data = ckdata.get_people_data();
  vector<Data> people_data = ckdata.get_people_data();
  vector<Data> jaffe_people_data = jaffedata.get_people_data();
  vector<Data> kdef_people_data = kdefdata.get_people_data();
  copy(jaffe_people_data.begin(), jaffe_people_data.end(),
      back_inserter(people_data));
  copy(kdef_people_data.begin(), kdef_people_data.end(),
      back_inserter(people_data));

  unsigned int jaffe_start = ck_people_data.size();
  unsigned int kdef_start = jaffe_start + jaffe_people_data.size();
//  vector<vector<string>> filename_data = ckdata.get_filename_data();

  SVMOneVsAll svm(C); // init svm

  ConfusionMatrix confusion_matrix;
  ConfusionMatrix ck_confusion_matrix;
  ConfusionMatrix jaffe_confusion_matrix;
  ConfusionMatrix kdef_confusion_matrix;

  Data train(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));
  Data test(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));

  // Adaboost
  Data adaboost_data(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));
  for(unsigned int i = 0; i < people_data.size(); ++i) {
    adaboost_data.x.push_back(people_data[i].x);
    adaboost_data.t.push_back(people_data[i].t);
  }
  Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t, weak_learners, true);

  for(unsigned int person = 0; person < people_data.size(); ++person) {
//    cout << " on person: " << person << endl;
    train.x.release();
    train.t.release();
    test.x.release();
    test.t.release();

    for (unsigned int i = 0; i < people_data.size(); ++i) {
      if (i == person) {
        test.x.push_back(people_data[i].x);
        test.t.push_back(people_data[i].t);
      } else {
        train.x.push_back(people_data[i].x);
        train.t.push_back(people_data[i].t);
      }
    }

    if (train.x.rows <= 0 || test.x.rows <= 0) {
      continue;
    }

    Mat reduced_train_x = adaboost.reduce_features(train.x);
    Mat reduced_test_x = adaboost.reduce_features(test.x);

    svm.train(reduced_train_x, train.t);

    Mat svm_features = svm.create_svm_features(reduced_train_x);





    for (int i = 0; i < reduced_test_x.rows; ++i) {
      int response = svm.predict(reduced_test_x.row(i));
      int truth = test.t.at<int>(i);
      confusion_matrix.update(response, truth);
      if (person < jaffe_start) {
        ck_confusion_matrix.update(response, truth);
      } else if (person < kdef_start) {
        jaffe_confusion_matrix.update(response, truth);
      } else {
        kdef_confusion_matrix.update(response, truth);
      }
      if (response != truth) {
        // Do something with filename if incorrectly classified
        // cout << test_filenames[i] << endl;
/*        Mat m;
        if (preprocess(filename_data[person][i], m) != 0) {
          cout << "preprocess failed: "<< endl;
          exit(1);
        }

        Mat gabor_features = ImageToFV(m,2.0,31,2.0,true);*/
        //namedWindow("Misclassified Image", WINDOW_AUTOSIZE);
        //imshow("Misclassified Image", image);
        //waitKey(0);
      }
    }
  }
  cout << "--------CK STATISTICS--------\n";
  ck_confusion_matrix.print();
  cout << "--------JAFFE STATISTICS--------\n";
  jaffe_confusion_matrix.print();
  cout << "--------KDEF STATISTICS--------\n";
  kdef_confusion_matrix.print();
  cout << "--------ALL STATISTICS--------\n";
  confusion_matrix.print();
  return 0;
}
