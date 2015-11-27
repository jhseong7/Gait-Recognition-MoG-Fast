#include "Preprocess_PMS.h"
#include "opencv_inc.h"
#include <cstdio>

void Before_PMS(Mat input_image,Mat* Image_array,Mat* Cutted_image_array,Mat* Contour_out_image_array,Mat* Resampled_image_array,double* bounding_box_ratio, bool* Period_find, int* number_of_image, int* period);
void Get_PMS(int period);
bool Feature_make(Mat input_image);

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

int number_of_image = 0;
int period = -1;

Configure Final_result(Mat input_image)
{
	bool period_find = false;
	period_find = Feature_make(input_image);
	
	Configure result;
	result.period_ok = false;
	if (period_find == true)
	{
		Get_PMS(period);

		result.Segment1 = PMS_result_Segment[0];
		result.Segment2 = PMS_result_Segment[1];
		result.Segment3 = PMS_result_Segment[2];
		result.Segment4 = PMS_result_Segment[3];

		result.period_ok = true;
	}
	else
	{
		result.period_ok = false;
	}

	return result;
}


bool Feature_make(Mat input_image)
{
	double Bounding_box_ratio[100] = { 0, };
	bool Period_find = false;

	Before_PMS(input_image, Image_array, Cutted_image_array, Contour_out_image_array, Resampled_image_array, Bounding_box_ratio, &Period_find, &number_of_image, &period);

	return Period_find;
}



void Before_PMS(Mat input_image,Mat* Image_array,Mat* Cutted_image_array,Mat* Contour_out_image_array,Mat* Resampled_image_array, double* bounding_box_ratio, bool* Period_find, int* number_of_image, int* period)
{
    int height=0;   int width=0;
    int temp = *period;
	int num_image = *number_of_image;
	int temp_period;

    Image_array[num_image] = input_image;
    Cutted_image_array[num_image] = Cutting_silhouette_area(&Image_array[num_image], &height, &width);
    bounding_box_ratio[num_image] = (double)height/(double)width;
    Contour_out_image_array[num_image] = Contour(&(Cutted_image_array[num_image]), &contour_point_array);
    temp_period = Gait_period_cal(num_image, bounding_box_ratio);
    
    refer_point = Find_refer_point(contour_point_array);
    Segment = Resampling(&contour_point_array,&refer_point);
    Resampled_image_array[num_image] = Draw_Resampling(Segment,Contour_out_image_array[num_image]);
    
    CSC_array.push_back(CSC(Segment));
    
    (*number_of_image)++;
    
    if(temp_period>10){*Period_find = true;}
}

void Get_PMS(int period)
{
    PMS_result = PMS(CSC_array,period);
    PMS_result_Segment = Segmenting(PMS_result);
}
