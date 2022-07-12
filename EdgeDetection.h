//
//  EdgeDetection.h
//  ProgettoDIP
//
//  Created by Riccardo Fona on 08/08/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#ifndef __ProgettoDIP__EdgeDetection__
#define __ProgettoDIP__EdgeDetection__

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

// Declaring class 
class EdgeDetection
{
private:
    void static rotateKernel(Mat,Mat*, float); // "Rotate" the kernel by a desired angle
    void static normalize(Mat*); // Normalize value between 0 and 255
    void static setZero(Mat*, Mat*); // check if a zero in the magnitude matrix correspond to a zero in the direction Matrix
    void static cvtGray(Mat*); // converting the edges map (magnitude and direction to gray scale)
    void static writeFile(Mat);

public:
    void static detect(Mat ,Mat, int const, int const, Mat*, Mat*, Scalar); // Execute the compass edge detection generate the magnitude and direction matrix
    void static edegeFilter(Mat*,float); // edge filtering to reduce noise (attenuate the magnitude more in the bottom)

};

#endif /* defined(__ProgettoDIP__EdgeDetection__) */
