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
    double C = 10.0;
    unsigned int weak_learners = 92;
    cout << "stddev = " << stddev << endl;
    cout << "spacial_aspect = " << spacial << endl;
    cout << "C = " << C << endl;
    cout << "weak_learners = " << weak_learners << endl;
    CKTrainData ckdata(true, stddev, spacial);
    Data train = ckdata.get_flat_data();

    Data adaboost_data = ckdata.get_flat_data();
    Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t,
        weak_learners, false);

    Mat reduced_train_x = adaboost.reduce_features(train.x);

    SVMOneVsAll svm(C);
    svm.train(reduced_train_x, train.t);

    ConfusionMatrix confusion_matrix;

    Mat image, test_x, reduced_test_x;

    for (unsigned int i = 0; i < jaffeImages.filenames.size(); i++) {
        test_x.release();
        reduced_test_x.release();
        if (preprocess(jaffeDir + jaffeImages.filenames[i], image) == 0) {
            // imshow("Face", image);
            // waitKey(0);
            test_x = ImageToFV(image, stddev);
            reduced_test_x = adaboost.reduce_features(test_x);
            int response = svm.predict(reduced_test_x);
            int truth = jaffeImages.labels[i];
            //cout << response << " vs " << truth << endl;
            confusion_matrix.update(response, truth);
        }
    }
    confusion_matrix.print();

    return 0;
}
