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

//GraphCut
extern cv::Mat Silhouette_Final;
extern cv::Mat Silhouette_Track;


extern cv::Mat Current_Frame;
extern cv::Mat BackgroundMOG;
extern cv::Mat HSV_Image, HSV_Background;
extern int Rows, Cols;

extern cv::UMat CL_Current_Frame;
extern cv::UMat CL_Background_Frame;

extern int Walk_Direction;
extern cv::Point Previous_Point;

extern int frame_no;

extern int Direction_Tally[2];

extern cv::Point maxP, minP;
extern cv::Point maxSize;

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
