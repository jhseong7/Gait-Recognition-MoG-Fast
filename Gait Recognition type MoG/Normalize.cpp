#include "opencv_inc.h"

#define Standard_y 1000
#define Standard_x 1000
#define y_size 600

Mat Normalize(Mat input_image)
{
	int size_y = 300;	
	float rat;
	int new_size_x = 0, new_size_y = 0;

	rat = (float)size_y / input_image.rows;
	new_size_x = rat*input_image.cols;
	new_size_y = rat*input_image.rows;

	Mat new_image;

	resize(input_image, new_image, Size(), rat, rat);

	threshold(new_image, new_image, 100, 255, CV_THRESH_BINARY);

	return new_image;
}

Mat Match_center(Mat input_image)
{
	int i = 0, j = 0;

	int center_x = 0, center_y = 0;
	int comp_x = 0, comp_y = 0;

	comp_x = Standard_x / 2;  comp_y = Standard_y / 2;

	uchar intensity;

	//Initiate
	int left = input_image.cols;
	int right = 0;
	int up = input_image.rows;
	int down = 0;

	Mat new_image = Mat::zeros(Standard_y, Standard_x, input_image.type());

	for (i = 0; i<input_image.rows; i++)
	{
		for (j = 0; j<input_image.cols; j++)
		{
			intensity = input_image.at<uchar>(i, j);
			if (intensity == 255)
			{
				new_image.at<uchar>(i, j) = 255;
				if (i<up){ up = i; }
				else if (i>down){ down = i; }
				else{ ; }
				if (j<left){ left = j; }
				else if (j>right){ right = j; }
				else{ ; }
			}
		}
	}

	center_y = (left + right) / 2;
	center_x = (up + down) / 2;

	comp_x = abs(comp_x - center_x);
	comp_y = abs(comp_y - center_y);

	Mat result_image = Mat::zeros(Standard_y, Standard_x, input_image.type());

	for (i = 0; i<Standard_y; i++)
	{
		for (j = 0; j<Standard_x; j++)
		{
			intensity = new_image.at<uchar>(i, j);
			if (intensity == 255)
			{
				result_image.at<uchar>(i + comp_x, j + comp_y) = 255;
			}
		}
	}

	return result_image;
}
