//
//  PanoramaSynthesys.h
//  ProgettoDIP
//
//  Created by Riccardo Fona on 08/08/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#ifndef __ProgettoDIP__PanoramaSynthesys__
#define __ProgettoDIP__PanoramaSynthesys__

// Including libraries
// choosing the correct libraries in function of the enviroment
#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

#include <iostream>

using namespace std;
using namespace cv;

class PanoramaSynthesys {
    
public:
    void static genSilhouette(Mat *);
};

#endif /* defined(__ProgettoDIP__PanoramaSynthesys__) */
