#ifndef COMMON_H__
#define COMMON_H__

#include "Global Variables.h"

#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ccomplex>
#include <fstream>

#include <Windows.h>

//Feature Extraction - Yong Joo SulChan code header

using namespace cv;
using namespace std;




bool CheckEmpty(Mat* Input_Image);
void ShowSteadyContour(Mat* Output_Image, Mat* BW_Contour, Mat* Resize_Points);

//fast BGsub
void FastBGSubtract(void);
void FastBGSubtract_NonCL(void);


//Contour
Mat Make_contour(Mat* input_image, vector<Point>* input_vector);
void ContourBasedFilter(Mat* output_image, Mat* input_image, int* Longest_Contour_Length);

//Masking
void ImageMask(Mat* Output_Image, Mat* Input_Image, Mat* Mask_Image);


#endif