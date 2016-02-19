#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
  /* Read data from csv file into CvMat */
  CvMLData data;
  data.read_csv("iris.csv");
  const CvMat* data_mat1 = data.get_values();
  cv::Mat data_mat = cv::Mat(data_mat1, true);

  CvMLData target;
  target.read_csv("iris_labels.csv");
  const CvMat* target_mat1 = target.get_values();
  cv::Mat target_mat = cv::Mat(target_mat1, true);

  /* Partition data into train/test sets */
  cv::Mat train_x = data_mat.rowRange(0, data_mat.rows-10);
  cv::Mat train_t = target_mat.rowRange(0, target_mat.rows-10);
  cv::Mat test_x = data_mat.rowRange(data_mat.rows-10, data_mat.rows);
  cv::Mat test_t = target_mat.rowRange(target_mat.rows-10, target_mat.rows);
  CvSVM svm;
  CvSVMParams params(
                      CvSVM::C_SVC,    /* Type of SVM, here multiclass */
                      CvSVM::LINEAR,   /* kernel type */
                      0.0,  /* kernel param (degree), poly kernel only */
                      0.0,  /* kernel param (gamma), poly/rbf kernel only */
                      0.0,  /* kernel param (coef0), poly/sigmoid kernel only */
                      10,   /* SVM optimization param C */
                      0,    /* SVM optimization param nu (not used for
                               multiclass) */
                      0,    /* SVM optimization param p (not used for
                               multiclass) */
                      NULL, /* class weights (or priors), optional */

                      /* termination criteria for learning algorithm */
                      cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000,
                                     0.000001)
                    );
  svm.train(train_x, train_t, cv::Mat(), cv::Mat(), params);
  // initialize to zeros using target_mat[0:10]. For some reason default
  // cv::Mat constructor results in segfault
  CvMat result_t_cvmat = target_mat.rowRange(0,10);
  const CvMat test_x_cvmat = test_x;
  svm.predict(&test_x_cvmat, &result_t_cvmat);
  cv::Mat result_t = cv::Mat(&result_t_cvmat, true);
  int correct = 0;
  for (int i = 0; i < test_t.rows; ++i) {
    if ((int)result_t.at<float>(i) == (int)test_t.at<float>(i)) ++correct;
  }
  double accuracy = (double)correct/(double)test_t.rows;
  std::cout << "accuracy: " << accuracy << "\n";
}
