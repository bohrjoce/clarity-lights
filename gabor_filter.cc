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
#include <set>
using namespace cv;
//! [namespace]
static const Size finalSize(448, 448);

using namespace std;
Mat ImageToFV(Mat inputImage, float stddev, int filtSize,double spacial_aspect,  bool visualize, set<int> *weak_learners_indices)
{
  Mat imageOutput;
  Mat rFilter;
  Mat iFilter;
  //we go in 1/2 octave steps, 8 different phases;
  //phase is in radians
  Mat face = inputImage.clone(); 
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
 // float wavelength[] = {4.0,5.656,8.0,11.3137,16.0 };
  float wavelength[] = {2.0,2.828,4.0,5.0,6.5};
  
  //cout << "stddev is "<< stddev << endl;
  for (int i =0; i < 5; i++){
    Mat col;
    for (int j = 0; j < 8; j++){
      rFilter = getGaborFilter(
          Size(filtSize,filtSize), //size of the filter
          stddev, //standard deviation of the envelope
          pi*j/8.0, //theta value, orientation of gaussian
          wavelength[i], //wavelength of sinusiod, determines freq center of guassian
          spacial_aspect, //"spacial aspect" which is just scaling of y dimension of guassian
          0, //phase offset of sinusoids, who cares
          CV_32F, //filter type, either CV_32F or CV_64F
          true //real or imaginary portion of filter
          );
      iFilter = getGaborFilter(
          Size(filtSize,filtSize), //size of the filter
          stddev, //standard deviation of the envelope
          pi*j/8.0, //theta value, orientation of gaussian
          wavelength[i], //wavelength of sinusiod, determines freq center of guassian
          spacial_aspect, //"spacial aspect" which is just scaling of y dimension of guassian
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
      transpose(filterOutput,filterOutput); 
      col.push_back(filterOutput);

      //filterOutput = filterOutput/max;
      //imageOutput = filterOutput.clone()/max;
      //resize(imageOutput, filterOutput, finalSize);
      //imshow( "imOutput",filterOutput);
      //imshow( "Filter", filter);
      //waitKey(0); // Wait for a keystroke


     //REMOVE TODO 
     
    }
    Mat row;
    transpose(col,row);
    imageOutput.push_back(row); 
  }

  Mat image_output; 
  normalize(imageOutput, imageOutput, 0.0,1.0,  NORM_MINMAX, CV_32F);
    
  imageOutput.convertTo(image_output,CV_8U, 255.0);

  //      resize(image_output, image_output, finalSize);
  if(visualize){
    imshow( "gabor_bank",image_output);
    waitKey(0); // Wait for a keystroke
  } 
  int oldNumRows = featureVector.rows; 

  featureVector = featureVector.reshape(1,1);
  normalize(featureVector, featureVector, 0.0,1.0,  NORM_MINMAX, CV_32F);

  if(visualize && weak_learners_indices!= NULL  ){
    Mat visualVector;
    featureVector.convertTo(visualVector,CV_8U,255.0);
    cvtColor(visualVector,visualVector,CV_GRAY2RGB); 
   for (set<int>::iterator it = weak_learners_indices->begin();
        it != weak_learners_indices->end(); it++) {
      visualVector.at<Vec3b>(0,(*it))[0] = 10;
      visualVector.at<Vec3b>(0,(*it))[1] = 10;
      visualVector.at<Vec3b>(0,(*it))[2] = 255;
    }
    visualVector = visualVector.reshape(0,oldNumRows);
    Mat visualCol; 
    Mat visualDisplay; 
    int numIncrements = 0;
    for(int i =0;i< oldNumRows; i+=48){
      Mat image = visualVector(Range(i,i+47), Range::all());
      transpose(image,image);   
      if(numIncrements && !(numIncrements%8)){
        transpose(visualCol,visualCol);
        visualDisplay.push_back(visualCol);
        visualCol.release(); 
      }
      visualCol.push_back(image);
      ++numIncrements;
    }

    transpose(visualCol,visualCol);
    visualDisplay.push_back(visualCol);
    imshow("visualization", visualDisplay);

    waitKey(0);
  }
  return featureVector;
}
