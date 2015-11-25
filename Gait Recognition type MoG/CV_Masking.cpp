#include "common.h"

#define MASK_CHANNEL 1
#define IMAGE_CHANNEL 3


void ImageMask(Mat* Output_Image ,Mat* Input_Image, Mat* Mask_Image) // 
{
	for (int x = 0; x < Cols; x++)
	{
		for (int y = 0; y < Rows; y++)
		{
			Output_Image->data[(y*Cols + x)*IMAGE_CHANNEL + 0] = Input_Image->data[(y*Cols + x)*IMAGE_CHANNEL + 0] & Mask_Image->data[y*Cols + x];
			Output_Image->data[(y*Cols + x)*IMAGE_CHANNEL + 1] = Input_Image->data[(y*Cols + x)*IMAGE_CHANNEL + 1] & Mask_Image->data[y*Cols + x];
			Output_Image->data[(y*Cols + x)*IMAGE_CHANNEL + 2] = Input_Image->data[(y*Cols + x)*IMAGE_CHANNEL + 2] & Mask_Image->data[y*Cols + x];
		}
	}

}