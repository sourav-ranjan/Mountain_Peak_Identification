//
//  commonFunction.cpp
//  ProgettoDIP
//
//  Created by Riccardo Fona on 05/10/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#include "commonFunction.h"

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

void commonFunction::readFile(Mat *output, String imageName)
{
    *output = imread(imageName, CV_LOAD_IMAGE_COLOR);
    if(output->empty()){
        cerr << "ERROR! image not loaded"<<endl;
        //return -1;
        exit(1);
    }
}

void commonFunction::writeImage(Mat *input, String fileName)
{
    Mat image = commonFunction::convertRGB(input);
    imwrite(fileName, image);
    cout << "file saved in: " << endl;
    cout << fileName << endl;
}

void commonFunction::overlay(Mat* model, Mat *photo, Mat *output, Point location)
{
    // Declaring parameter to have transparency
    CvScalar S = cvScalar(0.3, 0.3, 0.3, 0.3);
    //CvScalar S = cvScalar(0.0, 0.0, 0.0, 0.0);
    CvScalar D = cvScalar(0.7, 0.7, 0.7, 0.7);
    // copyng the pixel value of the mothel into the output matrix
    /* MANCA IL CONTROLLO SUL FATTO CHE SI POSSA USCIRE DALLA DIMENSIONE DELL'IMMAGINE */
    model->copyTo(*output);
    for(int x = 0; x < photo->cols; x++)
    {
        for(int y = 0; y < photo->rows; y++)
        {
            for (int ch = 0; ch < photo->channels(); ch++)
            {
                if ((y + location.y < output->rows) && (x+location.x < output->cols))
                    {
                        output->at<Vec3b>(y + location.y,x + location.x)[ch] = S.val[ch]*model->at<Vec3b>(y + location.y,x + location.x)[ch] + D.val[ch]*photo->at<Vec3b>(y,x)[ch];
                    }
            }
        }
    }

}

void commonFunction::overlay(Mat* model, Mat *photo, Mat *output)
{
    // Declaring parameter to have transparency
    CvScalar S = cvScalar(0.7, 0.7, 0.7, 0.7);
    CvScalar D = cvScalar(0.3, 0.3, 0.3, 0.3);
    // copyng the pixel value of the mothel into the output matrix
    /* MANCA IL CONTROLLO SUL FATTO CHE SI POSSA USCIRE DALLA DIMENSIONE DELL'IMMAGINE */
    model->copyTo(*output);
    for(int x = 0; x < photo->cols; x++)
    {
        for(int y = 0; y < photo->rows; y++)
        {
            for (int ch = 0; ch < photo->channels(); ch++)
            {
                //if ((y + location.y < output->rows) && (x+location.x < output->cols))
                {
                    output->at<Vec3b>(y,x)[ch] = S.val[ch]*model->at<Vec3b>(y,x)[ch] + D.val[ch]*photo->at<Vec3b>(y,x)[ch];
                }
            }
        }
    }
    
}


// Write image as matrix to file
void commonFunction::writeFile(Mat *input, char fileName)
{
    ofstream file(&fileName);

    if (file.is_open())
    {
        file << *input << endl;
        file.close();
    }
}

void commonFunction::setZero(Mat *magnitude, Mat *direction)
{
    for (int x = 0; x < magnitude->cols; x++)
    {
        for (int y = 0; y < magnitude->rows; y++)
        {
            if (magnitude->at<float>(y,x) == 0)
            {
                direction->at<uchar>(y,x) = 0;
            }
        }
    }
}

// Convert values to float and normalize them in the range [0,1]
void commonFunction::normalize(Mat *input, Mat *output)
{
    // Converting matrix to float
    Mat *floatInput = new Mat(input->rows, input->cols, CV_32FC1, Scalar(0));
    input->convertTo(*floatInput, CV_32FC1);
    
    // Computing the maximum value
    float max = *max_element(floatInput->begin<float>(), floatInput->end<float>());
    // Computing the minimum Value
    float min = *min_element(floatInput->begin<float>(), floatInput->end<float>());

    // Normalizing
    for (int x = 0; x < input->cols; x++)
    {
        for (int y = 0; y < input->rows; y++)
        {
            output->at<float>(y,x) = (floatInput->at<float>(y,x) - min)/(max - min);
        }
    }
}

