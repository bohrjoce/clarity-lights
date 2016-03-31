#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "SVMOneVsAll.h"

using namespace std;


int main() {
  // load trained SVM model
  SVMOneVsAll svm;
  svm.load();
  Adaboost adaboost("res/features.txt");

  // load trained adaboost model
  while (true) {
    system("streamer -f jpeg -o pic.jpeg");

  }
}
