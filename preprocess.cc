#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Global vars, params
const string outputPath = "preprocess-output/preprocessed_image.jpg";
const string face_cascade_name = "res/haarcascade_frontalface_alt.xml";
const Size finalSize(100, 100); // Todo: tune this param

// Main
int main(int argc, char** argv ) {

    // Command line args
    if (argc < 2)
    {
        printf("Did not specify image\n");
        return -1;
    }

    // Vars
    Mat image, imageGray, face;
    vector<Rect> faces;
    CascadeClassifier faceCascade;
    Rect faceBoundary;

    // Load cascade
    if (!faceCascade.load(face_cascade_name)){
        printf("Error loading cascade\n");
        return -1; 
    }

    // Read in image
    image = imread(argv[1], 1);
    if (!image.data) {
        printf("Image not found\n");
        return -1;
    }
    
    // Convert to black and white
    cvtColor(image, imageGray, COLOR_BGR2GRAY);
    equalizeHist(imageGray, imageGray);

    // Detect faces
    faceCascade.detectMultiScale(imageGray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(48, 48));
    if (!faces.size()) {
        printf("No face detected\n");
        return 0;
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

    // Crop, resize, and write image to disk
    face = imageGray(faceBoundary);
    resize(face, face, finalSize);
    imwrite(outputPath, face);

    // Display detected face
    imshow("Face Detected", face);
    waitKey(0);

    return 0;
}
