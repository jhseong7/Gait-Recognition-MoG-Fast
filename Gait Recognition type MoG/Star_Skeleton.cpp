#include "opencv_inc.h"

Mat Star_Skeleton(vector<vector<Point> >* input_vector,Mat* contour_image,double* leg_length_1, double* leg_length_2, double*gravity_center)
{
	// Make Local Variable contours
	vector<vector<Point> > contours = *input_vector;

	// Find the center using image moments
	int i,j;
	vector<Moments> mu(contours.size() );
	vector<Point2f> mc(1);
	int belly_start = (float)contours.size()/3;
	// belly_start = (float)contours.size()/2;
    belly_start = 0;
	int belly_end = (float)2*contours.size()/3;
	belly_end = contours.size();

	for(i=belly_start;i<belly_end;i++)
	{	
		mu[i] = moments(contours[i],false);
		mc[0] += Point2f(mu[i].m10/mu[i].m00,mu[i].m01/mu[i].m00);	
	}
	mc[0] = mc[0]/(float)(belly_end-belly_start);

	/*********************************
	 * Make contour image with center
	 *********************************/ 
	
	// Make Local Variable temp
    Mat temp = Mat::zeros(contour_image->size(),CV_8UC3);
	temp = *contour_image;

	cvtColor(temp,temp,CV_BGR2GRAY);

	// Constant Variable
	uchar intensity;
	Point2f center; center.x = mc[0].x; center.y = mc[0].y;
	Point2f temp_point;
	float distance_1=0; float distance_2=0;
	float temp_dis=0.1;
	Point2f far_dis_1; Point2f far_dis_2;
	Point2f ground;
	ground.x = 0; ground.y = 0;

	for(i=(temp.rows/2);i<temp.rows;i++)
	{
		for(j=0;j<temp.cols/2;j++)
		{
			intensity = temp.at<uchar>(i,j);
			if(intensity != 0)
			{
				temp.at<uchar>(i,j) = 255;
				temp_point.x = j, temp_point.y = i;
				temp_dis=sqrt((center.x - (float)temp_point.x)*(center.x - (float)temp_point.x) + (center.y - (float)temp_point.y)*(center.y - (float)temp_point.y));
					
				if(temp_dis>distance_1)
				{
					far_dis_1.x = j;
					far_dis_1.y = i;
					distance_1 = temp_dis;
				}
				else if(temp_dis==distance_1)
				{
					far_dis_1.x = (far_dis_1.x+j)/2;
					far_dis_1.y = (far_dis_1.y+i)/2;
				}
				if(ground.y<i)
				{
					ground.x = j;
					ground.y = i;
				}
			}
		}
		for(j=temp.cols/2;j<temp.cols;j++)
		{
			intensity = temp.at<uchar>(i,j);
			if(intensity != 0)
			{
				temp.at<uchar>(i,j) = 255;
				temp_point.x = j, temp_point.y = i;
				temp_dis=sqrt((center.x - temp_point.x)*(center.x - temp_point.x) + (center.y - temp_point.y)*(center.y - temp_point.y));
					
				if(temp_dis>distance_2)
				{
					far_dis_2.x = j;
					far_dis_2.y = i;
					distance_2 = temp_dis;
				}
				else if(temp_dis==distance_2)
				{
					far_dis_2.x = (far_dis_2.x+j)/2;
					far_dis_2.y = (far_dis_2.y+i)/2;
				}

				if(ground.y<i)
				{
					ground.x = j;
					ground.y = i;
				}
			}
		}
	}

	cvtColor(temp,temp,CV_GRAY2BGR);
	
	Scalar color;
	RNG rng(12345);
	color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

	Point temp_p;
	temp_p.x = center.x; temp_p.y = ground.y;


	*leg_length_1 = distance_1;
	*leg_length_2 = distance_2;
	*gravity_center = ground.y - center.y;

	return temp;
}	
