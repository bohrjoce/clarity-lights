#include <opencv2/opencv.hpp>

int main() {
  /* Read data from csv file into CvMat */
  CvMLData data;
  data.read_csv("iris.csv");
  //const CvMat* train_x = data.get_values();

  CvMLData target;
  target.read_csv("iris_labels.csv");
  //const CvMat* train_t = target.get_values();

  /* TODO svm train, test */
}
