#ifndef COMMON_H__
#define COMMON_H__


#include <Eigen/Dense>
#include <opencv2\opencv.hpp>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/video/background_segm.hpp>  
#include <opencv2/videoio.hpp>
#include <opencv2\core\ocl.hpp>

#include <Windows.h>

//Feature Extraction - Yong Joo SulChan code header

using namespace cv;
using namespace std;


#include "Cutting_silhouette_area.h"
#include "Contour_extraction.h"
#include "Gait_period_cal.hpp"
#include "Resampling.hpp"
#include "Find_refer_point.hpp"

bool CheckEmpty(Mat* Input_Image);
void ShowSteadyContour(Mat* Output_Image, Mat* BW_Contour, Mat* Resize_Points);

//fast BGsub
void FastBGSubtract(void);
void FastBGSubtract_NonCL(void);


//Contour
Mat contour(Mat* input_image, vector<vector<Point> >* contour_vector);
void ContourBasedFilter(Mat* output_image, Mat* input_image);
void InitContourWindow();

//Masking
void ImageMask(Mat* Output_Image, Mat* Input_Image, Mat* Mask_Image);


#endif