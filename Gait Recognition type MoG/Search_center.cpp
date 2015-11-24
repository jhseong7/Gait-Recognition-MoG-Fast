#include "opencv_inc.h"
#include "Search_center.h"

void Search_center(Mat* Input_image,Coordinate* center)
{
	Mat Image = *Input_image;

	int row = Image.rows;
	int col = Image.cols;
	int i,j;
	int pixel_num=0;
	int x_sum = 0;
	int y_sum = 0;
	
	for(j=0;j<Image.rows;j++)
	{
		for(i=0;i<Image.cols;i++)
		{
			uchar intensity = Image.at<uchar>(j,i);
			if(intensity == 255)
			{
				pixel_num++;
				x_sum+=j;
				y_sum+=i;
			}
		}
	}

	printf("# row : %d, # col : %d \n",row,col);
	printf("pixel_num : %d, x_sum : %d, y_sum : %d \n",pixel_num,x_sum,y_sum);
	center->x = x_sum/pixel_num;
	center->y = y_sum/pixel_num;	
}

