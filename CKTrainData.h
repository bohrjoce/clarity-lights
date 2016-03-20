#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

using namespace std;

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
    void iterate_ck_images();
    void iterate_ck_labels();

  public:
    void init();
    void print_filenames();
    void print_labels();
    vector< vector< vector< string >>> filenames;
    vector< vector< int >> labels;
    string get_filename(int speaker, int sequence, int image);
    int get_label(int speaker, int sequence, int image);
};
