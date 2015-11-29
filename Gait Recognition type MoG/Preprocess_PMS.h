
#ifndef Preprocess_PMS_h
#define Preprocess_PMS_h

#include "opencv_inc.h"

//vector<vector<complex<float>> > Preprocess_PMS(string identity,int identity_number);

Mat Contour(Mat* input_image,vector<Point>* contour_point);
Mat Cutting_silhouette_area(Mat* input_image,int* height,int* width);

void Image_number(int* number_of_image, string Identity, int Identity_num);

int Gait_period_cal(int array_number, double* Ratio_array);

vector< complex<float> > CSC(vector<vector<Point> > Segment_array);

vector<complex<float> > PMS(vector<vector<complex<float> >> CSC_array,int period_size);

vector<vector<Point> > Resampling(vector<Point> * input_contour,vector<Point>* refer_point);
Mat Draw_Resampling(vector<vector<Point> > Segment_array,Mat Size_refe_image);

vector<Point> Find_refer_point(vector<Point> contour_point);

float Gait_dissimilarity(vector<complex<float> > Gait_1, vector<complex<float> > Gait_2);
vector<vector<complex<float>> > Segmenting(vector<complex<float> > PMS_result);

//vector<Data_set> Weight_cal(vector<string> Name,int name_num,vector<int> configure_size);

#endif /* Preprocess_PMS_h */
