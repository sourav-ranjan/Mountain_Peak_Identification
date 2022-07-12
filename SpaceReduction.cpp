//
//  SpaceReduction.cpp
//  ProgettoDIP
//
//  Created by Riccardo Fona on 21/08/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#include "SpaceReduction.h"

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

#include <math.h>
#include "commonFunction.h"

using namespace std;
using namespace cv;

void SpaceReduction::vcc(Mat *photoMag, Mat* photoPhase, Mat *modelMag, Mat *modelPhase, Mat *output)
{
    Mat *ComplexPhotoEdge = new Mat();
    Mat *ComplexModelEdge = new Mat();
    
    // Converting the edge mapp from magnitude and direction to a complex matrix tha express real and immaginary part
    commonFunction::phaseMagToRealImag(photoMag, photoPhase, ComplexPhotoEdge);
    commonFunction::phaseMagToRealImag(modelMag, modelPhase, ComplexModelEdge);
    
    // Executing the DFT between the vector
    SpaceReduction::dft(ComplexPhotoEdge, ComplexModelEdge, output);
}

void SpaceReduction::dft(Mat *ComplexPhotoEdge, Mat *ComplexModelEdge, Mat *realVCC)
{
    // compute the size of DFT transform
    // The two DFTs must have the same dimension otherwise is impossible to do the multiplication
    Size dftSize;
    dftSize.width = ComplexPhotoEdge->cols + ComplexModelEdge->cols - 1;
    dftSize.height = ComplexPhotoEdge->rows + ComplexModelEdge->rows - 1;
    
    // Now we have to enlarge the dimesion of the original matrix padding with zeros
    Mat *DFTModelEdge = new Mat(dftSize, ComplexModelEdge->type(), Scalar::all(0));
    Mat *DFTPhotoEdge = new Mat(dftSize, ComplexPhotoEdge->type(), Scalar::all(0));
    
    //Coping the vector in the padeed matrix
    Mat roiDFTModelEdge(*DFTModelEdge, Rect(0,0,ComplexModelEdge->cols,ComplexModelEdge->rows)); //Selecting the area in to wich copy the original matrix
    ComplexModelEdge->copyTo(roiDFTModelEdge); // Padded MODEL vector matrix
    
    Mat roiDFTPhotoEdge(*DFTPhotoEdge, Rect(0,0,ComplexPhotoEdge->cols,ComplexPhotoEdge->rows)); //Selecting the area in to wich copy the original matrix
    ComplexPhotoEdge->copyTo(roiDFTPhotoEdge); // Padded PHOTO vector matrix

    // Make the dft of the vectroed edge map of the model
    cv::dft(*DFTModelEdge, *DFTModelEdge,CV_DXT_FORWARD);
    
    // Make the dft of the vectroed edge map of the photograph
    cv::dft(*DFTPhotoEdge, *DFTPhotoEdge, CV_DXT_FORWARD);
    
    // We now have to multiply between each other the two Discrete Fourier trasform of the vectored edge
    Mat *DFTVCC = new Mat(dftSize, DFTPhotoEdge->type(), Scalar::all(0)); // declaring the matrix to re allocate memory
    
    //Mat *DFTModelEdge1 = new Mat(dftSize, DFTPhotoEdge->type(), Scalar::all(0));
    //cv::mulSpectrums(*DFTModelEdge, *DFTModelEdge, *DFTModelEdge1, false); // Comupting the square of the spectrum
    //Mat *DFTPhotoEdge1 = new Mat(dftSize, DFTPhotoEdge->type(), Scalar::all(0));
    //cv::mulSpectrums(*DFTPhotoEdge, *DFTPhotoEdge, *DFTPhotoEdge1, false); // Comupting the square of the spectrum
    
    cv::mulSpectrums(*DFTModelEdge, *DFTPhotoEdge, *DFTVCC,DFT_ROWS,true);
    
    // To obtain the VCC (Vector cross correlation) we now need to go back in the original domain
    Mat *VCC = new Mat(); // Creating the necessary matrix
    cv::dft(*DFTVCC, *VCC,DFT_INVERSE + DFT_SCALE);
    
    // Separating the real and imaginary part of the VCC (We are only interested in the real part
    Mat planes[] = {Mat_<float>(), Mat::zeros(VCC->size(), CV_32F)};
    split(*VCC, planes);
    
    // Normalizing value between [0,1]
    *realVCC = Mat(planes[0].rows,planes[0].cols,CV_32FC1,Scalar(0));
    planes[0].copyTo(*realVCC);
    //SpaceReduction::makePeriod(VCC, planes[0]);
    
}

// Make the convolution periodic on the horizontal dimension and cut the unmeaningfull part on vertical direction
void SpaceReduction::makePeriod(Mat *output, Mat input)
{
    // Creating a matrix with the only the region of interest of the original image
    //Mat myROI(*input, Rect(0, 0, output->cols, output->rows));
    cout << output-> cols << endl;
    cout << output-> rows << endl;
    
    // Selecting the last part of VCC that must be copied at beginning in order to take in account of the fact that the VCC is horizontally periodic
    Mat periodicRep(input, Rect(output->cols,0,input.cols - output->cols,input.rows));
    for (int m = 0; m < periodicRep.rows; m++)
    {
        for (int n = 0;  n < periodicRep.cols; n++)
        {
            input.at<float>(m,n) += periodicRep.at<float>(m,n);
        }
    }
    
    // Cropping the VCC to select only the "valid area" a period
    Mat cropped(input,Rect(0,0,output->cols - 1,output->rows - 1));
    cropped.copyTo(*output);
}
