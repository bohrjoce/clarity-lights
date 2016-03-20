//! [includes]
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "gabor_filter.h"


using namespace cv;
//! [namespace]

using namespace std;
Mat ImageToFV(Mat inputImage, float stddev, int filtSize)
{
    Mat filter;
    //we go in 1/2 octave steps, 8 different phases;
    //phase is in radians
    Mat filterOutput;
    Mat imOutput;
    Mat featureVector;
    Mat image;
    inputImage.convertTo(image,CV_32F, 1.0/255.0);
    float wavelength = 2.0;
    //namedWindow( "Filter", WINDOW_AUTOSIZE ); // Create a window for display.
    // namedWindow( "Input", WINDOW_AUTOSIZE ); // Create a window for display.
    //namedWindow( "imOutput", WINDOW_AUTOSIZE ); // Create a window for display.

    //cout << "stddev is "<< stddev << endl;
    for (int i =1; i < 6; i++){

        for (int j = 0; j < 8; j++){
            filter = getGaborKernel(
                    Size(filtSize,filtSize), //size of the filter
                    stddev, //standard deviation of the envelope
                    pi*j/8.0, //theta value, orientation of gaussian
                    wavelength, //wavelength of sinusiod, determines freq center of guassian
                    1, //"spacial aspect" which is just scaling of y dimension of guassian
                    0 //phase offset of sinusoids, who cares
                    // ,CV_32F //filter type, either CV_32F or CV_64F
                    );
            filter2D(
                    image, //input image
                    filterOutput, //output of filter
                    CV_32F, //desired depth of output
                    filter  //filter to be convolved with input
                    );
            //         cout << " wavelength is " << wavelength << "\nphase is " << pi*j/8.0 <<endl;
            featureVector.push_back(filterOutput);
            //filterOutput.convertTo(imOutput, CV_8U,1.0/255.0);
            //imshow( "imOutput", imOutput);
            //imshow( "Filter", filter);
            //     waitKey(0); // Wait for a keystroke

        }
        //halfoctave increment
        wavelength = 1.5*(1<<i);
    }
    featureVector = featureVector.reshape(1,1);
    return featureVector;
}
