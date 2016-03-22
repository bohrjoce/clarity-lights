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
  Mat train_x(0, 0, CV_32F);
  Mat train_t(0, 0, CV_32SC1);
  Mat test_x(0, 0, CV_32F);
  Mat test_t(0, 0, CV_32SC1);
  Mat allSamples(0, 0, CV_32F);
  Mat allLabels(0, 0, CV_32SC1);

  Mat pFeatures;
  Mat m, gabor_features;
  vector<Mat>pFeats;
  vector<Mat>pLabels; 
  Mat curPersonFeatures(0,0,CV_32F);
  Mat curPersonLabels(0,0,CV_32SC1);
  for (unsigned int i = 0; i < ckdata.filenames.size(); ++i) {
    curPersonFeatures.release();
    curPersonLabels.release();
    for (unsigned int j = 0; j < ckdata.filenames[i].size(); ++j) {

      // ignore unlabeled examples and examples labeled contempt
      if (ckdata.labels[i][j] == -1 || ckdata.labels[i][j] == 2) continue;
      if (preprocess(ckdata.filenames[i][j][0], m) != 0) {
        cout << "preprocess failed: " << ckdata.filenames[i][j][0] << endl;
        exit(1);
      }
      // cout << "iteration for j is value " << j << endl;
      gabor_features = ImageToFV(m);
      // cout << " size of gabor_features is " << gabor_features.size() << endl; 
      unsigned int end = ckdata.filenames[i][j].size();
      // test generalization to new subjects
      // first frame in sequence. label = neutral = 2. 2 was old contempt label
      if (i % 25) {
        train_x.push_back(gabor_features);
        train_t.push_back(Mat(1, 1, CV_32SC1, 2));
      } else {
        test_x.push_back(gabor_features);
        test_t.push_back(Mat(1, 1, CV_32SC1, 2));
      }
      curPersonFeatures.push_back(gabor_features);
      curPersonLabels.push_back(Mat(1, 1, CV_32SC1, 2));

      if (preprocess(ckdata.filenames[i][j][end-1], m) != 0) {
        cout << "preprocess failed: " << ckdata.filenames[i][j][end-1] << endl;
        exit(1);
      }
      gabor_features = ImageToFV(m);
      // test generalization to new subjects
      if (i % 25) {
        train_x.push_back(gabor_features);
        train_t.push_back(Mat(1, 1, CV_32SC1, ckdata.labels[i][j]));
      } else {
        test_x.push_back(gabor_features);
        test_t.push_back(Mat(1, 1, CV_32SC1, ckdata.labels[i][j]));
      }
      curPersonFeatures.push_back(gabor_features);
      curPersonLabels.push_back(Mat(1, 1, CV_32SC1, ckdata.labels[i][j]));
    }
    // cout << " persons features size is " << curPersonFeatures.size() << endl;
    pFeats.push_back(curPersonFeatures); 
    pLabels.push_back(curPersonLabels);
  }

  cout << "pFeats is " << pFeats.size() << endl; 
  cout << "pLabels is " << pLabels.size() << endl;
  //for(unsigned int i = 0; i < pFeats.size(); ++i){
  //  cout << "features size for each person: " << pFeats[0].size() << endl;
  //}
  //ckdata.print_labels();
  /*  // set up training data
  // TODO: replace dummy data with cohn-kanade data. get filenames, preprocess,
  // extract gabor features
  int labels[4] = {1, -1, -1, -1};
  Mat labelsMat(4, 1, CV_32S, labels);
  float trainingData[4][2] = { {501, 10}, {255, 10}, {501, 255}, {10, 501} };
  Mat trainingDataMat(4, 2, CV_32FC1, trainingData);*/


  // train svm
  Ptr<SVM> svm = SVM::create();
  //  svm->trainAuto(TrainData::create(train_x, ROW_SAMPLE, train_t));
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);
  svm->setGamma(3);


  //INIT VALUES:
  //i=174
  //correct = 141
  int correct = 0;

  cout << "started training\n";

  int numPeople = pFeats.size();
  int total = 0;   
  for(int i = 0; i < numPeople; ++i) {
    cout << "\n\n leave one out: " << i << endl; 
    train_x.release();
    train_t.release();
    test_x.release();
    test_t.release();
    for (int j = 0; j <numPeople; ++j){
      if(j==i){ 
        test_x.push_back(pFeats[j]);
        test_t.push_back(pLabels[j]);
      }
      else{
        train_x.push_back(pFeats[j]);
        train_t.push_back(pLabels[j]);
      }
    }
    //cout << " training size is " << train_x.size() << endl;
    //cout << " test size is " << test_x.size() << endl;
    svm->train(train_x, ROW_SAMPLE, train_t);
    for (int i = 0; i < test_x.rows; ++i) {
      int response = svm->predict(test_x.row(i));
      cout << response << " vs " << (int)test_t.at<int>(i) << endl;
      if (response == (int)test_t.at<int>(i)) ++correct;
    }
    total+=test_x.rows;
    cout << " current correct " << correct << endl;
    cout << " on person: " << i << endl;
    cout << " total is " << total << endl;
  }

  double accuracy = (double)correct/(double)total;

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