void  commonFunction::phaseMagToRealImag(Mat *magnitude, Mat *phase, Mat *output)
{
    
    // Normalize the value of the magnitude between [0, 1]
    Mat *normalizedMag = new Mat(magnitude->rows,magnitude->cols,CV_32FC1,Scalar(0));
    magnitude->copyTo(*normalizedMag);
    //commonFunction::normalize(magnitude, normalizedMag);
    
    // Defining the vector of cartesian representation
    Mat planesModel[] = {Mat_<float>(), Mat::zeros(normalizedMag->size(), CV_32F)};
    planesModel[0] = planesModel[1];
    
    for (int m = 0; m < magnitude->rows; m++)
    {
        for(int n = 0; n < magnitude->cols; n++)
        {
            // Computing the real part
            planesModel[0].at<float>(m,n) = normalizedMag->at<float>(m,n)*cos((phase->at<uchar>(m,n))*M_PI/180.0);
            
            // Computing the imaginary part
            planesModel[1].at<float>(m,n) = normalizedMag->at<float>(m,n)*sin((phase->at<uchar>(m,n))*M_PI/180.0);
        }
    }
    // Merging the Mat vector of the real and immaginary part together in a single Mat object
    merge(planesModel, 2, *output);
}

void commonFunction::crop(Mat *input, Mat *output)
{
    // Creating a matrix with the only the region of interest of the original image
    //Mat myROI(*input, Rect(0, 0, output->cols, output->rows));
    cout << output-> cols << endl;
    cout << output-> rows << endl;
    
    // Selecting the last part of VCC that must be copied at beginning in order to take in account of the fact that the VCC is horizontally periodic
    Mat periodicRep(*input, Rect(output->cols,0,input->cols - output->cols,input->rows));
    for (int m = 0; m < periodicRep.rows; m++)
    {
        for (int n = 0;  n < periodicRep.cols; n++)
        {
            input->at<float>(m,n) += periodicRep.at<float>(m,n);
        }
    }
    
    // Cropping the VCC to select only the "valid area" a period
    Mat cropped(*input,Rect(0,0,output->cols - 1,output->rows - 1));
    cropped.copyTo(*output);
    commonFunction::showFloatMat(output,"Cropped VCC (and normalized between [0,255]");
    //imshow("croppedVCC", *output);
}

void commonFunction::showFloatMat(Mat *VCC, String WindowName)
{
    // Searching the maximum value
    float max = *max_element(VCC->begin<float>(), VCC->end<float>());
    // Searching the minum value
    float min = *min_element(VCC->begin<float>(), VCC->end<float>());
    // Normlaizing between zero and 255
    Mat ucharVCC(VCC->rows, VCC->cols, VCC->type(),Scalar::all(0));
    for (int m = 0; m < VCC->rows; m++)
    {
        for (int n = 0; n < VCC->cols; n++)
        {
            ucharVCC.at<float>(m,n) = 255*(VCC->at<float>(m,n) - min)/(max - min);
        }
    }
    // Converting in uchar
    ucharVCC.convertTo(ucharVCC, CV_8UC1);
    imshow(WindowName, ucharVCC);
}

