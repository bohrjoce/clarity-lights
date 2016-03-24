#include "adaboost.h"
#include <set>
// #include "NuggetBoost.h"

using namespace std;
using namespace cv;
using namespace ml;

Adaboost::Adaboost(Mat data_, Mat labels_) {

  // Error checking
  cout << "checking" << endl;
  if (data_.rows != labels_.rows || labels_.cols != 1) {
    cout << "Mats are formatted incorectly." << endl;
    exit(1);
  }

  // Copy data (shallow copy)
  cout << "copying" << endl;
  data = data_.clone();
  unsigned int num_samples = data.rows;

  // Push back a Mat to hold binary labels for each emotion, init to 0s
  cout << "pushing" << endl;
  for (unsigned int i = 0; i < NUM_EMOTIONS; i++) {
    labels.push_back(Mat::zeros(num_samples, 1, CV_32SC1));
  }

  // Construct binary labels
  cout << "constructing" << endl;
  for (unsigned int i = 0; i < num_samples; i++) {
    labels[labels_.at<int>(i) - 1].at<int>(i) = 1;
  }

  // // Print labels
  // cout << "printing" << endl;
  // cout << "1\t" << "2\t" << "3\t" << "4\t" << "5\t" << "6\t" << "7\t" << endl;
  // for (unsigned int i = 0; i < num_samples; i++) {
  //   for (unsigned int j = 0; j < NUM_EMOTIONS; j++) {
  //     cout << labels[j].at<int>(i) << "\t";
  //   }
  //   cout << endl;
  // }

  // for (unsigned int i = 1; i < 8; ++i) {
  //   Mat current_labels = labels.clone();
  //   for (unsigned int j = 0; j < current_labels.rows; ++j) {
  //     if (current_labels.at<int>(j) != i) {
  //       current_labels.at<int>(j) = 0;
  //     } else {
  //       current_labels.at<int>(j) = 1;
  //     }
  //   }
  //   labels.push_back(current_labels);
  // }
}

Mat Adaboost::feature_selection(unsigned int weak_learners) {

  Mat output = Mat(data.rows, 0, CV_32F);
  set<int> indices;
  
  for (unsigned int i = 0; i < NUM_EMOTIONS; i++) {
    cout << "Emotion: " << i << endl;
    // Create new boost model
    Ptr<Boost> boost = Boost::create();
    boost->setBoostType(Boost::REAL);
    boost->setWeakCount(weak_learners);
    boost->setMaxDepth(1);
    // Train model
    boost->train(data, ROW_SAMPLE, labels[i]);
    vector<DTrees::Split> splits = boost->getSplits();
    // Print features selected
    for (unsigned int i = 0; i < splits.size(); i++) {
      cout << "Split " << i << ", varIdx = " << splits[i].varIdx << ", quality = " << splits[i].quality << endl;
    }
    // Combine indices
    for (unsigned int i = 0; i < splits.size(); i++) {
      indices.insert(splits[i].varIdx);
    }
  }

  // Return data with only selected features
  for (set<int>::iterator it = indices.begin(); it != indices.end(); it++) {
    hconcat(output, data.col(*it), output);
  }
  cout << "Samples = " << output.rows << ", # features = " << output.cols << endl;
  return output;
}

// int main() {
//   return 0;
// }