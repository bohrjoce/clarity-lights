#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>

#include <math.h>
using namespace cv;
Mat getGaborFilter( Size ksize, double sigma, double theta,
                            double lambd, double gamma, double psi, int ktype,
                            bool realFilter);
