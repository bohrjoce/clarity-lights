#include "adaboost.h"
#include <set>
#include <sys/stat.h>

using namespace std;
using namespace cv;
using namespace ml;

Adaboost::Adaboost(Mat data_, Mat labels_, unsigned int weak_learners, bool retrain) {

  cout << "Adaboost feature selection\n";
  // Error checking
  if (data_.rows != labels_.rows || labels_.cols != 1) {
    cout << "Mats are formatted incorectly." << endl;
    exit(1);
  }
  // Copy data (shallow copy)
  data = data_.clone();
  unsigned int num_samples = data.rows;

  // Push back a Mat to hold binary labels for each emotion, init to 0s
  for (unsigned int i = 0; i < NUM_EMOTIONS; i++) {
    labels.push_back(Mat::zeros(num_samples, 1, CV_32SC1));
  }

  // Construct binary labels
  for (unsigned int i = 0; i < num_samples; i++) {
    labels[labels_.at<int>(i) - 1].at<int>(i) = 1;
  }

  for (unsigned int i = 0; i < NUM_EMOTIONS; i++) {
    // Create new boost model
    string savedir = ("res/boost" + to_string(weak_learners) + "-" +
          to_string(i) + ".xml");
    Ptr<Boost> boost = Boost::create();
    boost->setBoostType(Boost::REAL);
    boost->setWeakCount(weak_learners);
    boost->setMaxDepth(1);
    if (retrain) {
      // Train model
      cout << "Emotion: " << i << endl;
      boost->train(data, ROW_SAMPLE, labels[i]);
      boost->save(savedir);
    } else {
      struct stat buffer;
      if (stat(savedir.c_str(), &buffer) != 0) {
        cout << "saved model doesn't exist. rerun adaboost with retrain == true" << endl;
        exit(0);
      }
      boost = StatModel::load<Boost>(savedir);
    }
    vector<DTrees::Split> splits = boost->getSplits();
    /*// Print features selected
    for (unsigned int i = 0; i < splits.size(); i++) {
      cout << "Split " << i << ", varIdx = " << splits[i].varIdx <<
          ", quality = " << splits[i].quality << endl;
    }*/
    // Combine indices
    for (unsigned int i = 0; i < splits.size(); i++) {
      weak_learners_indices.insert(splits[i].varIdx);
    }
  }

  cout << "Total # features = " << weak_learners_indices.size() << endl;

  // // Print labels
  // cout << "Binary labels:" << endl;
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

Mat Adaboost::reduce_features(const Mat &original_mat) {

    Mat reduced_mat(original_mat.rows, 0, CV_32F);
    // Use data with only selected features
    for (set<int>::iterator it = weak_learners_indices.begin();
            it != weak_learners_indices.end(); it++) {
      hconcat(reduced_mat, original_mat.col(*it), reduced_mat);
    }
    return reduced_mat;
}
