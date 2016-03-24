#include "JaffeData.h"
#include "preprocess.h"
#include "gabor_filter.h"
#include <iostream>
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


int main() {
    JaffeImages jaffeImages;
    cout << jaffeImages.filenames.size() << endl;
    cout << jaffeImages.labels.size() << endl;

    Ptr<SVM> svm = StatModel::load<SVM>("res/svm.xml");
    double correct = 0, total = 0;
    Mat image, gabor;

    for (int i = 0; i < jaffeImages.filenames.size(); i++) {
        image.release();
        gabor.release();
        if (preprocess(jaffeDir + jaffeImages.filenames[i], image) == 0) {
            // imshow("Face", image);
            // waitKey(0);
            gabor = ImageToFV(image);
            int pred = svm->predict(gabor);
            cout << "Predicted: " << pred << endl;
            cout << "Actual: " << jaffeImages.labels[i] << endl;
            if (pred == jaffeImages.labels[i]) {
                correct++;
            }
            total++;
        }
    }
    
    cout << "Overall Accuracy: " << correct/total << endl;

    return 0;
}
