#include "JaffeData.h"

using namespace std;

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
                } else if (emotion == "DI") {
                    e = 3;
                } else if (emotion == "FE") {
                    e = 4;
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
    return 0;
}
