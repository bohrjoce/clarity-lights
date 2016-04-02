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
    Mat m;
    Data current_person(Mat(0,0,CV_32F), Mat(0,0,CV_32SC1));
    string current_name = "KA";
    string new_name = "KA";
    current_person.x.release();
    current_person.t.release();
    if (exists(p) && is_directory(p)) {
        directory_iterator it0(p), eod0;
        vector<path> v0;
        copy(it0, eod0, back_inserter(v0));
        sort(v0.begin(), v0.end());

        for (auto it = v0.begin(); it != v0.end(); ++it) {
            path file = *it;
            if(is_regular_file(file) && file.extension() == ".tiff") {
                filename = file.filename().string();
                filenames.push_back(filename);
                emotion = filename.substr(filename.find(".") + 1, 2);
                string new_name = filename.substr(0, 2);
                if (new_name != current_name) {
                  current_name = new_name;
                  people_data.push_back(current_person);
                  current_person.x.release();
                  current_person.t.release();
                }
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
                string file_path = file.string();
                // construct current_person
                if (preprocess(file_path, m) != 0) {
                    cout << "preprocess failed: " << file_path << endl;
                    exit(1);
                }
                Mat gabor_features = ImageToFV(m);
                current_person.x.push_back(gabor_features);
                current_person.t.push_back(Mat(1, 1, CV_32SC1, e));
            }
        }
        people_data.push_back(current_person);
    } else {
        cerr << "JAFFE directory missing." << endl;
        exit(1);
    }
}
/*
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
}*/
