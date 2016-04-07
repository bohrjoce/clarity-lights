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
    vector<Color> colors;
    colors.push_back(Color(255, 0, 0));     // anger
    colors.push_back(Color(100, 100, 100)); // neutral
    colors.push_back(Color(0, 225, 0));     // disgust
    colors.push_back(Color(200, 0, 245));   // fear
    colors.push_back(Color(255, 255, 0));   // happy
    colors.push_back(Color(0, 190, 255));   // sadness
    colors.push_back(Color(255, 185, 20));  // surprise

    cout << "\n\n\nDefault color mapping\n\n\n";
    cout << "Emotion      R    G    B\n";
    cout << "------------------------\n";
    cout << "Anger:     255    0    0\n";
    cout << "Neutral:   100  100  100\n";
    cout << "Disgust:     0  225    0\n";
    cout << "Fear:      200    0  245\n";
    cout << "Happy:     255  255    0\n";
    cout << "Sadness:     0  190  255\n";
    cout << "Surprise:  255  185   20\n\n\n";
    cout << "Do you want to change the default color mapping? [y/n]: ";
    cin >> response;
    if (response == 'y') {
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
    cout << "\n\nRun in demo mode (take pictures with keypress)? [y/n]: ";
    cin >> response;
    bool demo = false;
    if (response == 'y') {
      demo = true;
    } else {
      cout << "\n\nDo you want to change the photo catpure interval? [y/n]: ";
      cin >> response;
      if (response == 'y') {
        interval = 0;
        while (interval < 1 or interval > 60) {
          cout << "Capture interval (minutes, between 1-60): ";
          cin >> interval;
        }
        interval *= 60;
      }
    }


    // Load trained SVM model
    SVMOneVsAll svm;
    svm.load("trained_models/all-svm");
    Adaboost adaboost("trained_models/all-features.txt");

    // Set up Arduino
    Arduino arduino;
    if (!arduino.init()) {
        cout << "Arduino not found" << endl;
        exit(1);
    }

    do {
        if (demo) {
          cin.get();
        } else {
          sleep(interval);
        }
        // Take picture
        system(("streamer -f jpeg -o " + webcam_pic_path + " > clarity_lights.log 2>&1").c_str());
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
        cout << emotion_labels[response-1] << endl;
        // Set color
        if (!arduino.set_color(colors[response-1])) {
            cout << "Setting color failed" << endl;
        }
    } while (true);

    return 0;
}
