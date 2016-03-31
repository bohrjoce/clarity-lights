#include "KDEFValidation.h"

using namespace std;
using namespace cv;
using namespace ml;

const string afraid = "AF";
const string angry = "AN";
const string disgusted = "DI";
const string happy = "HA";
const string neutral = "NE";
const string sad = "SA";
const string surprised = "SU";

KDEFValidation::KDEFValidation() {
  fs::path target_dir(kdef_dir);
  fs::directory_iterator it0(target_dir), eod0;
  vector<fs::path> v0;
  copy(it0, eod0, back_inserter(v0));
  sort(v0.begin(), v0.end());
  for (auto sequence_it = v0.begin(); sequence_it != v0.end(); ++sequence_it) {
    if (fs::is_regular_file(*sequence_it)) continue;
    fs::directory_iterator it1(*sequence_it), eod1;
    vector<fs::path> v1;
    copy(it1, eod1, back_inserter(v1));
    sort(v1.begin(), v1.end());
    for (auto person_it = v1.begin(); person_it != v1.end(); ++person_it) {
      string imagePath = (*person_it).string();
      string extension = imagePath.substr(imagePath.size()-4, 4);
      if (extension != ".JPG") continue;
      //kdef jpgs take the following format:
      //letter1: session
      //letter2: gender
      //letter3&4:identity number
      //letter5&6:expression
      //letter7&8:angle
      string fileName = imagePath.substr(imagePath.find_last_of("/")+1);
      //check to see if we are a straight image
      char angle = fileName[6];
      if(angle != 'S') continue;
      Sample newSample;
      newSample.filepath = imagePath;
      string emotion = fileName.substr(4,2);
      if(emotion==afraid)
        newSample.emotion = 4;
      else if(emotion==angry)
        newSample.emotion = 1;
      else if(emotion==happy)
        newSample.emotion = 5;
      else if(emotion==disgusted)
        newSample.emotion = 3;
      else if(emotion==surprised)
        newSample.emotion = 7;
      else if(emotion==neutral)
        newSample.emotion = 2;
      else if(emotion==sad)
        newSample.emotion = 6;
      else{
        cout << " unexpected file format in KDEF dir" << endl;
        assert(1);
      }
      samples.push_back(newSample);
    }
  }
}

void KDEFValidation::print_samples(){
  for(unsigned int i = 0; i < samples.size(); ++i){
    cout << "\n~~~~~sample: " << i << "~~~~~\n";
    cout << "path: " << samples[i].filepath << "\n";
    cout << "emotion: " << samples[i].emotion << "\n";
  }
  cout << endl;
}

int main(int argc, char *argv[]) {
  KDEFValidation kd;
  float stddev = 2.0;
  double spacial_aspect = 2.0;
  unsigned int weak_learners = 92;
  double C = 10.0;
  cout << "stddev = " << stddev << endl;
  cout << "spacial_aspect = " << spacial_aspect << endl;
  cout << "C = " << C << endl;
  cout << "weak_learners = " << weak_learners << endl;
  CKTrainData ckdata(true, stddev, spacial_aspect);
  Data train = ckdata.get_flat_data();

  Data adaboost_data = ckdata.get_flat_data();
  Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t, weak_learners, false);
  Mat reduced_train_x = adaboost.reduce_features(train.x);

  // init svm
  SVMOneVsAll svm(C);
  svm.train(reduced_train_x, train.t);

  ConfusionMatrix confusion_matrix;

  Mat image, test_x, reduced_test_x;
  Mat testKDF(0, 0, CV_32F);
  Mat testKDL(0, 0, CV_32SC1);

  for (unsigned int i = 0; i < kd.samples.size(); ++i){
    test_x.release();
    reduced_test_x.release();
    if (preprocess(kd.samples[i].filepath, image) != 0) {
      cout << "preprocess failed: " << kd.samples[i].filepath << endl;
      exit(1);
    }
    test_x = ImageToFV(image, stddev);
    reduced_test_x = adaboost.reduce_features(test_x);
    int response = svm.predict(reduced_test_x);
    int truth = kd.samples[i].emotion;
//    cout << response << " vs " << truth << endl;
    confusion_matrix.update(response, truth);
  }
  confusion_matrix.print();

  return 0;
}
