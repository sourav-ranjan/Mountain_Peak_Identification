/*
 * histogram.hpp
 *
 *  Created on: 21/mar/2013
 *      Author: cardo89
 */

#ifndef HISTOGRAM_HPP_
#define HISTOGRAM_HPP_

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

using namespace cv;
using namespace std;

class Histogram
{
private:
	Mat histogram;
public:
	Mat static buildHistogramGray(Mat);
	Mat static buildHistogramColor(Mat);
	void static plotHistogramGray(Mat, int);
	Mat static equalizationGray(Mat);
	Mat static buildCumulative(Mat);
    Mat static equalizationColor(Mat);
};


#endif /* HISTOGRAM_HPP_ */

