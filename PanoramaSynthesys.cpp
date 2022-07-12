//
//  PanoramaSynthesys.cpp
//  ProgettoDIP
//
//  Created by Riccardo Fona on 08/08/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

// Including libraries
// choosing the correct libraries in function of the enviroment
#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

using namespace std;
using namespace cv;

#include "PanoramaSynthesys.h"

void PanoramaSynthesys::genSilhouette(Mat* input)
{
    Canny(*input, *input, 25, 100);
}
