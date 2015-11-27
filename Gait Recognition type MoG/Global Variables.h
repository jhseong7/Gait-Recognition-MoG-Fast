#ifndef GLOBAL_VAR_H__
#define GLOBAL_VAR_H__
#include <opencv2\opencv.hpp>

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
#endif