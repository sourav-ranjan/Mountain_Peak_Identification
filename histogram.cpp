/*
 * histogram.cpp
 *
 *  Created on: 21/mar/2013
 *      Author: cardo89
 */

#include <iostream>

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

#include "histogram.hpp"

using namespace cv;
using namespace std;

// Build the histogram associated to an image
Mat Histogram::buildHistogramGray(Mat input)
{
	// define the histogram vector (poi modificalo..)
	Mat histogram(1,256, CV_32FC1, Scalar(0));
	//float histogram[1][256];

	// Filling the histogram
	for (int x = 0; x < input.rows; x++)
	{
		for (int y = 0; y < input.cols; y++)
		{
			histogram.at<float>(0,input.at<uchar>(x,y))+=1.0;
		}
	}

	// Normalizing the histogram
	histogram = histogram/(input.rows*input.cols);
	return histogram;
}

// plot the histogram
void Histogram::plotHistogramGray(Mat histogram, int n)
{
	// Define image for plot
	Mat plot(600,n*histogram.cols, CV_8UC1, Scalar(0));

	// Find the max value of pixel color
	float max = *max_element(histogram.begin<float>(),histogram.end<float>());
	for(int x = 0; x < plot.cols; x++)
	{
		for(int y = 0; y < (int)(((histogram.at<float>(0,(x/n)))*plot.rows)/max); y++)
		{
			plot.at<uchar>(plot.rows - y,x) = (uchar)255;
		}
	}
	imshow("histogram",plot);
}

// take as input the histogram and build the relative cumulative function
Mat Histogram::buildCumulative(Mat input)
{
	Mat cumulative(input.rows,input.cols, CV_32FC1, Scalar(0));
	for(int i = 0; i < input.cols; i++)
	{
		for(int j = 0; j <= i; j++)
		cumulative.at<float>(0,i) = cumulative.at<float>(0,i) + input.at<float>(0, j);
	}
	return cumulative;
}

Mat Histogram::equalizationGray(Mat input)
{
	Mat output(input.rows,input.cols, CV_8UC1, Scalar(0));
    //Mat output(input.rows,input.cols, input.type(), Scalar(0));
	
    // buliding the histogram
	Mat histogram = buildHistogramGray(input);
	//Building the cumulative
	Mat cumulative = buildCumulative(histogram);
	cumulative = cumulative*255;
	for (int y = 0; y < input.cols; y++)
	{
		for (int x = 0; x < input.rows; x++)
		{
			output.at<uchar>(x,y) = cumulative.at<float>(0,input.at<uchar>(x,y));
		}
	}

	return output;
}

Mat Histogram::equalizationColor(Mat inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat ycrcb;
        
        cvtColor(inputImage,ycrcb,CV_BGR2YCrCb);
        
        vector<Mat> channels;
        split(ycrcb,channels);
        
        equalizeHist(channels[0], channels[0]);
        
        Mat result;
        merge(channels,ycrcb);
        
        cvtColor(ycrcb,result,CV_YCrCb2BGR);
        
        return result;
    }
    return Mat();
}
