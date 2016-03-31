#ifndef JAFFE_H
#define JAFFE_H

#include <string>
#include <vector>
#include "ConfusionMatrix.h"
#include "SVMOneVsAll.h"

const std::string jaffeDir = "jaffeimages/jaffe/";

class JaffeImages {
public:
    JaffeImages();
    std::vector<std::string> filenames;
    std::vector<int> labels;
};

#endif
