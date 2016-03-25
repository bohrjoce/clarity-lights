#ifndef CKTRAINDATA_H
#define CKTRAINDATA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "data.h"
#include "preprocess.h"
#include "gabor_filter.h"

using namespace std;
using namespace cv;

namespace fs = boost::filesystem;

const string ck_dir = "cohn-kanade/";

/* Create 3D array of filenames, std::string. files[i][j][k] refers to image
   file associated with speaker i, sequence j, image k

   Create 3D array of emotion labels. label[i][j][k] refers to emotion label
   associated with speaker i, sequence j, image k

   Exists in separate directories, same structure, iterate the same way.
*/

//enum Emotion { HAPPY, SAD, ANGRY, FRUSTRATED };

class CKTrainData {
  private:
    vector< vector< vector< string >>> filenames;
    vector< vector< int >> labels;
    vector<Data> people_data;
    void iterate_ck_images();
    void iterate_ck_labels();
    void check_files();

  public:
    CKTrainData(bool use_gabor, float gabor_stddev);
    void print_filenames();
    void print_labels();
    string get_filename(int speaker, int sequence, int image);
    vector<Data> get_people_data();
    Data get_flat_data();
};

#endif
