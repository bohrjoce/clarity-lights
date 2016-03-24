#include "gabor_impl.h"
/*
   Gabor filters and such. To be greatly extended to have full texture analysis.
   For the formulas and the explanation of the parameters see:
http://en.wikipedia.org/wiki/Gabor_filter
*/
using namespace cv;

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;
cv::Mat getGaborFilter( Size ksize, double sigma, double theta,
    double lambd, double gamma, double psi, int ktype,bool realFilter)
{
  double sigma_x = sigma;
  double sigma_y = sigma/gamma;
  int nstds = 3;
  int xmin, xmax, ymin, ymax;
  double c = cos(theta), s = sin(theta);

  if( ksize.width > 0 )
    xmax = ksize.width/2;
  else
    xmax = cvRound(std::max(fabs(nstds*sigma_x*c), fabs(nstds*sigma_y*s)));

  if( ksize.height > 0 )
    ymax = ksize.height/2;
  else
    ymax = cvRound(std::max(fabs(nstds*sigma_x*s), fabs(nstds*sigma_y*c)));

  xmin = -xmax;
  ymin = -ymax;

  CV_Assert( ktype == CV_32F || ktype == CV_64F );

  Mat kernel(ymax - ymin + 1, xmax - xmin + 1, ktype);
  double scale = 1;
  double ex = -0.5/(sigma_x*sigma_x);
  double ey = -0.5/(sigma_y*sigma_y);
  double cscale = CV_PI*2/lambd;

  for( int y = ymin; y <= ymax; y++ )
    for( int x = xmin; x <= xmax; x++ )
    {
      double xr = x*c + y*s;
      double yr = -x*s + y*c;
      //real part
      double real = scale*std::exp(ex*xr*xr + ey*yr*yr)*cos(cscale*xr + psi);
      //imaginary part 
      double imag = scale*std::exp(ex*xr*xr + ey*yr*yr)*sin(cscale*xr + psi);
      //double mag = sqrt((real*real + imag*imag));  
      double filtType;
      if(realFilter){
        filtType = real;
      }
      else
      {
        filtType = imag;
      }
      if( ktype == CV_32F )
        kernel.at<float>(ymax - y, xmax - x) = (float)filtType;
      else
        kernel.at<double>(ymax - y, xmax - x) = filtType;
    }

  return kernel;
}



