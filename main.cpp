#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
//#include <array>

#include "commonFunction.h"
#include "EdgeDetection.h"
#include "PanoramaSynthesys.h"
#include "SpaceReduction.h"
#include "commonFunction.h"
#include "realityAugmentation.h"
#include "estimateScale.h"

using namespace cv;
using namespace std;

// Defining the locaction of the images
//String modell = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/test/RealImage/paternoModelBenini.png";
String modell = "";
String query = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/test/RealImage/SyntheticTest_1.png";
String lbl = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/test/RealImage/SyntheticTest_1.png";

// Path where save the results to analyze them with matlab
String modellMag = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/magnitudeModel_AllFilter.jpg";
String phaseMag = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/phaseModel.jpg";
String magPhoto = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/magnitude_AllFilter.jpg";
String phasePhoto = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/phase.jpg";
String scaledPhotoName = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/scaledPhoto.jpg";
String OutputName = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/output.jpg";
String noisedName = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/noisedQery.jpg";
String VCC_name = "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/VCCLinear.jpg";

/* ************************ BY NOW THE EXIF AND SENSOR DATA ARE STATICALLY INSERTED BY HAND ************************* */
float focal = 6.2f; /*77/20.0;*/ /*4.48;*/  // extracted by the EXIF data of the picture (MontePaternoIphone4.jpg)
float sensorWidth =  6.16f;  /*4.54;*/ // Paramater for iPhone4
float sensorHeight = 4.62f; /*3.39;*/ // Paramater for iPhone4
/* ****************************************************************************************************************** */

/* ************************SETTING THE PARAMATER OF THE PROCESSING ************************* */
bool addNoise = false; // adding AWGN
Scalar filterPicture(1,1,1); // EdgeDetection paramter for the query picture
Scalar filterModel(1,1,1);  // EdgeDetection paramter for the synthetic model
bool toScale = false; // if true scale estimation of the picture is executed
const bool doVCC = true; // execute VCC, peak extracxtion and overlay
const bool drawArea = false; // draw a rectancge of the sze of the model on the linear VCC
/* ****************************************************************************************************************** */

// defining the kernel for the edge detection, in this case we use the Prewitt kernel
//const Mat kernel =  (Mat_<double>(3,3) << -1, 0, +1, -2, -0, +2, -1, 0, +1); // Sobel
const Mat kernel =  (Mat_<double>(3,3) << -1, +1, +1, -1, -2, +1, -1, +1, +1); // Prewitt

// Defining the value of the treshold for the edge detection
const int tLow = 150;
const int tHigh = 200;

int main(int argc, char** argv)
{
    //Loading file
    cout << "Loading synthetic panorama...";
    Mat model;
    commonFunction::readFile(&model, modell); // Synthetic panorama
    cout << "   DONE" << endl;
    cout << "Loading picture...";
    Mat photo;
    commonFunction::readFile(&photo, query); // Input image
    cout << "   DONE" << endl;
    cout << "Loading peaks name...";
    Mat label;
    commonFunction::readFile(&label, lbl); // Name of the peaks
    cout << "   DONE" << endl;
    
    // Adding AWGN noise
    if (addNoise)
    {
        Mat noise(photo.rows,photo.cols,CV_8UC3);
        cv::randn(noise, 0, 600);
        cv::add(photo, noise, photo);
        imwrite(noisedName, photo);
    }
    
    // Computing the the correct scale in order to proper compare the photo and the synthetic panorama
    Mat *scaledPhoto = new Mat(); //photo.copyTo(*scaledPhoto);
    estimateScale::setScale(&photo, &model, sensorWidth, sensorHeight, focal, scaledPhoto, toScale);
    
    // calculating the edges with the "Compass Edge Detector"
    Mat *photoMag = new Mat(scaledPhoto->rows,scaledPhoto->cols,scaledPhoto->type(),Scalar(0,0,0));
    Mat *photoPhase = new Mat(scaledPhoto->rows,scaledPhoto->cols,scaledPhoto->type(),Scalar(0,0,0));
    cout << "Computing photo edge...";
    EdgeDetection::detect(*scaledPhoto, kernel, tLow, tHigh, photoMag, photoPhase,filterPicture); // Edge of the photo
    imshow("Photo edge magnitude", *photoMag);
    cout << "   DONE" << endl;
    
    Mat *modelMag = new Mat(model.rows,model.cols,model.type(),Scalar(0,0,0));
    Mat *modelPhase = new Mat(model.rows,model.cols,model.type(),Scalar(0,0,0));
    cout << "Computing model edge...";
    EdgeDetection::detect(model, kernel, tLow, tHigh, modelMag, modelPhase,filterModel); // Edge of the model
    imshow("Model edge magnitude", *modelMag);
    cout << "   DONE" << endl;
    
    if (doVCC) {
    
    // Computing the VCC (Vectorialized cross-correlation) that gives the value of the traslation tu superimpose the picture
        Mat *VCC = new Mat();
        cout << "Computing photo Vector Cross Correlation...";
        SpaceReduction::vcc(photoMag, photoPhase, modelMag, modelPhase, VCC);
        commonFunction::showFloatMat(VCC,"VCC");
        cout << "   DONE" << endl;
    
        //Overlaying the images
        Mat *output = new Mat();
        cout << "Superimposing images...";
        realityAugmentation::setPeakName(&model, scaledPhoto, VCC, &label, output);
        imshow("results", *output);
        cout << "   DONE" << endl;
        //commonFunction::writeImage(output, OutputName);
        //imwrite(OutputName, *output);
        if (drawArea)
        {
            Point origin(0,0);
            Point vertex(model.cols,model.rows);
            rectangle(*VCC, origin, vertex,Scalar(255,0,0), 2);
        }
        //commonFunction::writeImage(VCC, VCC_name);
    }
    // Writing the edges images to file to analyse the with matlab
    //commonFunction::writeImage(scaledPhoto, scaledPhotoName);
    //imwrite(modellMag, *modelMag);
    //commonFunction::writeImage(modelMag, modellMag);
    //imwrite(phaseMag, *modelPhase);
    //commonFunction::writeImage(modelPhase, phaseMag);
    //imwrite(magPhoto, *photoMag);
    //commonFunction::writeImage(photoMag, magPhoto);
    //imwrite(phasePhoto, *photoPhase);
    //commonFunction::writeImage(photoPhase, phasePhoto);

    waitKey(0);
	return 0;
}
