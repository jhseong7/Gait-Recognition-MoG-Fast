//
//  Recognition.cpp
//  Read_Data_&_Recognition
//
//  Created by 이용주 on 2015. 11. 26..
//  Copyright © 2015년 Raymond. All rights reserved.
//

#include "opencv_inc.h"
#include <stdio.h>

string Recognition(vector<Data_set> Training_data, Configure Input_data)
{
    int Identity_size = (int)Training_data.size();
    int Identity_conf_size=0;
    int i,j;
    float Segment1_dis; float Segment2_dis; float Segment3_dis; float Segment4_dis;
    float Weight_total;
    float Dissimilarity=0;
    float Min_dis=0;
    string Identity;
    for(i=0;i<Identity_size;i++)
    {
        Identity_conf_size = (int)Training_data[i].Segmented_Configure.size();
        for(j=0;j<Identity_conf_size;j++)
        {
            Segment1_dis = Training_data[i].Segment1_Weight*Gait_dissimilarity(Training_data[i].Segmented_Configure[j].Segment1, Input_data.Segment1);
            Segment2_dis = Training_data[i].Segment2_Weight*Gait_dissimilarity(Training_data[i].Segmented_Configure[j].Segment2, Input_data.Segment2);
            Segment3_dis = Training_data[i].Segment3_Weight*Gait_dissimilarity(Training_data[i].Segmented_Configure[j].Segment3, Input_data.Segment3);
            Segment4_dis = Training_data[i].Segment4_Weight*Gait_dissimilarity(Training_data[i].Segmented_Configure[j].Segment4, Input_data.Segment4);
            Weight_total = Training_data[i].Segment1_Weight+Training_data[i].Segment2_Weight+Training_data[i].Segment3_Weight+Training_data[i].Segment4_Weight;
            Dissimilarity = (Segment1_dis+Segment2_dis+Segment3_dis+Segment4_dis)/Weight_total;
        }
        if(Min_dis > Dissimilarity)
        {
            Identity = Training_data[i].name;
        }
    }
    
    return Identity;
}
