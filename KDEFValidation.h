#include <string>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>



using namespace std;

namespace fs = boost::filesystem;


//emotion mapping:
//  0-7 (i.e. 
//  0=neutral,
//  1=anger,
//  2=contempt,
//  3=disgust,
//  4=fear,
//  5=happy,
//  6=sadness,
//  7=surprise).

const string kdef_dir = "KDEF/";


struct sample{
  string filepath;
  int emotion;
};


struct KDEFValidation{
  KDEFValidation();
  void init();
  void print_samples();
  vector <sample> samples; 

};




