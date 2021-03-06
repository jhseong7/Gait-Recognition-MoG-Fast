#include "opencv_inc.h"
#include "Preprocess_PMS.h"

int Gait_period_cal(int array_number, vector<double> Ratio_array)
{
    int t=1;	int i;
    double temp_sum1=0;	double temp_sum2=0;	double temp_sum3=0;
    double temp_result=0.0;	double result=0.0;
    int result_period=0;
    // Initiate result
    int N_t = array_number - t;
	if (N_t > 0)
	{
		for (i = 0; i <= N_t; i++)
		{
			temp_sum1 += Ratio_array[i] * Ratio_array[i + t];
			temp_sum2 += Ratio_array[i] * Ratio_array[i];
			temp_sum3 += Ratio_array[i + t] * Ratio_array[i + t];
		}
		temp_sum2 = sqrt(temp_sum2);
		temp_sum3 = sqrt(temp_sum3);
		temp_result = temp_sum1 / (temp_sum2*temp_sum3);
		result = temp_result;
		temp_sum1 = 0;
		temp_sum2 = 0;
		temp_sum3 = 0;

		for (t = 2; t < array_number; t++)
		{
			N_t = array_number - t;
			for (i = 0; i <= N_t; i++)
			{
				temp_sum1 += Ratio_array[i] * Ratio_array[i + t];
				temp_sum2 += Ratio_array[i] * Ratio_array[i];
				temp_sum3 += Ratio_array[i + t] * Ratio_array[i + t];
			}
			temp_sum2 = sqrt(temp_sum2);
			temp_sum3 = sqrt(temp_sum3);
			temp_result = temp_sum1 / (temp_sum2*temp_sum3);
			if (temp_result >= result)
			{
				result = temp_result;
				result_period = t;
				printf("result:%f, result_period:%d\n", result, result_period);
				result_period = 2 * result_period;
			}
			temp_sum1 = 0;
			temp_sum2 = 0;
			temp_sum3 = 0;
		}

		return result_period;

	}
	else
	{
		return -1;
	}

	return -1;
}

