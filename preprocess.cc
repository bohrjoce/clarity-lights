#include "preprocess.h"

using namespace std;
using namespace cv;

// comment this to use normal preprocess
// uncomment to use tan triggs preprocess
#define USE_TAN_TRIGGS

// Consts
static const string face_cascade_name = "res/haarcascade_frontalface_alt.xml";
static const Size finalSize(48, 48);

Mat norm_0_255(const Mat& src) {
    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
    case 1:
        cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}

Mat tan_triggs_preprocessing(InputArray src,
        float alpha = 0.1, float tau = 10.0, float gamma = 0.2, int sigma0 = 1,
        int sigma1 = 2) {

    // Convert to floating point:
    Mat X = src.getMat();
    X.convertTo(X, CV_32FC1);
    // Start preprocessing:
    Mat I;
    pow(X, gamma, I);
    // Calculate the DOG Image:
    {
        Mat gaussian0, gaussian1;
        // Kernel Size:
        int kernel_sz0 = (3*sigma0);
        int kernel_sz1 = (3*sigma1);
        // Make them odd for OpenCV:
        kernel_sz0 += ((kernel_sz0 % 2) == 0) ? 1 : 0;
        kernel_sz1 += ((kernel_sz1 % 2) == 0) ? 1 : 0;
        GaussianBlur(I, gaussian0, Size(kernel_sz0,kernel_sz0), sigma0, sigma0, BORDER_REPLICATE);
        GaussianBlur(I, gaussian1, Size(kernel_sz1,kernel_sz1), sigma1, sigma1, BORDER_REPLICATE);
        subtract(gaussian0, gaussian1, I);
    }

    {
        double meanI = 0.0;
        {
            Mat tmp;
            pow(abs(I), alpha, tmp);
            meanI = mean(tmp).val[0];

        }
        I = I / pow(meanI, 1.0/alpha);
    }

    {
        double meanI = 0.0;
        {
            Mat tmp;
            pow(min(abs(I), tau), alpha, tmp);
            meanI = mean(tmp).val[0];
        }
        I = I / pow(meanI, 1.0/alpha);
    }

    // Squash into the tanh:
    {
        Mat exp_x, exp_negx;
	exp( I / tau, exp_x );
	exp( -I / tau, exp_negx );
	divide( exp_x - exp_negx, exp_x + exp_negx, I );
        I = tau * I;
    }
    return I;
}

int preprocess(string imagePath, Mat &output) {
    // Vars
    Mat image, imageGray, face;
    vector<Rect> faces;
    CascadeClassifier faceCascade;
    Rect faceBoundary;

    // Load cascade
    if (!faceCascade.load(face_cascade_name)){
        printf("Error: unable to load cascade\n");
        exit(1); // Todo: better error handling
    }

    // Read in image
#ifdef USE_TAN_TRIGGS
    imageGray = imread(imagePath, CV_LOAD_IMAGE_GRAYSCALE);
#endif
#ifndef USE_TAN_TRIGGS
    image = imread(imagePath, 1);
    if (!image.data) {
        printf("Image not found\n");
        return -1;
    }

    // Convert to grayscale
    cvtColor(image, imageGray, COLOR_BGR2GRAY);
    equalizeHist(imageGray, imageGray);
#endif

    // Detect faces
    faceCascade.detectMultiScale(imageGray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(48, 48));
    if (!faces.size()) {
        printf("No face detected\n");
        return -1;
    }
    int largestArea = 0, area;
    for(size_t i = 0; i < faces.size(); i++) {
        Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
        // Determine largest face; Todo: explore other methods
        area = faces[i].width * faces[i].height;
        if (area > largestArea) {
            faceBoundary = faces[i];
            largestArea = area;
        }
    }

    // Crop, resize, normalize, and return face
    face = imageGray(faceBoundary);
    resize(face, face, finalSize);
#ifndef USE_TAN_TRIGGS
    normalize(face, face, 0, 255, NORM_MINMAX, CV_8U);
    output = face;
#endif
#ifdef USE_TAN_TRIGGS
    Mat preprocessed = tan_triggs_preprocessing(face);
    output = norm_0_255(preprocessed);
#endif
    return 0;
}
