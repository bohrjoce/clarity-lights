#ifndef CKTRAINDATA_H
#define CKTRAINDATA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "preprocess.h"
#include "gabor_filter.h"
#include "data.h"

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
    vector< vector < string >> filename_data;
    void iterate_ck_images();
    void iterate_ck_labels();
    void check_files();

  public:
    CKTrainData(bool use_gabor = true, float gabor_stddev = 2.0, double spacial_aspect = 2.0);
    void print_filenames();
    void partition_LOO_data(Data &train, Data &test, unsigned int person);
    void print_labels();
    string get_filename(int speaker, int sequence, int image);
    unsigned int get_num_people();
    vector<vector<string>> get_filename_data();
    Data get_flat_data();
};

#endif
