//
//  morphology.cpp
//  Lab16-05
//
//  Created by Riccardo Fona on 16/05/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#include "morphology.h"

 Mat Morphology::erosion(Mat input)
{
    Mat img = checkType(input);
    int lPixel,rPixel,uPixel,dPixel;
    Mat erosion(img.rows,img.cols,CV_8UC1,Scalar(0)); // Matrix in which put the "eroded" image

    for(int x = 0; x < img.rows; x++)
    {
        for (int y = 0; y < img.cols; y++)
        {
            // The fors four if take in accout of the border
            if (x == 0)
            {
                lPixel = 0;
            }
            else
            {
                lPixel = img.at<uchar>(x-1,y);
            }
            if (x == img.rows - 1)
            {
                rPixel = 0;
            }
            else
            {
                rPixel = img.at<uchar>(x+1,y);
            }
            if (y == 0)
            {
                uPixel = 0;
            }
            else
            {
                uPixel = img.at<uchar>(x,y -1);
            }
            if (y == img.cols - 1)
            {
                dPixel = 0;
            }
            else
            {
                dPixel = img.at<uchar>(x,y+1);
            }
            if (lPixel == 255 || rPixel == 255 || uPixel == 255 || dPixel == 255)
            {
                erosion.at<uchar>(x,y) = 255; // if the condition are satisfied the pixel goes into backgroud
            }
        }
        
    }
    
    return erosion;
}

Mat Morphology::dilation(Mat input)
{
    Mat img = checkType(input);
    int lPixel,rPixel,uPixel,dPixel;
    Mat dilation(img.rows,img.cols,CV_8UC1,Scalar(255)); // Matrix in which put the "dilated" image
    
    for(int x = 0; x < img.rows; x++)
    {
        for (int y = 0; y < img.cols; y++)
        {
            // The fors four if take in accout of the border
            if (x == 0)
            {
                lPixel = 255;
            }
            else
            {
                lPixel = img.at<uchar>(x-1,y);
            }
            if (x == img.rows - 1)
            {
                rPixel = 255;
            }
            else
            {
                rPixel = img.at<uchar>(x+1,y);
            }
            if (y == 0)
            {
                uPixel = 255;
            }
            else
            {
                uPixel = img.at<uchar>(x,y -1);
            }
            if (y == img.cols - 1)
            {
                dPixel = 255;
            }
            else
            {
                dPixel = img.at<uchar>(x,y+1);
            }
            if (lPixel == 0 || rPixel == 0 || uPixel == 0 || dPixel == 0)
            {
                dilation.at<uchar>(x,y) = 0; // if the condition are satisfied the pixel goes into foreground
            }
        }
    }
    return dilation;

}

Mat Morphology::opening(Mat input)
{
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)); // Calculating the structuring element
    cv::erode(input, input, element);
    cv::dilate(input, input, element);
    //Mat opened = dilation((erosion(input)));
    Mat opened = input;
    return opened;
}

Mat Morphology::closing(Mat input)
{
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)); // Calculating the structuring element
    cv::dilate(input, input, element);
    cv::erode(input, input, element);
    //Mat closed = erosion(dilation(input));
    Mat closed = input;
    return closed;
}

Mat Morphology::contour(Mat input)
{
    // extracting contour using the xor between the original image and the dilated one
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)); // Calculating the structuring element
    cv::dilate(input,input,element);
    Mat contour = input^input;
    return contour;
}

// Using OpenCV function instead of mine to improve performances!!!!
Mat Morphology::skeleton(Mat input)
{
    cv::Mat skeleton(input.size(), CV_8UC1, cv::Scalar(0));
    cv::Mat temp; // Temporary matrix to do the operation
    cv::Mat eroded; // Matrix into out the eroded image in order to reutilize it after
    
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)); // Calculating the structuring element
    // Control variable
    bool done;
    do
    {
        cv::erode(input, eroded, element);
        //eroded = erosion(input);
        cv::dilate(eroded, temp, element); // temp = open(img)
        //temp = dilation(eroded);
        cv::subtract(input, temp, temp);
        cv::bitwise_or(skeleton, temp, skeleton);
        eroded.copyTo(input);
        
        done = (cv::countNonZero(input) == 0); // check if the image have non zero pixel yet
    } while (!done);

    return skeleton;
}

Mat Morphology::checkType(Mat input)
{
    Mat output;
    if (input.channels() == 1)
    {
        output = input;
    }
    else
    {
        cvtColor(input, output, CV_BGR2GRAY);
    }
    return output;
}
