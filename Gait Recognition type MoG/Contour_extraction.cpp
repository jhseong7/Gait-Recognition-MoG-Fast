#include "opencv_inc.h"
#include "Contour_extraction.h"

Mat Erosion(int erosion_element,int erosion_size, Mat input_image, void*);
Mat Dilation(int dilation_element,int dilation_size, Mat input_image,void*);
Mat Make_contour(Mat* input_image,vector<Point>* input_vector);
vector<Point> Find_contour_point(Mat input_image);

static int trackbarposition = 200;

void InitContourWindow()
{
	namedWindow("Contour data", WINDOW_AUTOSIZE);
	createTrackbar("Track bar contour Length", "Contour data", &trackbarposition, 1000, NULL);
}

Mat Contour(Mat* input_image,vector<Point>* contour_point)
{
    int erosion_elem = 0;   int erosion_size = 0;
    int dilation_elem = 0;  int dilation_size = 0;

    Mat image = *input_image;
	Mat out_image(image.rows,image.cols,CV_8UC1,Scalar(1));
	Mat temp(image.rows,image.cols,CV_8UC1,Scalar(1));
    
    //vector<vector<Point> >* Contour_vector;

	erosion_elem = 3;   erosion_size = 1;
	dilation_elem = 3;	dilation_size = 1;

	temp = Dilation(dilation_elem,dilation_size,image,0);
	temp = Dilation(dilation_elem,dilation_size,temp,0);
    temp = Dilation(dilation_elem,dilation_size,temp,0);
	temp = Erosion(erosion_elem,erosion_size,temp,0);
    
	out_image = Make_contour(&temp,contour_point);
    
    cvtColor(out_image,out_image,CV_BGR2GRAY);
    threshold(out_image, out_image, 100, 255, CV_THRESH_BINARY);
    //*contour_point = Find_contour_point(out_image);
    
	return out_image;
}

/**  Function Erosion  */
Mat Erosion( int erosion_elem, int erosion_size, Mat input_image, void* )
{
	  int erosion_type = 0;
	  if( erosion_elem  == 0 ){ erosion_type = MORPH_RECT; }
	  else if( erosion_elem  == 1 ){ erosion_type = MORPH_CROSS; }
	  else if( erosion_elem  == 2) { erosion_type = MORPH_ELLIPSE; }

	  static Mat element = getStructuringElement( erosion_type,Size( 2*erosion_size + 1, 2*erosion_size+1 ),Point( erosion_size, erosion_size ) );
	  Mat out_image;

	  /// Apply the erosion operation
	  erode( input_image,out_image, element );

	  return out_image;
}

/** Function Dilation */
Mat Dilation( int dilation_elem, int dilation_size, Mat input_image,  void* )
{
	  int dilation_type = 0;
	  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
	  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
	  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	  static Mat element = getStructuringElement(dilation_type, Size(2 * dilation_size + 1, 2 * dilation_size + 1), Point(dilation_size, dilation_size));
	  Mat out_image;	  

	  /// Apply the dilation operation
	  dilate( input_image, out_image, element );

	  return out_image;
}

Mat Make_contour(Mat* input_image,vector<Point>* input_vector)
{
	Mat canny_output;
    Mat temp((*input_image).size(),CV_8UC1,Scalar(0));
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
    RNG rng(12345); Scalar color;
    int i;
    int length;
    
    threshold(*input_image, canny_output, 100, 255, CV_THRESH_BINARY);
    findContours(canny_output,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point(0,0));
	
	Mat drawing = Mat::zeros(canny_output.size(),CV_8UC3);
    color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

	for(i=0;i<contours.size();i++)
	{
        length = arcLength(contours[i], 1);
		if (length > trackbarposition )
        {
            //cout << contours[i] << endl;
            drawContours(drawing,contours,i,color,1,8,hierarchy,2,Point());
		}

    }
    

	(*input_vector) = contours[0];
	
    return drawing;
}

void ContourBasedFilter(Mat* output_image, Mat* input_image) 
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345); Scalar color;
	int length;

	threshold(*input_image, canny_output, 100, 255, CV_THRESH_BINARY);
	findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	*output_image = Mat::zeros(canny_output.size(), CV_8UC1);
	color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

	int Max_Length_Index = 0;
	int Max_Length = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		length = arcLength(contours[i], 1);
		
		if (Max_Length < length && length > trackbarposition)
		{
			Max_Length_Index = i;
			Max_Length = length;
		}
		
	}

	drawContours(*output_image, contours, Max_Length_Index, color, 1, 8, hierarchy, 2, Point());

	Point maxP, minP;

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

	for (int x = minP.x; x < maxP.x; x++)
	{
		for (int y = minP.y ; y < maxP.y; y++)
		{
			output_image->data[y*step_y + x*step_x] = input_image->data[y*step_y + x*step_x];
		}
	}
	



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
