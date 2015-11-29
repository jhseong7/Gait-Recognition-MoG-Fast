#include "opencv_inc.h"
#include "Preprocess_PMS.h"

vector< complex<float> > CSC(vector<vector<Point> > Segment_array)
{
    int Segment_size = (int)Segment_array.size();
    int array_size = 0;
    int Whole_array_num = 0;
    
    int i,j;
    
    Point2f center(0,0);
    Point2f temp;
    
    for(i=0;i<Segment_size;i++)
    {
        array_size = (int)Segment_array[i].size();
        Whole_array_num = Whole_array_num + array_size;
        for(j=0;j<array_size;j++)
        {
            temp = Segment_array[i][j];
            center.y = center.y + temp.y;
            center.x = center.x + temp.x;
        }
    }
    
    center.y = center.y/(float)Whole_array_num;
    center.x = center.x/(float)Whole_array_num;
    
    vector< complex<float> > CSC_array;
    complex<float> temp_complex;
    
    for(i=0;i<Segment_size;i++)
    {
        array_size = (int)Segment_array[i].size();
        for(j=0;j<array_size;j++)
        {
            temp.x = (float)Segment_array[i][j].x - (float)center.x;
            temp.y = (float)Segment_array[i][j].y - (float)center.y;
            temp_complex = {temp.x,temp.y};
            CSC_array.push_back(temp_complex);
        }
    }
    
    return CSC_array;
}

