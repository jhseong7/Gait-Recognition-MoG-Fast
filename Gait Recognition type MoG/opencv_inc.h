//
//  OpenCV_essential.h
//  Read_Data_&_Recognition
//
//  Created by ?´ìš©ì£?on 2015. 11. 25..
//  Copyright Â© 2015??Raymond. All rights reserved.
//

#ifndef OpenCV_essential_h
#define OpenCV_essential_h

#include "common.h";

using namespace cv;
using namespace std;
using namespace Eigen;

struct Configure
{
    vector<complex<float>> Segment1;
    vector<complex<float>> Segment2;
    vector<complex<float>> Segment3;
    vector<complex<float>> Segment4;
	bool period_ok;
};

struct Data_set
{
    string name;
    vector<Configure> Segmented_Configure;
    int Configure_size;
    float Segment1_Weight;
    float Segment2_Weight;
    float Segment3_Weight;
    float Segment4_Weight;
};

float Gait_dissimilarity(vector<complex<float> > Gait_1, vector<complex<float> > Gait_2);

string Recognition(vector<Data_set> Training_data, Configure Input_data);
Configure Final_result(Mat input_image,int* period_final);

void ResetRecognition();
string Train_main(Mat Input_image, vector<Data_set>* Training_data);

vector<Data_set> Read_training_data(string File_path);

#endif /* OpenCV_essential_h */
