//! [includes]
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "gabor_filter.h"
#include "gabor_impl.h"

using namespace cv;
//! [namespace]
static const Size finalSize(448, 448);

using namespace std;
Mat ImageToFV(Mat inputImage, float stddev, int filtSize)
{
  Mat rFilter;
  Mat iFilter;
  //we go in 1/2 octave steps, 8 different phases;
  //phase is in radians
  Mat filterOutput;
  Mat imOutput;
  Mat featureVector;
  Mat image;
  Mat realRes;
  Mat imaginaryRes;
  inputImage.convertTo(image,CV_32F, 1.0/255.0);
  //imshow("image", image);
  //waitKey(0);
  //namedWindow( "Filter", WINDOW_AUTOSIZE ); // Create a window for display.
  // namedWindow( "Input", WINDOW_AUTOSIZE ); // Create a window for display.
  //namedWindow( "imOutput", WINDOW_AUTOSIZE ); // Create a window for display.
  //five wavelengths, at a half-octave increase from eachother
  //a half-octave is sqrt(2) increase
  float wavelength[] = {2.0,2.82,4.0,5.6568,8.0};
  //cout << "stddev is "<< stddev << endl;
  for (int i =0; i < 5; i++){

    for (int j = 0; j < 8; j++){
      rFilter = getGaborFilter(
          Size(filtSize,filtSize), //size of the filter
          stddev, //standard deviation of the envelope
          pi*j/8.0, //theta value, orientation of gaussian
          wavelength[i], //wavelength of sinusiod, determines freq center of guassian
          .5, //"spacial aspect" which is just scaling of y dimension of guassian
          0, //phase offset of sinusoids, who cares
          CV_32F, //filter type, either CV_32F or CV_64F
          true //real or imaginary portion of filter 
          );
      iFilter = getGaborFilter(
          Size(filtSize,filtSize), //size of the filter
          stddev, //standard deviation of the envelope
          pi*j/8.0, //theta value, orientation of gaussian
          wavelength[i], //wavelength of sinusiod, determines freq center of guassian
          .5, //"spacial aspect" which is just scaling of y dimension of guassian
          0, //phase offset of sinusoids, who cares
          CV_32F, //filter type, either CV_32F or CV_64F
          false //real or imaginary portion of filter 
          );
      filter2D(
          image, //input image
          realRes, //output of filter
          CV_32F, //desired depth of output
          rFilter  //filter to be convolved with input
          );
      filter2D(
          image, //input image
          imaginaryRes, //output of filter
          CV_32F, //desired depth of output
          iFilter  //filter to be convolved with input
          );
      
      cv::pow(realRes,2.0, realRes);
      cv::pow(imaginaryRes,2.0, imaginaryRes);
      cv::add(realRes, imaginaryRes, filterOutput); 
      cv::sqrt(filterOutput,filterOutput); 
     
      //         cout << " wavelength is " << wavelength << "\nphase is " << pi*j/8.0 <<endl;
      //cout << " max pixel value is " << max << endl;


      //cout << "normalized max pixel value is " << max << endl;
      featureVector.push_back(filterOutput);


      //filterOutput = filterOutput/max; 
      //imageOutput = filterOutput.clone()/max;
      //resize(imageOutput, filterOutput, finalSize);
      //imshow( "imOutput",filterOutput);
      //imshow( "Filter", filter);
      //waitKey(0); // Wait for a keystroke

    }
  }
  /* double min, max;
     minMaxLoc(filterOutput, &min, &max);
     cout << "max is " << max << endl;
     cout << "min is " << min << endl;

     featureVector = featureVector/max;
     */
  featureVector = featureVector.reshape(1,1);
  normalize(featureVector, featureVector, 0.0,1.0,  NORM_MINMAX, CV_32F);

  return featureVector;
}
