#include "preprocess.h"

using namespace std;
using namespace cv;

// Consts
static const string outputPath = "preprocess-output/preprocessed_image.jpg";
static const string face_cascade_name = "res/haarcascade_frontalface_alt.xml";
static const Size finalSize(48, 48);

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
    image = imread(imagePath, 1);
    if (!image.data) {
        printf("Error: image not found\n");
        exit(1);
    }

    // Convert to grayscale
    cvtColor(image, imageGray, COLOR_BGR2GRAY);
    equalizeHist(imageGray, imageGray);

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

    // Crop, resize, and return face
    face = imageGray(faceBoundary);
    resize(face, face, finalSize);
    output = face;
    return 0;
}
