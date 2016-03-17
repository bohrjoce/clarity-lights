#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>

using namespace cv;
using namespace ml;
using namespace fs = boost::filesystem

const char * datapath = "cohn-kanade/";

int main() {

  fs::path p(datapath);
  if (!exists(p) || !is_directory(p)) {
    std::cout << datapath << " does not exist\n";
    return -1;
  }
  fs::recursive_directory_iterator begin(p), end;
  std::vector<fs::directory_entry> v(begin, end);
  std::cout << "there are " << v.size() << " files\n";
  for (auto& f: v)
    std::cout << f << "\n";
  return 0;

  int height = 512, width = 512;
  Mat image = Mat::zeros(height, width, CV_8UC3);

  // set up training data
  int labels[4] = {1, -1, -1, -1};
  Mat labelsMat(4, 1, CV_32S, labels);
  float trainingData[4][2] = { {501, 10}, {255, 10}, {501, 255}, {10, 501} };
  Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

  // train svm
  Ptr<SVM> svm = SVM::create();
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);
  svm->setGamma(3);

  svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
  Vec3b green (0,255,0), blue (255,0,0);
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

  // show training data
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
  waitKey(0);
}
