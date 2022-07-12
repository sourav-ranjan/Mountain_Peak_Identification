//
//  commonFunction.h
//  ProgettoDIP
//
//  Created by Riccardo Fona on 05/10/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#ifndef __ProgettoDIP__commonFunction__
#define __ProgettoDIP__commonFunction__

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opnecv.hpp>
#endif

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class commonFunction {
    
private:
    static Mat FTA(Mat_<float> S, Mat srcRow, int numberOfMax, int spotSize);
    static Mat convertRGB(Mat*);
    
public:
    static void readFile(Mat*, String); // Read image from file and put it in a mat
    static void writeFile(Mat*, char); // Write image as matrix to file
    static void writeImage(Mat*, String); // Write image to file
    static void normalize(Mat*, Mat*); // Normalize value between [0,1] and converting to float
    static void setZero(Mat*, Mat*); // Set the values of the second matrix to zero if they are zeros in the first one
    static void overlay(Mat*, Mat*, Mat*, Point); // Overlays the second images to the first and copy the in the third value that is the output
    static void overlay(Mat*, Mat*, Mat*); // Overlays the second images to the first and copy the in the third value that is the output supposing zero translation
    static void phaseMagToRealImag(Mat*, Mat*, Mat*); // Build a complex matrix starting from magnitude and direction matrice
    static void phaseMagToRealImag(Mat*, Mat*);
    static void RealImagToPhaseMag(Mat*, Mat*, Mat*);
    static void RealImagToPhaseMag(Mat*, Mat*);
    static void crop(Mat*, Mat*);
    static void showFloatMat(Mat*,String);
    static Mat findPeaks(Mat_<float>, int, int);
    static Mat coloredPeaks(Mat_<float> S, Mat src , int FTAspotSize=6, int NumFTAvalue=10, float offset=50);
    
};


#endif /* defined(__ProgettoDIP__commonFunction__) */
