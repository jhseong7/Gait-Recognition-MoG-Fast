//
//  Read_training_data.cpp
//  Read_Data_&_Recognition
//
//  Created by 이용주 on 2015. 11. 26..
//  Copyright © 2015년 Raymond. All rights reserved.
//
#include "opencv_inc.h"
#include <stdio.h>



vector<Data_set> Read_training_data(string File_path)
{
    char Training_file_Data_char[100];  string Training_file_Data_string;
    char Training_file_Identity_list[100];
    string Identity_number_string;      int Identity_number=0;
    string Identity_name_string;        vector<string> Identity_Name;
    string Identity_confi_size_string;  vector<int> Configure_number;
    string Identity_weight_string;
    string Identity_conf_real;  string Identity_conf_imag;
    int Index = -1; int Sub_index = -1;
    int Size=0; int Sub_size=0;
    int k=0;
    
    vector<Data_set> Training_Data;
    Data_set temp;
    Configure Configure;
    
    ifstream Training_data_file;
    sprintf(Training_file_Identity_list, "%sTraining_Identity_list.txt",File_path.c_str());
    Training_data_file.open(Training_file_Identity_list);
    
    Training_data_file.getline(Training_file_Data_char, 100);
    Training_file_Data_string = Training_file_Data_char;
    
    Index = (int)Training_file_Data_string.find("Identity_number");
    Sub_index = (int)Training_file_Data_string.find("Identity_number_end");
    Size = sizeof("Identity_number");
    Sub_size = Sub_index - Index;
    Identity_number_string = Training_file_Data_string.substr(Index+Size,Sub_size-Size);
    Identity_number = atoi(Identity_number_string.c_str());
    cout << Identity_number << endl;
    
    for(k=0;k<Identity_number;k++)
    {
        Training_data_file.getline(Training_file_Data_char, 100);
        Training_file_Data_string = Training_file_Data_char;
        Index = (int)Training_file_Data_string.find("Name");
        Sub_index = (int)Training_file_Data_string.find("Name_end");
        Size = sizeof("Name");
        Sub_size = Sub_index - Index;
        Identity_name_string = Training_file_Data_string.substr(Index+Size,Sub_size-Size-1);
        Identity_Name.push_back(Identity_name_string);
        cout << Identity_name_string << endl;
        Training_data_file.getline(Training_file_Data_char, 100);
        Training_file_Data_string = Training_file_Data_char;
        Index = (int)Training_file_Data_string.find("Configure_size");
        Sub_index = (int)Training_file_Data_string.find("Configure_size_end");
        Size = sizeof("Configure_size");
        Sub_size = Sub_index - Index;
        Identity_confi_size_string = Training_file_Data_string.substr(Index+Size,Sub_size-Size);
        Configure_number.push_back(atoi(Identity_confi_size_string.c_str()));
        cout << Identity_confi_size_string << endl;
    }
    
    Training_data_file.close();
    k=0;
    
    char Training_file_PMS_data[50];
    char Identity_segment_start[50];    char Identity_segment_end[50];
    char Identity_conf_start[50];       char Identity_conf_end[50];
    int i,j,l;
    int temp_conform;
    
    for(i=0;i<Identity_number;i++)
    {
        sprintf(Training_file_PMS_data, "%s%s_Training_data.txt", File_path.c_str(),Identity_Name[i].c_str());
        cout << Training_file_PMS_data << endl;
        Training_data_file.open(Training_file_PMS_data);
        Training_data_file.getline(Training_file_Data_char, 100);
        Training_file_Data_string = Training_file_Data_char;
        temp_conform = Training_file_Data_string.compare("Weight_start");
        if((Training_file_Data_string.compare("Weight_start")==0))
        {
            Training_data_file.getline(Training_file_Data_char, 100);
            Training_file_Data_string = Training_file_Data_char;
            while(!((Training_file_Data_string.compare("Weight_end"))==0))
            {
                Index = 0;
                Sub_index = (int)Training_file_Data_string.length();
                Size = sizeof("Segment1_Weight");
                Sub_size = Sub_index - Index;
                Identity_weight_string = Training_file_Data_string.substr(Index+Size,Sub_size-Size);
                cout << Identity_weight_string << endl;
                if(k==0){temp.Segment1_Weight=1;}
                else if(k==1){temp.Segment2_Weight=stof(Identity_weight_string.c_str());}
                else if(k==2){temp.Segment3_Weight=stof(Identity_weight_string.c_str());}
                else if(k==3){temp.Segment4_Weight=stof(Identity_weight_string.c_str());}
                else{;}
                k++;
                Training_data_file.getline(Training_file_Data_char, 100);
                Training_file_Data_string = Training_file_Data_char;
            }
        }
        k=0;
        for(l=0;l<Configure_number[i];l++)
        {
            Training_data_file.getline(Training_file_Data_char, 100);
            Training_file_Data_string = Training_file_Data_char;
            sprintf(Identity_conf_start, "%s_%d_start",Identity_Name[i].c_str(),l);
            sprintf(Identity_conf_end, "%s_%d_end",Identity_Name[i].c_str(),l);
            if((Training_file_Data_string.compare(Identity_conf_start)==0))
            {
                while(!((Training_file_Data_string.compare(Identity_conf_end))==0))
                {
                    for(j=0;j<4;j++)
                    {
                        Training_data_file.getline(Training_file_Data_char, 100);
                        Training_file_Data_string = Training_file_Data_char;
                        sprintf(Identity_segment_start, "%s_Segment%d_start",Identity_Name[i].c_str(),j);
                        sprintf(Identity_segment_end, "%s_Segment%d_end",Identity_Name[i].c_str(),j);
                        if((Training_file_Data_string.compare(Identity_segment_start)==0))
                        {
                            Training_data_file.getline(Training_file_Data_char, 100);
                            Training_file_Data_string = Training_file_Data_char;
                            while(!((Training_file_Data_string.compare(Identity_segment_end))==0))
                            {
                                Index = 0;
                                Sub_index = (int)Training_file_Data_string.find("+");
                                Size = Sub_index - Index;
                                Identity_conf_real = Training_file_Data_string.substr(Index,Size-1);
                                Index = (int)Training_file_Data_string.length();
                                Sub_size = Index - Sub_index;
                                Identity_conf_imag = Training_file_Data_string.substr(Sub_index+2,Sub_size);
                                cout << Identity_conf_real << endl;
                                cout << Identity_conf_imag << endl;
                                complex<float> Identity_conf(stof(Identity_conf_real),stof(Identity_conf_imag));
                                if(k==0){Configure.Segment1.push_back(Identity_conf);}
                                else if(k==1){Configure.Segment2.push_back(Identity_conf);}
                                else if(k==2){Configure.Segment3.push_back(Identity_conf);}
                                else if(k==3){Configure.Segment4.push_back(Identity_conf);}
                                else{;}
                                Training_data_file.getline(Training_file_Data_char, 100);
                                Training_file_Data_string = Training_file_Data_char;
                            }
                            k++;
                        }
                    }
                    k=0;
                    temp.Segmented_Configure.push_back(Configure);
                    Configure.Segment1.clear();
                    Configure.Segment2.clear();
                    Configure.Segment3.clear();
                    Configure.Segment4.clear();
                    Training_data_file.getline(Training_file_Data_char, 100);
                    Training_file_Data_string = Training_file_Data_char;
                }
            }
        }
        k=0;
        temp.name = Identity_Name[i];
        temp.Configure_size = Configure_number[i];
        Training_Data.push_back(temp);
        temp.Segmented_Configure.clear();
        
        Training_data_file.close();
    }
    
    return Training_Data;
}

