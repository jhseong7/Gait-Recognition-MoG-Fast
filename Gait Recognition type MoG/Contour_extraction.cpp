#include "common.h"
//#include "opencv_inc.h"

Mat Erosion(int erosion_element,int erosion_size, Mat input_image, void*);
Mat Dilation(int dilation_element,int dilation_size, Mat input_image,void*);
//Mat Make_contour(Mat* input_image,vector<Point>* input_vector);
vector<Point> Find_contour_point(Mat input_image);

Mat Contour(Mat* input_image,vector<Point>* contour_point)
{
    int erosion_elem = 0;   int erosion_size = 0;
    int dilation_elem = 0;  int dilation_size = 0;

    Mat image = *input_image;
	Mat out_image(image.rows,image.cols,CV_8UC1,Scalar(1));
	Mat temp(image.rows,image.cols,CV_8UC1,Scalar(1));

    // 모폴로지 사이즈 결정
	erosion_elem = 2;   erosion_size = 2;
	dilation_elem = 2;	dilation_size = 2;

    // 모폴로지
	temp = Dilation(dilation_elem,dilation_size,image,0);
	temp = Dilation(dilation_elem,dilation_size,temp,0);
    temp = Dilation(dilation_elem,dilation_size,temp,0);
	temp = Erosion(erosion_elem,erosion_size,temp,0);
    
    // 컨투어 생성
	//Mat rrr;

	//ContourBasedFilter(&out_image, &temp, &rrr);

	out_image = Make_contour(&temp,contour_point);
    
    // 리턴 할 이미지의 이진화
    cvtColor(out_image,out_image,CV_BGR2GRAY);
    threshold(out_image, out_image, 100, 255, CV_THRESH_BINARY);
    
	return out_image;
}

/**  Function Erosion  */
Mat Erosion( int erosion_elem, int erosion_size, Mat input_image, void* )
{
	  int erosion_type;
	  if( erosion_elem  == 0 ){ erosion_type = MORPH_RECT; }
	  else if( erosion_elem  == 1 ){ erosion_type = MORPH_CROSS; }
	  else if( erosion_elem  == 2) { erosion_type = MORPH_ELLIPSE; }

	  Mat element = getStructuringElement( erosion_type,Size( 2*erosion_size + 1, 2*erosion_size+1 ),Point( erosion_size, erosion_size ) );
	  Mat out_image;

	  /// Apply the erosion operation
	  erode( input_image,out_image, element );

	  return out_image;
}

/** Function Dilation */
Mat Dilation( int dilation_elem, int dilation_size, Mat input_image,  void* )
{
	  int dilation_type;
	  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
	  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
	  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	  Mat element = getStructuringElement( dilation_type,Size( 2*dilation_size + 1, 2*dilation_size+1 ),Point( dilation_size, dilation_size ) );
	  Mat out_image;	  

	  /// Apply the dilation operation
	  dilate( input_image, out_image, element );

	  return out_image;
}

/*
Mat Make_contour(Mat* input_image,vector<Point>* input_vector)
{
	Mat canny_output;
    Mat temp((*input_image).size(),CV_8UC1,Scalar(0));
	vector<vector<Point> > contours;
    vector<vector<Point> > output_contours;
	vector<Vec4i> hierarchy;
    RNG rng(12345); Scalar color;
    int i=0;
    int length=0;

	threshold(*input_image, canny_output, 70, 255, CV_THRESH_BINARY);
    // Threshold를 통한 이진화
	findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
	
	Mat drawing = Mat::zeros(canny_output.size(),CV_8UC3);
    color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

	for(i=0;i<contours.size();i++)
	{
        length = arcLength(contours[i], 1);
        if(length>100)
        {
            output_contours.push_back(contours[i]);
            drawContours(drawing,contours,i,color,1,8,hierarchy,2,Point());
        }
    }
    
	(*input_vector) = output_contours[0];
	
    return drawing;
}

*/