#include "opencv_inc.h"
#include "Preprocess_PMS.h"

float procrustes_distance(complex<float> Figure_1,complex<float>Figure_2);

vector<vector<complex<float>> > Segmenting(vector<complex<float> > PMS_result)
{
    vector<vector<complex<float>> > PMS_segment(4);
    int Seg_ref_1=8;
    int Seg_ref_2=23;
    int Seg_ref_3=34;
    int Seg_ref_4 = (int)PMS_result.size();
    
    int i=0;
    
    for(i=0;i<Seg_ref_1;i++)
    {
        PMS_segment[0].push_back(PMS_result[i]);
    }
    for(i=Seg_ref_1;i<Seg_ref_2;i++)
    {
        PMS_segment[1].push_back(PMS_result[i]);
    }
    for(i=Seg_ref_2;i<Seg_ref_3;i++)
    {
        PMS_segment[2].push_back(PMS_result[i]);
    }
    for(i=Seg_ref_3;i<Seg_ref_4;i++)
    {
        PMS_segment[3].push_back(PMS_result[i]);
    }
    
    return PMS_segment;
}


float Gait_dissimilarity(vector<complex<float> > Gait_1, vector<complex<float> > Gait_2)
{
    int Gait_1_size = (int)Gait_1.size(); int Gait_2_size = (int)Gait_2.size();
    int Gait_size=0;
    float Whole_distance=0;
    int i;
    complex<float> temp_1;  complex<float> temp_2; complex<float> temp_3;
    
    if(!(Gait_1_size==Gait_2_size))
    {
        cout << "Number of feature points is different" << endl;
        abort();
    }
    
    Gait_size = Gait_1_size;

    for(i=0;i<Gait_size;i++)
    {
        Whole_distance += procrustes_distance(Gait_1[i], Gait_2[i]);
    }
    
    return Whole_distance;
}

float procrustes_distance(complex<float> Figure_1,complex<float>  Figure_2)
{
    complex<float> temp;
    float temp_mag_1;   float temp_mag_2;   float temp_mag_3;
    
    temp = (conj(Figure_1)*Figure_2);
    temp_mag_1 = norm(temp);
    temp_mag_2 = norm(Figure_1); temp_mag_3 = norm(Figure_2);
    
    float return_value;
    
    return_value = 1 - (temp_mag_1/(temp_mag_2*temp_mag_3));
    
    if(return_value<0){return_value=0;}
    
    return return_value;
}