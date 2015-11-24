
#include "opencv_inc.h"
#include "Resampling.hpp"

#define DEFAULT_INIT 0

int sample_point = 40;

vector<vector<Point> > Resampling(vector<Point>* input_contour, vector<Point>* refer_point)
{
    int i,j; int size = (*input_contour).size(); Point temp;
    
    Point Head = (*refer_point)[0]; Point Left_foot = (*refer_point)[1]; Point Right_foot = (*refer_point)[2];
    int Head_num=0,Right_foot_num=0,Left_foot_num=0;
    
    vector<Point> Head_start_contour(size);
    
    int Seg1_length=DEFAULT_INIT;            int Seg2_length=DEFAULT_INIT;           int Seg3_length=DEFAULT_INIT;
    int Seg1_sample_point=15;   int Seg2_sample_point=10;  int Seg3_sample_point=15;
    
    for(i=0;i<size;i++)
    {
        temp = (*input_contour)[i];
        if(temp==Head){Head_num = i;}
        else if(temp==Right_foot){Right_foot_num=i;}
        else if(temp==Left_foot){Left_foot_num=i;}
        else{;};
    }
    
    if(Head_num > Right_foot_num)
    {
        Seg2_length = Right_foot_num - Left_foot_num;
        Seg3_length = Head_num - Right_foot_num;
        for(j=Head_num;j<size;j++){Head_start_contour[j-Head_num] = (*input_contour)[j];}
        for(j=0;j<Head_num;j++){Head_start_contour[j+(size-Head_num)] = (*input_contour)[j];}
        Right_foot_num = Right_foot_num + (size - Head_num);
        Left_foot_num = Left_foot_num + (size - Head_num);
        Head_num = 0;
        Seg1_length = Left_foot_num;
    }
    else if(Head_num < Right_foot_num)
    {
        Seg1_length = Left_foot_num - Head_num;
        Seg2_length = Right_foot_num - Left_foot_num;
        for(j=Head_num;j<size;j++){Head_start_contour[j-Head_num] = (*input_contour)[j];}
        for(j=0;j<Head_num;j++){Head_start_contour[j+(size-Head_num)] = (*input_contour)[j];}
        Right_foot_num = Right_foot_num - Head_num;
        Left_foot_num = Left_foot_num - Head_num;
        Head_num = 0;
        Seg3_length = size - Right_foot_num;
    }
    
    double Seg1_inter_length = ((double)Seg1_length/(double)Seg1_sample_point);
    double Seg2_inter_length = ((double)Seg2_length/(double)Seg2_sample_point);
    double Seg3_inter_length = ((double)Seg3_length/(double)Seg3_sample_point);
    
    int Whole_point_num = Seg1_sample_point + Seg2_sample_point + Seg3_sample_point;
    int index1=0,index2=0,index3=0;
    
    vector<Point> Segment1(Seg1_sample_point); vector<Point> Segment2(Seg2_sample_point); vector<Point> Segment3(Seg3_sample_point);
    vector<vector<Point> > result;
    
    int k;
    
    for(k=0;k<Seg1_sample_point;k++)
    {
        index1 = (int)k*Seg1_inter_length;
        Segment1[k] = Head_start_contour[index1];
        temp = Segment1[k];
        //cout << index1 << ":" << temp << endl;
    }
    for(k=0;k<Seg2_sample_point;k++)
    {
        index2 = Seg1_length + (int)k*Seg2_inter_length;
        Segment2[k] = Head_start_contour[index2];
        temp = Segment2[k];
       // cout << index2 << ":" << temp << endl;
    }
    for(k=0;k<Seg3_sample_point;k++)
    {
        index3 = (Seg1_length+Seg2_length) + (int)k*Seg3_inter_length;
        Segment3[k] = Head_start_contour[index3];
        temp = Segment3[k];
       // cout << index3 << ":" << temp << endl;
    }
    
    result.push_back(Segment1);    result.push_back(Segment2);    result.push_back(Segment3);
    
    return result;
}

Mat Draw_Resampling(vector<vector<Point> > Segment_array,Mat Size_refe_image)
{
    int Segment_size = Segment_array.size();    int array_size=0;
    Mat Sampling_image(Size_refe_image.size(),CV_8UC3,Scalar(0));
    
    int i,j;
    
    Point temp;
    
    array_size = Segment_array[0].size();
    for(j=0;j<array_size;j++)
    {
        temp = Segment_array[0][j];
        Sampling_image.at<Vec3b>(temp.y,temp.x)[0] = 250;
        Sampling_image.at<Vec3b>(temp.y,temp.x)[1] = 250;
        Sampling_image.at<Vec3b>(temp.y,temp.x)[2] = 250;
    }
    array_size = Segment_array[1].size();
    for(j=0;j<array_size;j++)
    {
        temp = Segment_array[1][j];
        Sampling_image.at<Vec3b>(temp.y,temp.x)[2] = 255;
    }
    array_size = Segment_array[2].size();
    for(j=0;j<array_size;j++)
    {
        temp = Segment_array[2][j];
        Sampling_image.at<Vec3b>(temp.y,temp.x)[1] = 255;
        
        
    }
    
    return Sampling_image;
}