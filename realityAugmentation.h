//
//  realityAugmentation.h
//  ProgettoDIP
//
//  Created by Riccardo Fona on 12/10/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#ifndef __ProgettoDIP__realityAugmentation__
#define __ProgettoDIP__realityAugmentation__

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

#include <iostream>

using namespace cv;
using namespace std;

class realityAugmentation {
private:
    static cv::Point extractLoacation(Mat*, Mat*); // Find of how many pixels the image has to be translated to be correctly superimposed on the syntethic panorama
public:
    static void setPeakName(Mat*, Mat*, Mat*, Mat*, Mat*); // Overlays the taken picture to the synthetic panorama with all the name of the peak
};

#endif /* defined(__ProgettoDIP__realityAugmentation__) */
