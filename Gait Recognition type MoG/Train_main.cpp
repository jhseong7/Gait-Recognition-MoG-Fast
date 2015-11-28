#include "opencv_inc.h"
#include <iostream>

#include "Global Variables.h"

string Train_main(Mat Input_image, string File_path)
{

    string Final_result_string = "No Data";

    
    static vector<Data_set> Training_data;
    vector<float> Weight;
    Configure Input_data;

	if (Training_data.size() == 0){ Training_data = Read_training_data(File_path); }
    Input_data = Final_result(Input_image);
	if (Input_data.period_ok == true)
	{
		Final_result_string = Recognition(Training_data, Input_data);
		cout << Final_result_string << endl;
	}
	else
	{
		;
	}

	return Final_result_string;
}


