#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include "CKTrainData.h"
#include "preprocess.h"
#include "gabor_filter.h"

using namespace cv;
using namespace ml;
using namespace std;

int main() {

  int height = 512, width = 512;
  Mat image = Mat::zeros(height, width, CV_8UC3);

  CKTrainData ckdata;
  ckdata.init();
  Mat train_x;
  Mat train_t;
  Mat test_x;
  Mat test_t;
  Mat m, gabor_features;

  for (unsigned int i = 0; i < ckdata.filenames.size(); ++i) {
    for (unsigned int j = 0; j < ckdata.filenames[i].size(); ++j) {
      if (ckdata.labels[i][j] == -1) continue;
      if (preprocess(ckdata.filenames[i][j][0], m) != 0) {
        cout << "preprocess failed: " << ckdata.filenames[i][j][0] << endl;
        exit(1);
      }
      gabor_features = ImageToFV(m);
      unsigned int end = ckdata.filenames[i][j].size();
      // test generalization to new subjects
      if (i % 9) {
        train_x.push_back(gabor_features);
        train_t.push_back(Mat(1, 1, CV_32FC1, ckdata.labels[i][j]));
      } else {
        test_x.push_back(gabor_features);
        test_t.push_back(Mat(1, 1, CV_32FC1, ckdata.labels[i][j]));
      }

      if (preprocess(ckdata.filenames[i][j][end-1], m) != 0) {
        cout << "preprocess failed: " << ckdata.filenames[i][j][end-1] << endl;
        exit(1);
      }
      gabor_features = ImageToFV(m);
      // test generalization to new subjects
      if (i % 9) {
        train_x.push_back(gabor_features);
        train_t.push_back(Mat(1, 1, CV_32FC1, ckdata.labels[i][j]));
      } else {
        test_x.push_back(gabor_features);
        test_t.push_back(Mat(1, 1, CV_32FC1, ckdata.labels[i][j]));
      }
    }
    break;
  }
  cout << "finished preprocessing\n";
/*  // set up training data
  // TODO: replace dummy data with cohn-kanade data. get filenames, preprocess,
  // extract gabor features
  int labels[4] = {1, -1, -1, -1};
  Mat labelsMat(4, 1, CV_32S, labels);
  float trainingData[4][2] = { {501, 10}, {255, 10}, {501, 255}, {10, 501} };
  Mat trainingDataMat(4, 2, CV_32FC1, trainingData);*/

  // train svm

  Ptr<SVM> svm = SVM::create();
  svm->trainAuto(TrainData::create(train_x, ROW_SAMPLE, train_t));
  cout << "finished training\n";
/*  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);
  svm->setGamma(3);

  svm->train(train_x, ROW_SAMPLE, train_t);*/

  int correct = 0;
  for (int i = 0; i < test_x.rows; ++i) {
    int response = svm->predict(test_x.at<Mat>(i));
    if (response == test_t.at<int>(i)) ++correct;
  }
  double accuracy = (double)correct/(double)test_x.rows;

  cout << "accuracy : " << accuracy << endl;
/*
  // show decision regions given by svm
  for (int i = 0; i < image.rows; ++i) {
    for (int j = 0; j < image.cols; ++j) {
      float sample[2] = {(float)j, (float)i};
      Mat sampleMat(1, 2, CV_32FC1, sample);
      float response = svm->predict(sampleMat);

      if ((int)response == 1) image.at<Vec3b>(i,j) = green;
      else if ((int)response == -1) image.at<Vec3b>(i,j) = blue;
    }
  }
*/
/*  // show training data
  int thickness = -1;
  int lineType = 8;
  circle( image, Point(501,  10), 5, Scalar(  0,   0,   0), thickness, lineType );
  circle( image, Point(255,  10), 5, Scalar(255, 255, 255), thickness, lineType );
  circle( image, Point(501, 255), 5, Scalar(255, 255, 255), thickness, lineType );
  circle( image, Point(10 , 501), 5, Scalar(255, 255, 255), thickness, lineType );

  // show support vectors
  thickness = 2;
  lineType = 8;
  Mat sv = svm->getSupportVectors();

  for (int i = 0; i < sv.rows; ++i) {
    const float* v = sv.ptr<float>(i);
    circle( image, Point( (int) v[0], (int) v[1]), 6, Scalar(128, 128, 128), thickness, lineType );
  }
  imwrite("result.png", image);
  imshow("SVM simple example", image);
  waitKey(0);*/
}
