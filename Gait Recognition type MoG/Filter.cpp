#include "common.h"

//#include "opencv_inc.h"

#define DELTA_MAX 20

//포인트 지점
Point maxP, minP;
Point maxSize;

static int trackbarposition = 200;


Mat Make_contour(Mat* input_image, vector<Point>* input_vector)
{
	Mat canny_output;
	Mat temp((*input_image).size(), CV_8UC1, Scalar(0));
	vector<vector<Point> > contours;
	vector<vector<Point> > output_contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345); Scalar color;
	int i = 0;
	int length = 0;

	threshold(*input_image, canny_output, 70, 255, CV_THRESH_BINARY);
	// Threshold를 통한 이진화
	findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

	for (i = 0; i<contours.size(); i++)
	{
		length = arcLength(contours[i], 1);
		if (length>100)
		{
			output_contours.push_back(contours[i]);
			drawContours(drawing, contours, i, color, 1, 8, hierarchy, 2, Point());
		}
	}

	(*input_vector) = output_contours[0];

	return drawing;
}

void ContourBasedParameterCalc(Mat* input_image, int* Longest_Contour_Length)
{
	Mat Thresholded;
	Mat ContourImage;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345); Scalar color;
	int length;

	threshold(*input_image, Thresholded, 70, 255, CV_THRESH_BINARY);
	findContours(Thresholded, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	ContourImage = Mat::zeros(Thresholded.size(), CV_8UC1);
	color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

	int Max_Length_Index = 0;
	*Longest_Contour_Length = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		length = arcLength(contours[i], 1);

		if ((*Longest_Contour_Length) < length && length > trackbarposition)
		{
			Max_Length_Index = i;
			(*Longest_Contour_Length) = length;
		}
		
	}

	drawContours(ContourImage, contours, Max_Length_Index, color, 1, 8, hierarchy, 2, Point());


	int row = input_image->rows;
	int col = input_image->cols;

	maxP.x = 0;		maxP.y = 0;
	minP.x = col;	minP.y = row;

	int step_y = ContourImage.step[0];
	int step_x = ContourImage.step[1];
	uchar contourpixel = 0;

	for (int x = 0; x < col; x++)
	{
		for (int y = 0; y < row; y++)
		{
			contourpixel = ContourImage.data[y*step_y + x*step_x];
			if (contourpixel != 0)
			{
				if (x > maxP.x)
					maxP.x = x;
				if (y > maxP.y)
					maxP.y = y;

				if (x < minP.x)
					minP.x = x;
				if (y < minP.y)
					minP.y = y;

			}
		}
	}

	Point Current_Point = (minP + maxP) / 2;


	if (Current_Point.x >Previous_Point.x)
	{
		Direction_Tally[1]++;
		Direction_Tally[0] = 0;
	}

	else if (Current_Point.x < Previous_Point.x)
	{
		Direction_Tally[0]++;
		Direction_Tally[1] = 0;
	}

	if (abs(Current_Point.x - Previous_Point.x) >DELTA_MAX)
		maxSize.x = 0;

	if (abs(Current_Point.y - Previous_Point.y) >DELTA_MAX)
		maxSize.y = 0;

	Previous_Point = Current_Point;

	//박스의  최대 크기 기록
	Point BoxSize = maxP - minP;

	if (BoxSize.x > maxSize.x)
		maxSize.x = BoxSize.x;

	if (BoxSize.y > maxSize.y)
		maxSize.y = BoxSize.y;

}

void ContourBasedFilter(Mat* output_image, Mat* input_image, int* Longest_Contour_Length)
{
	Mat Thresholded;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345); Scalar color;
	int length;

	threshold(*input_image, Thresholded, 70, 255, CV_THRESH_BINARY);
	findContours(Thresholded, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
	
	*output_image = Mat::zeros(Thresholded.size(), CV_8UC1);
	color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

	int Max_Length_Index = 0;
	*Longest_Contour_Length = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		length = arcLength(contours[i], 1);
		
		if ((*Longest_Contour_Length) < length && length > trackbarposition)
		{
			Max_Length_Index = i;
			(*Longest_Contour_Length) = length;
		}
		
	}

	drawContours(*output_image, contours, Max_Length_Index, color, 1, 8, hierarchy, 2, Point());


	int row = input_image->rows;
	int col = input_image->cols;

	maxP.x = 0;		maxP.y = 0;
	minP.x = col;	minP.y = row;

	int step_y = output_image->step[0];
	int step_x = output_image->step[1];
	uchar contourpixel = 0;

	for (int x = 0; x < col; x++)
	{
		for (int y = 0; y < row; y++)
		{
			contourpixel = output_image->data[y*step_y + x*step_x];
			if (contourpixel != 0)
			{
				if (x > maxP.x)
					maxP.x = x;
				if (y > maxP.y)
					maxP.y = y;

				if (x < minP.x)
					minP.x = x;
				if (y < minP.y)
					minP.y = y;

			}
		}
	}

	Point Current_Point = (minP + maxP) / 2;
	

	for (int x = minP.x; x < maxP.x; x++)
	{
		for (int y = minP.y ; y < maxP.y; y++)
		{
			output_image->data[y*step_y + x*step_x] = input_image->data[y*step_y + x*step_x];
		}
	}
	
	if (Current_Point.x >Previous_Point.x)
	{
		Direction_Tally[1]++;
		Direction_Tally[0] = 0;
	}

	else if (Current_Point.x < Previous_Point.x)
	{
		Direction_Tally[0]++;
		Direction_Tally[1] = 0;
	}

	if (abs(Current_Point.x - Previous_Point.x) >DELTA_MAX)
		maxSize.x = 0;

	if (abs(Current_Point.y - Previous_Point.y) >DELTA_MAX)
		maxSize.y = 0;

	Previous_Point = Current_Point;

		//박스의  최대 크기 기록
	Point BoxSize = maxP - minP;

	if (BoxSize.x > maxSize.x)
		maxSize.x = BoxSize.x;

	if (BoxSize.y > maxSize.y)
		maxSize.y = BoxSize.y;
	
}

/*
vector<Point> Find_contour_point(Mat input_image)
{
    Mat image(input_image.size(),CV_8UC1,Scalar(0));
    image = input_image;
    Mat temp_image(input_image.size(),CV_8UC1,Scalar(0));
    
    int i,j;
    int row_size = input_image.rows;    int col_size = input_image.cols;
    Scalar intensity;
    vector<Point> Contour_point; Point temp;
    
    for(i=0;i<row_size;i++)
    {
        for(j=0;j<col_size;j++)
        {
            intensity = image.at<uchar>(i,j);
            if(intensity.val[0]==255)
            {
                temp.x = j; temp.y = i;
                Contour_point.push_back(temp);
                temp_image.at<uchar>(i,j) = 255;
                imshow("temp",temp_image);
                waitKey();
            }
        }
    }
    
    return Contour_point;
}
*/
