//
//  morphology.h
//  Lab16-05
//
//  Created by Riccardo Fona on 16/05/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#ifndef __Lab16_05__morphology__
#define __Lab16_05__morphology__

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Morphology
{
private:
    static Mat checkType(Mat);
public:
	static Mat erosion(Mat);
    static Mat dilation(Mat);
    static Mat contour(Mat);
    static Mat skeleton(Mat);
    static Mat opening(Mat);
    static Mat closing(Mat);
};


#endif /* defined(__Lab16_05__morphology__) */
