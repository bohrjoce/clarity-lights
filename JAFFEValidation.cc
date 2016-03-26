#include "JAFFEValidation.h"
#include "preprocess.h"
#include "gabor_filter.h"
#include "CKTrainData.h"
#include "data.h"
#include "adaboost.h"
#include <iostream>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace ml;
using namespace boost::filesystem;


JaffeImages::JaffeImages() {
    path p(jaffeDir);
    string filename, emotion;
    int e;
    if (exists(p) && is_directory(p)) {
        directory_iterator it(p), eod;
        BOOST_FOREACH(path const &file, make_pair(it, eod)) {
            if(is_regular_file(file) && file.extension() == ".tiff") {
                filename = file.filename().string();
                filenames.push_back(filename);
                emotion = filename.substr(filename.find(".") + 1, 2);
                if (emotion == "HA") {
                    e = 5;
                } else if (emotion == "SA") {
                    e = 6;
                } else if (emotion == "SU") {
                    e = 7;
                } else if (emotion == "AN") {
                    e = 1;
                } else if (emotion == "NE") {
                    e = 2;
                } else if (emotion == "DI") {
                    e = 3;
                } else if (emotion == "FE") {
                    e = 4;
                } else {
                    cout << emotion << endl;
                    exit(1);
                }
                labels.push_back(e);
            }
        }
    } else {
        cerr << "JAFFE directory missing." << endl;
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    JaffeImages jaffeImages;

    float stddev = 2.0;
    double spacial = 2.0;
    double C_val = 10.0;
    CKTrainData ckdata(true, stddev, spacial);
    Data train = ckdata.get_flat_data();

    unsigned int weak_learners = 82;
    Data adaboost_data = ckdata.get_flat_data();
    Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t,
        weak_learners, false);

    Mat reduced_train_x = adaboost.reduce_features(train.x);

    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setC(C_val);
    svm->train(reduced_train_x, ROW_SAMPLE, train.t);

    vector<int> emote_total(NUM_EMOTIONS, 0);
    vector<int> emote_correct(NUM_EMOTIONS, 0);
    vector<vector<int>> confusion(NUM_EMOTIONS, vector<int>());
    for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
      confusion[i] = vector<int>(NUM_EMOTIONS, 0);
    }

    int correct = 0, total = 0;
    Mat image, test_x, reduced_test_x;

    for (unsigned int i = 0; i < jaffeImages.filenames.size(); i++) {
        test_x.release();
        reduced_test_x.release();
        if (preprocess(jaffeDir + jaffeImages.filenames[i], image) == 0) {
            // imshow("Face", image);
            // waitKey(0);
            test_x = ImageToFV(image, stddev);
            reduced_test_x = adaboost.reduce_features(test_x);
            int response = svm->predict(reduced_test_x);
            int truth = jaffeImages.labels[i];
            //cout << response << " vs " << truth << endl;
            if (response == truth) {
                ++correct;
            }
            ++total;
            ++emote_total[truth-1];
            ++confusion[truth-1][response-1];
        }
    }

    double accuracy_sum = 0;
    double accuracy = (double)correct/(double)total;
    cout << "accuracy : " << accuracy << endl;
    for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
      accuracy = (double)confusion[i][i]/(double)emote_total[i];
      accuracy_sum += accuracy;
      cout << "emotion " << i+1 << " accuracy : " << confusion[i][i] << "/" <<
          emote_total[i] << " = " << accuracy << endl;
    }
    cout << "UAR : " << accuracy_sum/(double)NUM_EMOTIONS << endl;
    cout << "Confusion matrix: left actual, top predicted" << endl;
    cout << "1    2    3    4    5    6    7\n\n";
    for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
      cout << i+1 << "  ";
      for (unsigned int j = 0; j < NUM_EMOTIONS; ++j) {
        cout << confusion[i][j] << " ";
      }
      cout << endl;
    }
    return 0;
}
