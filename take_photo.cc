#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "preprocess.h"
#include "gabor_filter.h"
#include "SVMOneVsAll.h"
#include "arduino.h"

using namespace std;

const string webcam_pic_path = "res/webcam_pic.jpeg";
const int interval = 60; //seconds

int main() {
    // Set RGB vals for each emotion
    unsigned int r, g, b;
    vector<Color> colors;
    cout << "RGB values for:" << endl;
    cout << "Anger: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    colors.push_back(Color(r, g, b));
    cout << "Neutral: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    colors.push_back(Color(r, g, b));
    cout << "Disgust: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    colors.push_back(Color(r, g, b));
    cout << "Fear: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    colors.push_back(Color(r, g, b));
    cout << "Happy: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    colors.push_back(Color(r, g, b));
    cout << "Sadness: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    colors.push_back(Color(r, g, b));
    cout << "Surprise: ";
    cin >> r >> g >> b;
    cout << r << " " << g << " " << b << endl;
    colors.push_back(Color(r, g, b));

    // Load trained SVM model
    SVMOneVsAll svm;
    svm.load();
    Adaboost adaboost("trained_models/features.txt");

    // Set up Arduino
    // Arduino arduino;
    // if (!arduino.init()) {
    //     cout << "Arduino not found" << endl;
    //     exit(1);
    // }

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
        int response = svm.predict(reduced_features);
        cout << "Emotion: " << response << endl;
        // Set color
        // if (!arduino.set_color(colors[response-1])) {
        //     cout << "Setting color failed" << endl;
        // }
        // Delete picture
//        system(("rm " + webcam_pic_path).c_str());
    } while (!sleep(interval));

    return 0;
}
