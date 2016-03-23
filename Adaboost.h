#include <iostream>

using namespace std;

struct Data {
  Mat x;
  Mat t;
};

class Adaboost {
  private:
    Data data;
    vector<Mat> binary_labels; // index i is binary label for emotion i
  public:
    Adaboost(Mat data_, Mat labels_) : data(data), labels(labels_)
    Data feature_selection();
};
