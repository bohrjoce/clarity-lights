#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include "preprocess.h"
#include "gabor_filter.h"
#include "SVMOneVsAll.h"
#include "arduino.h"
#include "Trees.h"

//#define TESTING

using namespace std;

const string webcam_pic_path = "res/webacam_pic.jpeg";

int main() {
    // Set RGB vals for each emotion
    vector<string> emotion_labels = {"anger", "neutral", "disgust", "fear", "happy", "sadness", "surprise"};
    int interval = 60; //seconds
    char response;
    cout << "\n\n\n                 THANK YOU FOR USING CLARITY LIGHTS \n\n\n";
    cout << "Clarity Lights will take a photo using your webcam every 1 minute. This photo will \n";
    cout << "be used to determine your emotion, and the light color of the box will be adjusted \n";
    cout << "according to your preferences. The photo is deleted after use, and only exists on your\n";
    cout << "filesystem. Use Ctrl+C to exit Clarity Lights.\n\n\n";
    cout << "Do you give Clarity Lights permission to access your webcam and take photos as described? [y/n]: ";
    cin >> response;
    if (response != 'y') {
      cout << "\nGoodbye.\n";
      exit(0);
    }

#ifdef TESTING
    string name;
    cout << "\n\nType your name here: ";
    cin >> name;
#endif

    vector<Color> colors;
    colors.push_back(Color(255, 0, 0));     // anger
    colors.push_back(Color(255, 255, 255)); // neutral
    colors.push_back(Color(100, 255, 0));     // disgust
    colors.push_back(Color(0, 255, 150));   // fear
    colors.push_back(Color(255, 255, 0));   // happy
    colors.push_back(Color(0, 0, 255));   // sadness
    colors.push_back(Color(255, 0, 255));  // surprise

    cout << "\n\n\nDefault color mapping\n\n\n";
    cout << "Emotion      R    G    B          color\n";
    cout << "---------------------------------------\n";
    cout << "Anger:     255    0    0            red\n";
    cout << "Neutral:   255  255  255          white\n";
    cout << "Disgust:   100  255    0   yellow-green\n";
    cout << "Fear:        0  255  150           teal\n";
    cout << "Happy:     255  255    0         yellow\n";
    cout << "Sadness:     0    0  255           blue\n";
    cout << "Surprise:  255    0  255        magenta\n\n\n";
    cout << "Is the default color mapping acceptable? [y/n]: ";
    cin >> response;
    if (response == 'n') {
      colors.clear();
      unsigned int r, g, b;
      cout << "\n\nRGB values for:" << endl;
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
    }
    response = 'n';
#ifndef TESTING
    cout << "\n\nRun in demo mode (take pictures with keypress)? [y/n]: ";
    cin >> response;
#endif
    bool demo = false;
    if (response == 'y') {
      demo = true;
    } else {
      cout << "\n\nDo you want to change the photo catpure interval? [y/n]: ";
      cin >> response;
      if (response == 'y') {
        interval = 0;
        while (interval < 5 or interval > 1200) {
          cout << "Capture interval (minutes, between 5-1200): ";
          cin >> interval;
        }
      }
    }


    // Load trained SVM model
    // all-svm/all-features.txt for normal preprocessed model
    // tantrigg-svm/tantrigg-features.txt for tantrigg preprocess
    SVMOneVsAll svm;
    svm.load("trained_models/tantrigg-svm");
/*    Trees tree;
    tree.load("trained_models/preprocess-tree");*/
    Adaboost adaboost("trained_models/tantrigg-features.txt");

/*    // Set up Arduino
    Arduino arduino;
    if (!arduino.init()) {
        cout << "Arduino not found" << endl;
        exit(1);
    }*/
#ifdef TESTING
    ofstream emotionlog;
    string emotionlog_filename = "log/" + name + "-emotion.log";
    emotionlog.open(emotionlog_filename.c_str());
#endif

    do {
        if (demo) {
          cin.get();
        } else {
          sleep(interval);
        }
        // Take picture
        system(("streamer -f jpeg -o " + webcam_pic_path + " > log/clarity_lights.log 2>&1").c_str());
        // Feature extraction
        Mat webcam_pic;
        if (preprocess(webcam_pic_path, webcam_pic) != 0) {
          cout << "Preprocess failed" << endl;
          continue;
        }
        // Classify emotion
//        Mat gabor_features = ImageToFV(webcam_pic, 2.0, 31, 2.0, true, &(adaboost.weak_learners_indices));
        Mat gabor_features = ImageToFV(webcam_pic);
        Mat reduced_features = adaboost.reduce_features(gabor_features);
        int response = svm.predict(reduced_features);
/* // uncomment this to use decision tree
        Mat svm_features = svm.create_svm_features(reduced_features);
        int response = tree.predict(svm_features);*/
        vector<double> prediction = svm.raw_predict(reduced_features);
        for (unsigned int i = 0; i < prediction.size(); ++i) {
          cout << prediction[i] << " ";
        }
        cout << endl;
        // Set color
/*        if (!arduino.set_color(colors[response-1])) {
            cout << "Setting color failed" << endl;
        }*/
#ifdef TESTING
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        string time_str(asctime(timeinfo));
        time_str = time_str.substr(11, 8);
        emotionlog << time_str << "  " << emotion_labels[response-1] << endl;
#endif
#ifndef TESTING
        cout << emotion_labels[response-1] << endl;
#endif
    } while (true);

    return 0;
}
