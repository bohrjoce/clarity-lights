#ifndef KDEFVALIDATION_H
#define KDEFVALIDATION_H

#include <string>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "preprocess.h"
#include "gabor_filter.h"
#include "CKTrainData.h"
#include "adaboost.h"
#include "ConfusionMatrix.h"
#include "SVMOneVsAll.h"

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

const std::string kdef_dir = "KDEF/";


struct Sample{
  std::string filepath;
  int emotion;
};


struct KDEFValidation {
  std::vector<Sample> samples;
  KDEFValidation();
  void print_samples();
};

#endif
