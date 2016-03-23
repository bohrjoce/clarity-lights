#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "KDEFValidation.h"
#include "CKTrainData.h"
#include "preprocess.h"
#include "gabor_filter.h"

using namespace cv;
using namespace ml;
using namespace std;

int main() {
  KDEFValidation kd;

  
  // kd.print_samples();
//  return 1;
  int height = 512, width = 512;
  Mat image = Mat::zeros(height, width, CV_8UC3);

  CKTrainData ckdata;
  ckdata.init();
  for (unsigned int i = 0; i < ckdata.filenames.size(); ++i) {
    for (unsigned int j = 0; j < ckdata.filenames[i].size(); ++j) {
      Mat m;
      preprocess(ckdata.filenames[i][j][0], m);
      namedWindow("Display window.", WINDOW_AUTOSIZE);
      imshow("Display window.", m);
      waitKey(0);
      int end = ckdata.filenames[i][j].size();
      preprocess(ckdata.filenames[i][j][end-1], m);
      namedWindow("Display window.", WINDOW_AUTOSIZE);
      imshow("Display window.", m);
      waitKey(0);
    }
  }
  Mat allSamples(0, 0, CV_32F);
  Mat allLabels(0, 0, CV_32SC1);

  Mat m, gabor_features;
  vector<Mat>pFeats;
  vector<Mat>pLabels;
  Mat curPersonFeatures(0,0,CV_32F);
  Mat curPersonLabels(0,0,CV_32SC1);
  
  Mat testKDF(0, 0, CV_32F);
  Mat testKDL(0, 0, CV_32SC1); 
  /*
   for (unsigned int i = 0; i < kd.samples.size(); ++i){
    if (kd.samples[i].emotion ==2) continue;
      if (preprocess(kd.samples[i].filepath, m) != 0) {
        cout << "preprocess failed: " << kd.samples[i].filepath << endl;
        exit(1);
      }
      // cout << "iteration for j is value " << j << endl;
      gabor_features = ImageToFV(m);
      testKDF.push_back(gabor_features);
      testKDL.push_back(Mat(1, 1, CV_32SC1, kd.samples[i].emotion));
  }

  cout << " testKDF size is " << testKDF.size() << endl; 
  */

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

      gabor_features = ImageToFV(m);
      
      // cout << " size of gabor_features is " << gabor_features.size() << endl; 
      unsigned int end = ckdata.filenames[i][j].size();
      // test generalization to new subjects
      /*  TEST: use raw greyscale instead of gabor
      gabor_features.release();
      m.convertTo(gabor_features, CV_32F, 1.0/255.0);
      gabor_features = gabor_features.reshape(1,1);*/

      // first frame in sequence. label = neutral = 2. 2 was old contempt label
      curPersonFeatures.push_back(gabor_features);
      curPersonLabels.push_back(Mat(1, 1, CV_32SC1, 2));

      unsigned int end = ckdata.filenames[i][j].size();
      if (preprocess(ckdata.filenames[i][j][end-1], m) != 0) {
        cout << "preprocess failed: " << ckdata.filenames[i][j][end-1] << endl;
        exit(1);
      }

      gabor_features = ImageToFV(m);
 
      // test generalization to new subjects
      if (i % 11) {
        train_x.push_back(gabor_features);
        train_t.push_back(Mat(1, 1, CV_32SC1, ckdata.labels[i][j]));
      } else {
        test_x.push_back(gabor_features);
        test_t.push_back(Mat(1, 1, CV_32SC1, ckdata.labels[i][j]));
      }
      /*  TEST: use raw greyscale instead of gabor
      gabor_features.release();
      m.convertTo(gabor_features,CV_32F, 1.0/255.0);
      gabor_features = gabor_features.reshape(1,1);*/

      // last frame in sequence. label = ckdata.labels[i][j]
      curPersonFeatures.push_back(gabor_features);
      curPersonLabels.push_back(Mat(1, 1, CV_32SC1, ckdata.labels[i][j]));
    }
    // cout << " persons features size is " << curPersonFeatures.size() << endl;
    pFeats.push_back(curPersonFeatures);
    pLabels.push_back(curPersonLabels);
  }

  cout << "pFeats is " << pFeats.size() << endl;
  cout << "pLabels is " << pLabels.size() << endl;

  // train svm
  Ptr<SVM> svm = SVM::create();
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);
  // TODO: get this to work
  //  svm->trainAuto(TrainData::create(train_x, ROW_SAMPLE, train_t));
  //INIT VALUES:
  //i=174
  //correct = 141
  int correct = 0;

  cout << "started training\n";

  int numPeople = pFeats.size();
  int total = 0;
  Mat train_x(0, 0, CV_32F);
  Mat train_t(0, 0, CV_32SC1);
  Mat test_x(0, 0, CV_32F);
  Mat test_t(0, 0, CV_32SC1);
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
 /* 
  correct = 0;
  for(int i = 0; i < testKDF.rows; ++i){
    int response = svm->predict(testKDF.row(i));
    cout << response << " vs " << (int)testKDL.at<int>(i) << endl;
    if (response == (int)testKDL.at<int>(i)) ++correct;
  
  }
  double accuracy = (double)correct/(double)testKDF.rows;
*/

  double accuracy = (double)correct/(double)total;

  cout << "accuracy : " << accuracy << endl;

/*
  Testing on an external image

  cout << "accuracy : " << accuracy << endl;
 
  // Train model on cohn-kanade and save xml
  svm->train(train_x, ROW_SAMPLE, train_t);
  svm->save("res/svm.xml");
  // OR
  // Load trained svm model
  // svm = StatModel::load<SVM>("res/svm.xml");

  cout << "finished training, model saved in svm.xml\n";


  Mat result, g1;
  if (preprocess("res/happy.jpg", result) == 0) {
    g1 = ImageToFV(result);
    float response = svm->predict(g1);
    cout << "Response: " << response << endl;
  }*/
}