Mat commonFunction::findPeaks(Mat_<float> S, int numberOfMax, int spotSize){
    //create temp image where FTA work and maxPoint where the result will put
    Mat temp = S.clone();
    Mat maxPoint(Mat::zeros(S.rows, S.cols, CV_32FC1));
    
    //create che gaussian kernel that will aplied in the maximum
    int gausSize = S.rows+S.rows%2-1;
    Mat kernelX = getGaussianKernel(gausSize, maxPoint.rows/spotSize/2, CV_32FC1);
    Mat kernelY = getGaussianKernel(gausSize, maxPoint.rows/spotSize/2, CV_32FC1);
    Mat gaus = kernelX * kernelY.t();
    int gausCenter = (gausSize+1)/2;
    
    //search the max
    double min,max;
    Point maxLoc, minLoc;
    for (int k=0; k<numberOfMax; k++) {
        minMaxLoc(temp,&min,&max,&minLoc,&maxLoc);
        int X, Y;
        X=maxLoc.x; Y=maxLoc.y;
        
        //add in the maximum position the gaussian
        int gx, gy;
        for(int y = 0; y < maxPoint.rows; y++){
            for(int x = 0 ; x < maxPoint.cols; x++){
                gx= gausCenter+(x-X);
                gy= gausCenter+(y-Y);
                if(gx<gaus.cols && gx>=0 && gy<gaus.rows && gy>=0){
                    
                    maxPoint.at<float>(y,x)=maxPoint.at<float>(y,x)+(gaus.at<float>(gy,gx)*max);
                }
            }
        }
        
        //set to zero the area with spot size arround the maximum
        for(int y = -temp.rows/spotSize; y < temp.rows/spotSize; y++){
            for(int x = -temp.rows/spotSize ; x < temp.rows/spotSize; x++){
                if(X+x<temp.cols && X+x>=0 && Y+y<temp.rows && Y+y>=0){
                    
                    temp.at<float>(y+Y,x+X)=0.0;
                    
                }
                
            }
        }
        
        
    }
    
    return maxPoint;
}

Mat commonFunction::coloredPeaks(Mat_<float> S, Mat src , int FTAspotSize, int NumFTAvalue, float offset){
    //resize S to the source image dimension
    Mat Sres;
    resize(S, Sres, src.size());
    
    //Compute the FTA algoritm and create a map with gaussian spots
    Mat Sspot;
    Sspot=FTA(Sres, src, NumFTAvalue, FTAspotSize);
    
    //Normalize the image over 1024+offset(the offset will not visualized)
    
    double min, max;
    minMaxLoc(Sspot,&min,&max);
    Mat_<float> sNorm;
    sNorm = ((Sspot-min)/(max-min))*(1023.0+offset);
    sNorm=sNorm-offset;
    
    //make the color scale image
    Mat color(Sres.rows,Sres.cols,CV_8UC3);
    
    for(int y = 0; y < color.rows; y++){
        for(int x = 0; x < color.cols; x++){
            
            if(0 <= sNorm.at<float>(y,x) && sNorm.at<float>(y,x) <256){
                color.at<Vec3b>(y,x)[0]=255;
                color.at<Vec3b>(y,x)[1]=(uchar)sNorm.at<float>(y,x);
                color.at<Vec3b>(y,x)[2]=0;
                
            }
            
            else if(256 <= sNorm.at<float>(y,x) && sNorm.at<float>(y,x) <512){
                color.at<Vec3b>(y,x)[0]=511-(uchar)sNorm.at<float>(y,x);
                color.at<Vec3b>(y,x)[1]=255;
                color.at<Vec3b>(y,x)[2]=0;
                
            }
            
            else if(512 <= sNorm.at<float>(y,x) && sNorm.at<float>(y,x) <768){
                color.at<Vec3b>(y,x)[0]=0;
                color.at<Vec3b>(y,x)[1]=255;
                color.at<Vec3b>(y,x)[2]=(uchar)sNorm.at<float>(y,x)-512;
                
            }
            
            else if(768 <= sNorm.at<float>(y,x) && sNorm.at<float>(y,x) <1024){
                color.at<Vec3b>(y,x)[0]=0;
                color.at<Vec3b>(y,x)[1]=1023-(uchar)sNorm.at<float>(y,x);
                color.at<Vec3b>(y,x)[2]=255;
                
            }
            
            
            else {
                color.at<Vec3b>(y,x)[0]=0;
                color.at<Vec3b>(y,x)[1]=0;
                color.at<Vec3b>(y,x)[2]=0;
                
            }
            
        }
        
    }
    
    return color;
}


