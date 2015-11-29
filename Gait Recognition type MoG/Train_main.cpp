#include "opencv_inc.h"
#include <iostream>

#include "Global Variables.h"

vector<Data_set> Training_data;

string Train_main(Mat Input_image, vector<Data_set>* Training_data)
{
    string Final_result_string = "No Data";
    vector<float> Weight;
    Configure Input_data;
	int period_final=0;
	
	
	/*
	if (Training_data.size() == 0)
	{ 
		Training_data = Read_training_data(File_path); 
	}

	*/

    Input_data = Final_result(Input_image,&period_final);
	if (Input_data.period_ok == true)
	{
		Final_result_string = Recognition(*Training_data, Input_data);
		cout << Final_result_string << endl;
	}
	else
	{
		;
	}

	return Final_result_string;
}


