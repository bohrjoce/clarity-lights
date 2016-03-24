#include "KDEFValidation.h"
const string afraid = "AF";
const string angry = "AN";
const string disgusted = "DI";
const string happy = "HA";
const string neutral = "NE";
const string sad = "SA";
const string surprised = "SU";

KDEFValidation::KDEFValidation(){
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
      sample newSample;
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
        newSample.emotion = 0;
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
