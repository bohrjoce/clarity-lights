#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>

#include "CKTrainData.h"
#include "adaboost.h"

using namespace cv;
using namespace ml;
using namespace std;

int main(int argc, char *argv[]) {

  float stddev = 2.0;
  double spacial_aspect = atof(argv[1]);

  CKTrainData ckdata(true, stddev, spacial_aspect);
  vector<Data> people_data = ckdata.get_people_data();

  // init svm
  Ptr<SVM> svm = SVM::create();
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);

  int correct = 0;
  double accuracy = 0;
  int total = 0;
  vector<int> emote_total(NUM_EMOTIONS, 0);
  vector<int> emote_correct(NUM_EMOTIONS, 0);
  vector<vector<int>> confusion(NUM_EMOTIONS, vector<int>());
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    confusion[i] = vector<int>(NUM_EMOTIONS, 0);
  }

  vector<double> C_vals = {0.1, 1.0, 10.0};
  Data train(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));
  Data test(Mat(0, 0, CV_32F), Mat(0, 0, CV_32SC1));

  // Adaboost
  unsigned int weak_learners = 92;
  Data adaboost_data = ckdata.get_flat_data();
  Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t, weak_learners, true);

for (unsigned int c_idx = 0; c_idx < C_vals.size(); ++c_idx) {
cout << "C = " << C_vals[c_idx] << endl;
svm->setC(C_vals[c_idx]);
total = 0;
correct = 0;
for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
  for (unsigned int j = 0; j < NUM_EMOTIONS; ++j) {
    confusion[i][j] = 0;
  }
  emote_total[i] = 0;
}
  for(unsigned int i = 0; i < people_data.size(); ++i) {
  //  cout << " on person: " << i << endl;
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
 //     cout << "Empty train or test set." << endl;
      continue;
    }

    Mat reduced_train_x = adaboost.reduce_features(train.x);
    Mat reduced_test_x = adaboost.reduce_features(test.x);

    svm->train(reduced_train_x, ROW_SAMPLE, train.t);

    for (int i = 0; i < reduced_test_x.rows; ++i) {
      int response = svm->predict(reduced_test_x.row(i));
      int truth = test.t.at<int>(i);
  //    cout << response << " vs " << truth << endl;
      if (response == truth) {
        ++correct;
      }
      ++emote_total[truth-1];
      ++confusion[truth-1][response-1];
    }
    total += reduced_test_x.rows;
    accuracy = (double)correct/(double)total;
//    cout << " current acc: " << accuracy << endl;
  }
  accuracy = (double)correct/(double)total;
  double accuracy_sum = 0;
  cout << "accuracy : " << accuracy << endl;
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    accuracy = (double)confusion[i][i]/(double)emote_total[i];
    accuracy_sum += accuracy;
    cout << "emotion " << i+1 << " accuracy : " << confusion[i][i] << "/" <<
        emote_total[i] << " = " << accuracy << endl;
  }
  cout << "UAR : " << accuracy_sum/(double)NUM_EMOTIONS << endl;
  cout << "Confusion matrix: left actual, top predicted" << endl;
  cout << "1    2    3    4    5    6    7\n\n";
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    cout << i+1 << "  ";
    for (unsigned int j = 0; j < NUM_EMOTIONS; ++j) {
      cout << confusion[i][j] << " ";
    }
    cout << endl;
  }
}
  return 0;
}
