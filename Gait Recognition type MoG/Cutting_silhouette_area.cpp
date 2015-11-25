#include "opencv_inc.h"

Mat Cutting_silhouette_area(Mat* input_image,int* height,int* width)
{
    //Variable
	Mat image = *input_image;
	uchar intensity;
    
    //Index
    int i,j;
    
    //Initiate
	int left=image.cols;
	int right=0;
	int up=image.rows;
	int down=0;

	bool IsNoData = true;

    //////////////////////////////////////////
    /** 이미지를 훓으면서 Intensity가 0이 아닌 경우를 찾고
        해당 픽셀의 좌표가 기존의 좌우경계 값보다 작거나 큰 경우
        해당 픽셀의 좌표로 대체 한다. **/
    //////////////////////////////////////////

	for(i=0;i<image.rows;i++)
	{
		for(j=0;j<image.cols;j++)
		{
			intensity = image.at<uchar>(i,j);
			if(intensity!=0)
			{
				if(i<up){up=i;}
				else if(i>down){down=i;}
				else{;}
				if(j<left){left=j;}
				else if(j>right){right=j;}
				else{;}
			}
		}
	}

	

	int sil_height = down-up;
	int sil_width = right - left;

    //Rect 함수를 사용해서 해당 부분만을 잘라낸다.
    Rect rect(left,up,sil_width,sil_height);
	Mat out_image = image(rect);
	copyMakeBorder(out_image,out_image,10,10,10,10,BORDER_CONSTANT,0);
    
    //Ratio 계산을 위해 Height,Width를 반환한다. 
	*height = sil_height;
    *width = sil_width;
	return out_image;
}
