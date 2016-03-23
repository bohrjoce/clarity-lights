#ifndef JAFFE_H
#define JAFFE_H

#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <opencv2/opencv.hpp>

using namespace std;

const string jaffeDir = "jaffeimages/jaffe/";

class JaffeImages {
public:
    JaffeImages();
    vector<string> filenames;
    vector<int> labels;
};

#endif