#include "Preprocess_PMS.h"
#include "opencv_inc.h"
#include "Global Variables.h"
#include <cstdio>

void Before_PMS(Mat input_image,Mat* Image_array,Mat* Cutted_image_array,Mat* Contour_out_image_array,Mat* Resampled_image_array, int* number_of_image);
void Get_period(bool* Period_find, int * number_of_image);
void Get_PMS(int period);

Mat* Image_array = new Mat[100];
Mat* Cutted_image_array = new Mat[100];
Mat* Contour_out_image_array = new Mat[100];
Mat* Resampled_image_array = new Mat[100];

vector<Point> contour_point_array;
vector<Point> refer_point;
vector<vector<Point> > Segment;
vector<vector<complex<float> >> CSC_array;
vector<complex<float> > PMS_result;
vector<vector<complex<float>> > PMS_result_Segment;
vector<double> bounding_box_ratio;

int number_of_image = 0;
int period = -1;
bool period_find = false;

Configure Final_result(Mat input_image,int* period_final)
{
	Configure result;
	result.period_ok = false;

	Before_PMS(input_image, Image_array, Cutted_image_array, Contour_out_image_array, Resampled_image_array, &number_of_image);
	
	if (period_find == false)
	{
		Get_period(&period_find, &number_of_image);
	}
	else if (period_find == true)
	{
		if (number_of_image == period)
		{
			Get_PMS(period);

			result.Segment1 = PMS_result_Segment[0];
			result.Segment2 = PMS_result_Segment[1];
			result.Segment3 = PMS_result_Segment[2];
			result.Segment4 = PMS_result_Segment[3];
			
			result.period_ok = true;
			(*period_final) = period;

			return result;
		}
	}
	else
	{
		result.period_ok = false;
	}

	number_of_image++;

	return result;
}


/*
bool Feature_make(Mat input_image)
{
	bool Period_find = false;

	Before_PMS(input_image, Image_array, Cutted_image_array, Contour_out_image_array, Resampled_image_array, &Period_find, &number_of_image);

	return Period_find;
}
*/


void Before_PMS(Mat input_image,Mat* Image_array,Mat* Cutted_image_array,Mat* Contour_out_image_array,Mat* Resampled_image_array, int* number_of_image)
{
    int height=0;   int width=0;
	int num_image = *number_of_image;

    Image_array[num_image] = input_image;
    Cutted_image_array[num_image] = Cutting_silhouette_area(&Image_array[num_image], &height, &width);
    bounding_box_ratio.push_back((double)height/(double)width);
    Contour_out_image_array[num_image] = Contour(&(Cutted_image_array[num_image]), &contour_point_array);
   
    refer_point = Find_refer_point(contour_point_array);
    Segment = Resampling(&contour_point_array,&refer_point);
    Resampled_image_array[num_image] = Draw_Resampling(Segment,Contour_out_image_array[num_image]);
    
    CSC_array.push_back(CSC(Segment));

    //(*number_of_image)++; 
}

void Get_period(bool* Period_find,int * number_of_image)
{
	int num_image = *number_of_image;
	int temp = period;
	int temp_period;

	if (num_image > 10)
	{
		temp_period = Gait_period_cal(num_image, bounding_box_ratio);
		period = temp_period;
		if (!(temp_period == -1))
		{
			if (temp == temp_period)
			{
				*Period_find = true;
				period = temp_period;
			}
		}
		else if (temp_period == -1){ *Period_find = false; }
	}
}

void Get_PMS(int period)
{
    PMS_result = PMS(CSC_array,period);
    PMS_result_Segment = Segmenting(PMS_result);
}
