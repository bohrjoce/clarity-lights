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
  double C_val = 10.0;
  // kd.print_samples();

  CKTrainData ckdata(true, stddev, spacial_aspect);
  Data train = ckdata.get_flat_data();

  Data adaboost_data = ckdata.get_flat_data();
  unsigned int weak_learners = 92;
  Adaboost adaboost = Adaboost(adaboost_data.x, adaboost_data.t, weak_learners, false);

  Mat reduced_train_x = adaboost.reduce_features(train.x);

  Ptr<SVM> svm = SVM::create();
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);
  svm->setC(C_val);
  svm->train(reduced_train_x, ROW_SAMPLE, train.t);

  Mat image, test_x, reduced_test_x;
  Mat testKDF(0, 0, CV_32F);
  Mat testKDL(0, 0, CV_32SC1);
  int correct = 0, total = 0;
  vector<int> emote_total(NUM_EMOTIONS, 0);
  vector<int> emote_correct(NUM_EMOTIONS, 0);
  vector<vector<int>> confusion(NUM_EMOTIONS, vector<int>());
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    confusion[i] = vector<int>(NUM_EMOTIONS, 0);
  }

  for (unsigned int i = 0; i < kd.samples.size(); ++i){
    test_x.release();
    reduced_test_x.release();
    if (preprocess(kd.samples[i].filepath, image) != 0) {
      cout << "preprocess failed: " << kd.samples[i].filepath << endl;
      exit(1);
    }
    test_x = ImageToFV(image, stddev);
    reduced_test_x = adaboost.reduce_features(test_x);
    int response = svm->predict(reduced_test_x);
    int truth = kd.samples[i].emotion;
    cout << response << " vs " << truth << endl;
    if (response == truth) {
      ++correct;
    }
    ++emote_total[truth-1];
    ++confusion[truth-1][response-1];
    ++total;
  }

  double accuracy_sum = 0;
  double accuracy = (double)correct/(double)total;
  cout << "accuracy : " << accuracy << endl;
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    accuracy = (double)confusion[i][i]/(double)emote_total[i];
    accuracy_sum += accuracy;
    cout << "emotion " << i+1 << " accuracy : " << confusion[i][i] << "/" <<
        emote_total[i] << " = " << accuracy << endl;
  }
  cout << "UAR : " << accuracy_sum/(double)NUM_EMOTIONS << endl;
  cout << "Confusion matrix: left actual, top predicted" << endl;
  cout << "1    2    3    4    5    6    7\n\n";
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    cout << i+1 << "  ";
    for (unsigned int j = 0; j < NUM_EMOTIONS; ++j) {
      cout << confusion[i][j] << " ";
    }
    cout << endl;
  }
  return 0;
}
