//
//  EstimateScale.cpp
//  ProgettoDIP
//
//  Created by Riccardo Fona on 12/09/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

// Including libraries
// choosing the correct libraries in function of the enviroment
#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

#include "EstimateScale.h"
#include <math.h>

using namespace cv;
using namespace std;

float estimateScale::calculateScale(int panoramaWidth, int photoWidth, float sensorWidth, float sensorHeight, float focal)
{
    float FOV = 2*atan(sensorWidth/(2*focal));
    float scale = FOV*panoramaWidth/(2*M_PI*photoWidth);
    cout << "Scaling factor:   ";
    cout << scale << endl;
    return scale;
}

void estimateScale::setScale(Mat *photo, Mat *model, float sensorWidth, float sensorHeight, float focal, Mat *output, bool needScale)
{
    if (needScale)
    {
        float scale = estimateScale::calculateScale(model->cols, photo->cols, sensorWidth, sensorHeight, focal);
        // Creating a matrix with the new dimension
        *output = Mat(round(photo->rows*scale),round(photo->cols*scale),photo->type());
    
        resize(*photo /* input image*/, *output /*result image */, output->size()/*new dimensions*/, 0, 0, INTER_CUBIC/* interpolation method*/);
        
        cout << output->cols << endl;
        cout << output->rows << endl;
        
        imshow("scaled image", *output);
    }
    else
    {
        photo->copyTo(*output);
        cout << "scaling factor: 1"<<endl;
    }
    
    
}
