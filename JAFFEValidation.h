#ifndef JAFFE_H
#define JAFFE_H

#include <string>
#include <vector>
#include "ConfusionMatrix.h"
#include "SVMOneVsAll.h"
#include "data.h"

const std::string jaffeDir = "jaffeimages/jaffe/";

class JaffeImages {
private:
    std::vector<Data> people_data;
public:
    JaffeImages();
    std::vector<std::string> filenames;
    std::vector<int> labels;
    std::vector<Data> get_people_data() { return people_data; }
};

#endif
