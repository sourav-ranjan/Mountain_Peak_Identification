//
//  SpaceReduction.h
//  ProgettoDIP
//
//  Created by Riccardo Fona on 21/08/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#ifndef __ProgettoDIP__SpaceReduction__
#define __ProgettoDIP__SpaceReduction__

#include <iostream>

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

using namespace std;
using namespace cv;

class SpaceReduction {
    static void dft(Mat*, Mat*, Mat*); // Calulate the dft of a complex vector and the crosscorrelation between complex matrices
    static void makePeriod (Mat*, Mat); // From the "standard convolution" computed in frequency buil the circular convolution
    
public:
    static void vcc(Mat*, Mat*, Mat*, Mat*, Mat*); // Calculate the Vector cross correlation between two complex vector and return the normalized real part of the VCC
};

#endif /* defined(__ProgettoDIP__SpaceReduction__) */
