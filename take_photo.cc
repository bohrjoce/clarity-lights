#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "SVMOneVsAll.h"
#include "preprocess.h"
#include "gabor_filter.h"

using namespace std;

const string webcam_pic_path = "res/webcam_pic.jpeg";

int main() {
    // Set RGB vals for each emotion
    unsigned int r, g, b;
    cout << "RGB values for:" << endl;
    cout << "Anger: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    cout << "Neutral: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    cout << "Disgust: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    cout << "Fear: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    cout << "Happy: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    cout << "Sadness: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    cout << "Surprise: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;

    // Load trained SVM model
    SVMOneVsAll svm;
    svm.load();
    Adaboost adaboost("res/features.txt");

    do {
        // Take picture
        system(("streamer -f jpeg -o " + webcam_pic_path).c_str());
        // Feature extraction
        Mat webcam_pic;
        if (preprocess(webcam_pic_path, webcam_pic) != 0) {
          cout << "Preprocess failed" << endl;
          continue;
        }
        // Classify emotion
        Mat gabor_features = ImageToFV(webcam_pic);
        Mat reduced_features = adaboost.reduce_features(gabor_features);
        int response = svm.predict(reduce_features);
        cout << "Emotion: " << response << endl;
        // Delete picture
        system(("rm " + webcam_pic_path).c_str());
    } while (!sleep(60));

    return 0;
}
