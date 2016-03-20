#include "CKTrainData.h"

void CKTrainData::iterate_ck_images() {

  unsigned int i = 0, j = 0;
  fs::path target_dir(ck_dir + "cohn-kanade-images/");
  fs::directory_iterator it0(target_dir), eod0;

  // cohn-kanade/cohn-kanade-images/SXXX
  BOOST_FOREACH(fs::path const &person_dir, std::make_pair(it0, eod0)) {

    if (fs::is_regular_file(person_dir)) continue;
    fs::directory_iterator it1(person_dir), eod1;
    filenames.push_back(vector<vector<string>>());

    j = 0;
    // cohn-kanade/cohn-kanade-images/SXXX/YYY
    BOOST_FOREACH(fs::path const &sequence_dir, std::make_pair(it1, eod1)) {

      if (fs::is_regular_file(sequence_dir)) continue;
      fs::directory_iterator it2(sequence_dir), eod2;
      filenames[i].push_back(vector<string>());

      // cohn-kanade/cohn-kanade-images/SXXX/YYY/abc.jpg
      BOOST_FOREACH(fs::path const &filename, std::make_pair(it2, eod2)) {
        string extension = filename.string().substr(filename.string().size()-4, 4);
        if (extension != ".png") continue;
        filenames[i][j].push_back(filename.string());
      }
      ++j;
    }
    ++i;
  }
}

void CKTrainData::iterate_ck_labels() {

  unsigned int i = 0, j = 0;
  fs::path target_dir(ck_dir + "Emotion/");
  fs::directory_iterator it0(target_dir), eod0;

  // cohn-kanade/cohn-kanade-images/SXXX
  BOOST_FOREACH(fs::path const &person_dir, std::make_pair(it0, eod0)) {

    if (fs::is_regular_file(person_dir)) continue;
    fs::directory_iterator it1(person_dir), eod1;
    labels.push_back(vector<int>());

    j = 0;
    // cohn-kanade/cohn-kanade-images/SXXX/YYY
    BOOST_FOREACH(fs::path const &sequence_dir, std::make_pair(it1, eod1)) {

      if (fs::is_regular_file(sequence_dir)) continue;
      fs::directory_iterator it2(sequence_dir), eod2;
      labels[i].push_back(-1);

      // cohn-kanade/cohn-kanade-images/SXXX/YYY/label.txt
      BOOST_FOREACH(fs::path const &filename, std::make_pair(it2, eod2)) {
        string extension = filename.string().substr(filename.string().size()-4, 4);
        if (extension != ".txt") continue;
        ifstream ifn;
        int value;
        ifn.open(filename.string());
        ifn >> value;
        labels[i][j] = value;
      }
      ++j;
    }
    ++i;
  }
}

void CKTrainData::init() {
  iterate_ck_images();
  iterate_ck_labels();
}

void CKTrainData::print_filenames() {
  for (unsigned int i = 0; i < filenames.size(); ++i) {
    cout << "------------------------- person " << i << " -------------------------\n\n";
    for (unsigned int j = 0; j < filenames[i].size(); ++j) {
      cout << "~~~~~~~~~~~~~~~~~~~~~~~~~ seq " << j << " ~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
      for (unsigned int k = 0; k < filenames[i][j].size(); ++k) {
        cout << filenames[i][j][k] << endl;
      }
    }
  }
}

void CKTrainData::print_labels() {
  for (unsigned int i = 0; i < labels.size(); ++i) {
    cout << "------------------------- person " << i << " -------------------------\n\n";
    for (unsigned int j = 0; j < labels[i].size(); ++j) {
      cout << "~~~~~~~~~~~~~~~~~~~~~~~~~ seq " << j << " ~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
      cout << labels[i][j] << endl;
    }
  }
}
