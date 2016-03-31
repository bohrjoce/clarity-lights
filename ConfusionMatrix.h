#ifndef CONFUSIONMATRIX_H
#define CONFUSIONMATRIX_H

#include <iostream>
#include <vector>
#include "adaboost.h"

class ConfusionMatrix {
  private:
    int correct;
    int total;
    std::vector<int> emote_total;
    std::vector<int> emote_correct;
    std::vector<std::vector<int>> confusion;

  public:
    ConfusionMatrix();
    void update(int response, int truth);
    void print();
};

#endif
