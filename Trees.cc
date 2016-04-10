#include "Trees.h"

using namespace cv;
using namespace ml;
using namespace std;

Trees::Trees(){
  tree = DTrees::create();
  tree->setMaxDepth(10);
  tree->setCVFolds(0);
  //default is 10 max params
}
void Trees::train(Mat train_x, Mat train_t) {
  tree->train(train_x, ROW_SAMPLE, train_t);
}
void Trees::save(string base_dir) {
    string savefile = base_dir + ".xml";
    tree->save(savefile);
}
void Trees::load(string base_dir) {
  string loadfile = base_dir  + ".xml";
  tree = StatModel::load<DTrees>(loadfile);
}

int Trees::predict(Mat test_x) {
  return tree->predict(test_x);
}
