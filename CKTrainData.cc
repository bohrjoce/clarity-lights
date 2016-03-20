#include "CKTrainData.h"

void CKTrainData::iterate_ck_images() {

  unsigned int i = 0, j = 0;
  fs::path target_dir(ck_dir + "cohn-kanade-images/");

  fs::directory_iterator it0(target_dir), eod0;
  vector<fs::path> v0;
  copy(it0, eod0, back_inserter(v0));
  sort(v0.begin(), v0.end());

  // cohn-kanade/cohn-kanade-images/SXXX
  for (auto person_it = v0.begin(); person_it != v0.end(); ++person_it) {

    if (fs::is_regular_file(*person_it)) continue;
    fs::directory_iterator it1(*person_it), eod1;
    vector<fs::path> v1;
    copy(it1, eod1, back_inserter(v1));
    sort(v1.begin(), v1.end());

    filenames.push_back(vector<vector<string>>());

    j = 0;
    // cohn-kanade/cohn-kanade-images/SXXX/YYY
    for (auto sequence_it = v1.begin(); sequence_it != v1.end(); ++sequence_it) {

      if (fs::is_regular_file(*sequence_it)) continue;
      fs::directory_iterator it2(*sequence_it), eod2;
      vector<fs::path> v2;
      copy(it2, eod2, back_inserter(v2));
      sort(v2.begin(), v2.end());

      filenames[i].push_back(vector<string>());

      // cohn-kanade/cohn-kanade-images/SXXX/YYY/abc.jpg
      for (auto filename_it = v2.begin(); filename_it != v2.end(); ++filename_it) {
        string filename_str = (*filename_it).string();
        string extension = filename_str.substr(filename_str.size()-4, 4);
        if (extension != ".png") continue;
        filenames[i][j].push_back(filename_str);
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
  vector<fs::path> v0;
  copy(it0, eod0, back_inserter(v0));
  sort(v0.begin(), v0.end());

  // cohn-kanade/cohn-kanade-images/SXXX
  for (auto person_it = v0.begin(); person_it != v0.end(); ++person_it) {

    if (fs::is_regular_file(*person_it)) continue;
    fs::directory_iterator it1(*person_it), eod1;
    vector<fs::path> v1;
    copy(it1, eod1, back_inserter(v1));
    sort(v1.begin(), v1.end());

    labels.push_back(vector<int>());

    j = 0;
    // cohn-kanade/cohn-kanade-images/SXXX/YYY
    for (auto sequence_it = v1.begin(); sequence_it != v1.end(); ++sequence_it) {

      if (fs::is_regular_file(*sequence_it)) continue;
      fs::directory_iterator it2(*sequence_it), eod2;
      vector<fs::path> v2;
      copy(it2, eod2, back_inserter(v2));
      sort(v2.begin(), v2.end());

      labels[i].push_back(-1);

      // cohn-kanade/cohn-kanade-images/SXXX/YYY/label.txt
      for (auto filename_it = v2.begin(); filename_it != v2.end(); ++filename_it) {
        string filename_str = (*filename_it).string();
        string extension = filename_str.substr(filename_str.size()-4, 4);
        if (extension != ".txt") continue;
        ifstream ifn;
        int value;
        ifn.open(filename_str);
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
  check_data();
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

void CKTrainData::check_data() {
  if (filenames.size() != labels.size()) {
    cout << "filenames and labels sizes do not match\n";
  }
  for (unsigned int i = 0; i < filenames.size(); ++i) {
    if (filenames[i].size() != labels[i].size()) {
      cout << "filename label " << i << " do not match\n";
      cout << "filename: " << filenames[i].size() << endl;
      cout << "label: " << labels[i].size() << endl;
      cout << filenames[i][0][0] << endl;
    }
  }
}
