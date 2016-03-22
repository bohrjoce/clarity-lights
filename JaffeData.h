#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::filesystem;

const string jaffeDir = "jaffeimages/jaffe/";

class JaffeImages {
public:
    JaffeImages();
    vector<string> filenames;
    vector<int> labels;
};
