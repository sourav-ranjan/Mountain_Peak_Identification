//
//  realityAugmentation.cpp
//  ProgettoDIP
//
//  Created by Riccardo Fona on 12/10/13.
//  Copyright (c) 2013 Riccardo Fona. All rights reserved.
//

#include "realityAugmentation.h"
#include "commonFunction.h"

#ifdef __APPLE__ //&& __MACH__
#include <opencv2/opencv.hpp>
#elif defined __gnu_linux__
#include <opencv2/opencv.hpp>
#endif

void realityAugmentation::setPeakName(Mat *model, Mat *photo, Mat *VCC, Mat *label, Mat* output)
{
    /* *******************************DA RIMUOVERE***************************** */
    // PROVO A RIMUOVERE LA PARTE DI VCC CHE VA FUORI DALLA DIMENSIONE DEL PANORAMA
    Mat *croppedVCC = new Mat(model->rows,model->cols,VCC->type(),Scalar::all(0));
    commonFunction::crop(VCC, croppedVCC);
    //commonFunction::normalize(croppedVCC, croppedVCC);
    /* ************************************************************************** */
    // Extracting Peaks
    //Mat peaks = commonFunction::findPeaks(*croppedVCC, 5, 13);
    Mat peaks = commonFunction::coloredPeaks(*croppedVCC, *croppedVCC, 13, 10);
    Point translation = realityAugmentation::extractLoacation(croppedVCC, photo); // Estimating the translaton needed for the correct overlay of picture and model
    commonFunction::overlay(model, photo, output, translation); // Superimposing the model and the picture
    //commonFunction::overlay(output, label, output); // superimposing the name of the peaks
    commonFunction::overlay(output, &peaks, output);
    Point vertex;
    vertex.x = photo->cols;
    vertex.y = photo->rows;
    rectangle(*output, translation, translation+vertex,(255,0,0), 2);
    cout << "VCC size" << endl;
    cout << VCC->cols << endl;
    cout << VCC->rows << endl;
    //commonFunction::writeImage(croppedVCC, "/Users/cardo89/Dropbox/Universita/Magistrale/DIgital image and visualization/Progetto/Presentazione/ImmaginiTest/SyntheticModel/VCCPeriodized.jpg");
}

Point realityAugmentation::extractLoacation(Mat *VCC, Mat *photo)
{
    Point maxVCC;
    maxVCC.x = 0;
    maxVCC.y = 0;
    // Computing the maximum value
    float max = *max_element(VCC->begin<float>(), VCC->end<float>());
    // Computing the minimum Value
    float min = *min_element(VCC->begin<float>(), VCC->end<float>());

    for (int m = 0; m < VCC->rows; m++)
    {
        for (int n = 0; n < VCC->cols; n++)
        {
            if (VCC->at<float>(m,n) == max) // The VCC is normalized so the maximus should be alway one
            {
                maxVCC.x = n;//  + 2*photo->cols;
                maxVCC.y = m;//  + photo->rows;
            }
        }
    }
    cout << endl;
    cout << "minimum value of VCC:  ";
    cout << min << endl;
    cout << "maximum value of VCC:  ";
    cout << max << endl;
    cout << "trasnlastions value:" << endl;
    cout << "rows translation:   ";
    cout << maxVCC.y << endl;
    cout << "cols translation:   ";
    cout << maxVCC.x << endl;
    return maxVCC;
}
