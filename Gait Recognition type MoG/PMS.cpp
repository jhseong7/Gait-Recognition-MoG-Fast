#include "opencv_inc.h"
#include "Preprocess_PMS.h"

vector<complex<float> > PMS(vector<vector<complex<float> >> CSC_array)
{
    int CSC_size = (int)CSC_array.size();
    int array_size = 0;
    int i;
    int m,n;
    
    vector<vector< complex<float>> > Product_mat;
    vector<vector< complex<float>> > S_Z;
    vector<complex<float> > temp_mat;
    vector<complex<float> > Major_eigen_vector;
    complex<float> mat_ij_1;
    complex<float> mat_ij_2;
    complex<float> scalar(0,0);
	S_Z.resize(40, vector< complex<float>>(40, 0));
	for (m = 0; m<40; m++)
	{
		for (n = 0; n<40; n++)
		{
			S_Z[m][n] = 0;
		}
	}

    for(i=0;i<period;i++)
    {
        array_size = (int)(CSC_array[i]).size();

        for(m=0;m<array_size;m++)
        {
            mat_ij_1 = CSC_array[i][m];
            scalar = scalar + mat_ij_1*conj(mat_ij_1);
            for(n=0;n<array_size;n++)
            {
                mat_ij_2 = CSC_array[i][n];
                mat_ij_2 = conj(mat_ij_2);
                temp_mat.push_back(mat_ij_1*mat_ij_2);
            }
            Product_mat.push_back(temp_mat);
            temp_mat.clear();
        }
        
        for(m=0;m<array_size;m++)
        {
            for(n=0;n<array_size;n++)
            {
                Product_mat[m][n] = Product_mat[m][n]/scalar;
            }
        }
        
        for(m=0;m<array_size;m++)
        {
            for(n=0;n<array_size;n++)
            {
                S_Z[m][n] = S_Z[m][n] + Product_mat[m][n];
            }
        }
        
		scalar = 0;
        Product_mat.clear();
    }
    
    MatrixXcf temp(array_size,array_size);
    MatrixXcf evec(array_size,array_size);
    MatrixXcf eval(array_size,1);
    
    for(m=0;m<array_size;m++)
    {
        for(n=0;n<array_size;n++)
        {
            temp(m,n) = S_Z[m][n];
        }
    }
    
    ComplexEigenSolver<MatrixXcf> ces;
    ces.compute(temp);
    
    evec = ces.eigenvectors();  eval = ces.eigenvalues();
    
    //cout << " Eigen " << eval(0,0) << endl;
    //cout << " EE " << evec(0,0) << endl;
    
    float Max_eigen=0.0;
    int Max_eigen_index=0;
    float temp_value=0.0;
    
    for(i=0;i<array_size;i++)
    {
        temp_value = norm( (i,0));
        if(temp_value > Max_eigen)
        {
            Max_eigen = temp_value;
            Max_eigen_index = i;
        }
    }
    
    /*
    cout << "The eigenvector of temp are" << endl << evec << endl;
    cout << "The eigenvalue of temp are" << endl << eval << endl;
    cout << "Max eigenvalue " << endl << Max_eigen << endl;
    cout << "Max eigenvalue index " << endl << Max_eigen_index << endl;
    cout << "Max eigenvector " << endl << Max_eigen*evec(i) << endl;
    */
     
    for(i=0;i<array_size;i++)
	{
		Major_eigen_vector.push_back(evec(i, Max_eigen_index));
        //Major_eigen_vector.push_back(Max_eigen*evec(i,Max_eigen_index));
        //cout << Major_eigen_vector[i] << endl;
    }
    
    return Major_eigen_vector;
}