Mat commonFunction::FTA(Mat_<float> S, Mat srcRow, int numberOfMax, int spotSize){
    //create temp image where FTA work and maxPoint where the result will put
    Mat temp = S.clone();
    Mat maxPoint(Mat::zeros(S.rows, S.cols, CV_32FC1));
    
    //create che gaussian kernel that will aplied in the maximum
    int gausSize = S.rows+S.rows%2-1;
    Mat kernelX = getGaussianKernel(gausSize, maxPoint.rows/spotSize/2, CV_32FC1);
    Mat kernelY = getGaussianKernel(gausSize, maxPoint.rows/spotSize/2, CV_32FC1);
    Mat gaus = kernelX * kernelY.t();
    int gausCenter = (gausSize+1)/2;
    
    // declaring vector of max values and position
    vector<double> maxVal;
    vector<Point> maxLocation;
    
    //search the max
    double min,max;
    Point maxLoc, minLoc;
    for (int k=0; k<numberOfMax; k++) {
        minMaxLoc(temp,&min,&max,&minLoc,&maxLoc);
        // create a vector with maximum and theri position
        maxVal.push_back(max);
        maxLocation.push_back(maxLoc);
        int X, Y;
        X=maxLoc.x; Y=maxLoc.y;
        
        //add in the maximum position the gaussian
        int gx, gy;
        for(int y = 0; y < maxPoint.rows; y++){
            for(int x = 0 ; x < maxPoint.cols; x++){
                gx= gausCenter+(x-X);
                gy= gausCenter+(y-Y);
                if(gx<gaus.cols && gx>=0 && gy<gaus.rows && gy>=0){
                    
                    maxPoint.at<float>(y,x)=maxPoint.at<float>(y,x)+(gaus.at<float>(gy,gx)*max);
                }
                
            }
        }
        
        //set to zero the area with spot size arround the maximum
        for(int y = -temp.rows/spotSize; y < temp.rows/spotSize; y++){
            for(int x = -temp.rows/spotSize ; x < temp.rows/spotSize; x++){
                if(X+x<temp.cols && X+x>=0 && Y+y<temp.rows && Y+y>=0){
                    
                    temp.at<float>(y+Y,x+X)=0.0;
                    
                }
                
            }
        }
        
        
    }
    for( std::vector<double>::const_iterator i = maxVal.begin(); i != maxVal.end(); ++i)
        std::cout << *i << ' ';
    std::ofstream output_file("/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/maxVal.txt");
    std::ostream_iterator<double> output_iterator(output_file, "\n");
    std::copy(maxVal.begin(), maxVal.end(), output_iterator);
    
    std::ofstream loc_file("/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/maxLoc.txt");
    std::ostream_iterator<Point> loc_iterator(loc_file, "\n");
    std::copy(maxLocation.begin(), maxLocation.end(), loc_iterator);
    
    return maxPoint;
}

Mat commonFunction::convertRGB(Mat *VCC)
{
    // Searching the maximum value
    float max = *max_element(VCC->begin<float>(), VCC->end<float>());
    // Searching the minum value
    float min = *min_element(VCC->begin<float>(), VCC->end<float>());
    // Normlaizing between zero and 255
    Mat ucharVCC(VCC->rows, VCC->cols, VCC->type(),Scalar::all(0));
    if (VCC->channels() == 1) {
        for (int m = 0; m < VCC->rows; m++)
        {
            for (int n = 0; n < VCC->cols; n++)
            {
                ucharVCC.at<float>(m,n) = 255*(VCC->at<float>(m,n) - min)/(max - min);
            }
        }
        // Converting in uchar
        ucharVCC.convertTo(ucharVCC, CV_8UC1);
    }
    else
    {
        for (int ch = 0; ch < VCC->channels(); ch++)
        {
            for (int m = 0; m < VCC->rows; m++)
            {
                for (int n = 0; n < VCC->cols; n++)
                {
                    ucharVCC.at<Vec3b>(m,n)[ch] = 255*(VCC->at<Vec3b>(m,n)[ch] - min)/(max - min);
                }
            }
        }
        // Converting in uchar
        ucharVCC.convertTo(ucharVCC, CV_8UC3);
    }
    // Converting in uchar
    //ucharVCC.convertTo(ucharVCC, CV_8UC1);
    return ucharVCC;
}

