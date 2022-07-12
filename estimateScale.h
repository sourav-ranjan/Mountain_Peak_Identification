//
//  EstimateScale.h
//  ProgettoDIP
//
//  Created by Riccardo Fona on 12/09/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#ifndef __ProgettoDIP__estimateScale__
#define __ProgettoDIP__estimateScale__

// Including libraries
// choosing the correct libraries in function of the enviroment

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class estimateScale {
    
private:
    static float calculateScale(int, int, float, float, float);
public:
    static void setScale(Mat*, Mat*, float, float, float, Mat*, bool); // Take in input the images, the dimension of the sensor and the focal lengt to estimate the FOV and from that the scale
};

#endif /* defined(__ProgettoDIP__EstimateScale__) */
