//
//  EdgeDetection.cpp
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

#include "EdgeDetection.h"
#include "morphology.h"
#include "histogram.hpp"
#include "commonFunction.h"

using namespace cv;
using namespace std;

// Generate the rotated kernel for the "Compass Edge Detector"
void EdgeDetection::rotateKernel(Mat Kernel, Mat *RotatedKernel, float degrees)
{
    // Defining the dimension of the new kernel equal to the original one
    *RotatedKernel = (Mat_<double>(Kernel.rows,Kernel.cols) << 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Cenrer of the rotation (the center value of the kernel
    Point center = Point(Kernel.cols/2,Kernel.rows/2);
    
    for (int y = 0; y < Kernel.rows; y++)
    {
        for (int x = 0; x < Kernel.cols; x++)
        {
            //for (int compass = 0; compass < Kernel.rows; compass++)
            {
                //double radians = compass * (degrees * M_PI / 180.0);
                
                double radians = (degrees * M_PI / 180.0);
                
                int newX = (int)(round((x - center.x) * cos(radians) - round(y - center.y) * sin(radians)) + center.x);
                
                int newY = (int )(round((x - center.x) * sin(radians) + round(y - center.y) * cos(radians)) + center.y);
                
                RotatedKernel->at<double>(y, x) = Kernel.at<double>(newY, newX);
            } 
        } 
    }
}
// Compass Edge Deetection Algorithm
// Devi correggere tutti i nomi in modo che si possa seguire un filo logico....
// Mancherebbe un equalizzazione prima del thresholding per migliorare il risultato (puoi usare la tecnica degli istogrammi)
void EdgeDetection::detect(Mat input1, Mat Kernel, int const tLow, int const tHigh, Mat *magnitude, Mat *directions, Scalar param)
{
    int thresh = param(0);
    int morph = param(1);
    int filter = param(2);
    // Initializing the matrix of the "rotated kernel"
    Mat *RotatedKernel = new Mat;

    // Pre filtering low pass the image in order to reduce the noise (the edge detector are sensible to the noise..)
    Mat lowpassfilter(3,3,CV_32FC1, Scalar(1.0/9));
    Mat input;
    filter2D(input1, input, input1.depth(), lowpassfilter);

    for (int i = 0; i <8; i++)
    {
        Mat edges(input.rows,input.cols,input.type(),Scalar(0,0,0));
        float degrees = 45*i;
        rotateKernel(Kernel, RotatedKernel, degrees);
    
        filter2D(input, edges, input.depth(), *RotatedKernel);

        // selecting for each channel the maximum value of the convolution
        // at every iteration check if the pixel value stored in the output is lower than the new covolution results in this case the value is substituded with the new one
        for (int k = 0; k < magnitude->channels(); k++)
        {
            for (int x = 0; x < magnitude->rows; x++)
            {
                for (int y = 0; y < magnitude->cols; y++)
                {
                    if (edges.at<Vec3b>(x,y)[k] > magnitude->at<Vec3b>(x,y)[k])
                    {
                        magnitude->at<Vec3b>(x,y)[k] = edges.at<Vec3b>(x,y)[k];
                        directions->at<Vec3b>(x,y)[k] =45*i; // By now the angles are in degree to compute the VCC we need angle in radians
                    }
                }
            }
        }
    }
    //imshow("colored directions",*directions);
    //imshow("colored magnitude",*magnitude);
    //imwrite("/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/PaternoBenini/magnitude_color.jpg", *magnitude);
    // Normalizing the pixel value between 0 and 255
    normalize(magnitude);
    
    /* *****TEST******* */
    Mat *merged = new Mat(magnitude->rows,magnitude->cols,CV_32FC1,Scalar(0));
    for (int m = 0; m < magnitude->rows; m++) {
        for (int n = 0; n < magnitude->cols; n++) {
     
            merged->at<float>(m,n) = (magnitude->at<Vec3b>(m, n)[1]+magnitude->at<Vec3b>(m, n)[2]+magnitude->at<Vec3b>(m, n)[3])/3;
        }
     }
    //commonFunction::showFloatMat(merged, "merged");
    merged->convertTo(*magnitude, CV_8UC1);
    //imshow("gray mag", *magnitude);
    //merged->copyTo(*magnitude);
    /* *****END TEST****** */
    // converting the magnitude to a gray scale
    //EdgeDetection::cvtGray(magnitude);
    
    // Converting the phase to grayscale
    //EdgeDetection::cvtGray(directions);
    /* ******* TEST: avoiding conversion in grayscale and equally weightig the component */
    Mat *mergedPhase = new Mat(magnitude->rows,magnitude->cols,CV_32FC1,Scalar(0));
    for (int m = 0; m < magnitude->rows; m++) {
        for (int n = 0; n < magnitude->cols; n++) {
            
            mergedPhase->at<float>(m,n) = (directions->at<Vec3b>(m, n)[1]+directions->at<Vec3b>(m, n)[2]+directions->at<Vec3b>(m, n)[3])/3;
        }
    }
    mergedPhase->convertTo(*directions, CV_8UC1);
    /* ******** END ********* */
    // Applyng a treshold with hysteresis on the results of ther compass edge detection and trasform the results of the detection into a binary image
    if (thresh == 1)
    {
        //threshold(*magnitude, *magnitude, tLow, tHigh, cv::THRESH_BINARY);
        //Mat *tempMagnitude = new Mat(magnitude->rows,magnitude->cols,magnitude->type(),Scalar(0));
        threshold(*magnitude, *magnitude, tLow, tHigh, cv::THRESH_OTSU);
        // In this way the threshold is applied without loosing information on the magnitude
        /*for (int m = 0; m < magnitude->rows; m++) {
            for (int n = 0; n < magnitude->cols; n++) {
                
                if (tempMagnitude->at<uchar>(m,n) == 0) {
                    magnitude->at<Scalar>(m,n) = 0;
                }
            }
        }*/
    }
    
    // Reducing the lines of the binary image to a one pixel thinckness lines
    if (morph == 1)
    {
    *magnitude = Morphology::skeleton(*magnitude);
    }
    
    // filtering the edges to remove some noise
    if (filter == 1)
    {
    edegeFilter(magnitude, 0.7);
    }
    else
    {
        Mat *EdgeMagnitude = new Mat(magnitude->rows,magnitude->cols,CV_32FC1,Scalar(0));
        commonFunction::normalize(magnitude, EdgeMagnitude);
        EdgeMagnitude->copyTo(*magnitude);
    }
    // Setting to zero in the direction matrix the value of the pixel that are zero in the magnitude matrix
    // In fact after the thresholding we have some extra zero pixel in the magnitude matrix
    commonFunction::setZero(magnitude, directions);
}

// Normalize matrix value between 0 and 255 (correct value for a 8bit per channels image)
void EdgeDetection::normalize(Mat *input)
{
    for (int k = 0; k < input->channels(); k++)
    {
        for (int x = 0; x < input->rows; x++)
        {
            for(int y = 0; y < input->cols; y++)
            {
                if (input->at<Vec3b>(x,y)[k] < 0)
                {
                    input->at<Vec3b>(x,y)[k] = 0;
                }
                
                if (input->at<Vec3b>(x,y)[k] > 255)
                {
                    input->at<Vec3b>(x,y)[k] = 255;
                }
            }
        }
    }
}

void EdgeDetection::cvtGray(Mat *input)
{
    cvtColor(*input, *input, CV_BGR2GRAY ); // converting the colour image in a gray scale image
}

// attenuates the botton part of the edges where usually in mountain we only have noise
void EdgeDetection::edegeFilter(Mat *input, float b)
{
    Mat *EdgeMagnitude = new Mat(input->rows,input->cols,CV_32FC1,Scalar(0));
    commonFunction::normalize(input, EdgeMagnitude);
    int i;
    for (int n = 0; n < EdgeMagnitude->cols; n++) {
        i = 1;
        for (int m = 0; m < EdgeMagnitude->rows; m++) {
            
            EdgeMagnitude->at<float>(m,n) = EdgeMagnitude->at<float>(m,n)*pow(b,i - 1);
            
            if (m > 0 && EdgeMagnitude->at<float>(m - 1,n) == 0  && EdgeMagnitude->at<float>(m,n) != 0)
            {
                i++;
            }
            
            if (m > 0 && EdgeMagnitude->at<float>(m - 1,n) != 0  && EdgeMagnitude->at<float>(m,n) != 0)
            {
                i++;
            }
            //cout << i << endl;
            
        }
    }
    EdgeMagnitude->copyTo(*input);
}
