#include "ConfusionMatrix.h"

using namespace std;

ConfusionMatrix::ConfusionMatrix() :
    correct(0),
    total(0),
    emote_total(vector<int>(NUM_EMOTIONS, 0)),
    emote_correct(vector<int>(NUM_EMOTIONS, 0)),
    confusion(vector<vector<int>>(NUM_EMOTIONS, vector<int>())) {
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    confusion[i] = vector<int>(NUM_EMOTIONS, 0);
  }
}

void ConfusionMatrix::update(int response, int truth) {
  if (response == truth) {
    ++correct;
  }
  ++emote_total[truth-1];
  ++confusion[truth-1][response-1];
  ++total;
}

void ConfusionMatrix::print() {
  double accuracy_sum = 0;
  cout << endl;
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    double accuracy = (double)confusion[i][i]/(double)emote_total[i];
    accuracy_sum += accuracy;
    cout << "emotion " << i+1 << " accuracy : " << confusion[i][i] << "/" <<
        emote_total[i] << " = " << accuracy << endl;
  }
  cout << endl;
  cout << "accuracy : " << (double)correct/(double)total << endl;
  cout << "UAR : " << accuracy_sum/(double)NUM_EMOTIONS << endl << endl;
  cout << "Confusion matrix: left actual, top predicted" << endl;
  cout << "1    2    3    4    5    6    7\n\n";
  for (unsigned int i = 0; i < NUM_EMOTIONS; ++i) {
    cout << i+1 << "  ";
    for (unsigned int j = 0; j < NUM_EMOTIONS; ++j) {
      cout << confusion[i][j] << " ";
    }
    cout << endl;
  }
}

