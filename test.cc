#include "preprocess.h"

using namespace std;
using namespace cv;

// Main
int main(int argc, char** argv ) {

    Mat face;
    if (!preprocess(argv[1], face)) {
        imshow("Face Detected", face);
        waitKey(0);
    }

    return 0;
}