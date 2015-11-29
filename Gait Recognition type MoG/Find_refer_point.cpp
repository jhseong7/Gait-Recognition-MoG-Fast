#include "opencv_inc.h"
#include "Preprocess_PMS.h"

vector<Point> Find_refer_point(vector<Point> contour_point)
{
    int size = contour_point.size(); Point temp;
    int i=0,j=0,k=0;
    float x_sum=0,y_sum=0;    float x_mean,y_mean;
    float xx=0,xy=0,yx=0,yy=0;    float xd,yd;
    float A[2][2];
    
    float maj1,maj2,min1,min2;
    
    for(i=0;i<size;i++)
    {
        temp = contour_point[i];
        x_sum = temp.x + x_sum;
        y_sum = temp.y + x_sum;
    }
    
    x_mean = x_sum/(float)size; y_mean = y_sum/(float)size;
    
    for(i=0;i<size;i++)
    {
        temp = contour_point[i];
        
        xd = (float)temp.y - x_mean;
        yd = (float)temp.x - y_mean;
        
        xx = xx + (xd*xd)/(float)size;
        xy = xy + (xd*yd)/(float)size;
        yx = yx + (yd*xd)/(float)size;
        yy = yy + (yd*yd)/(float)size;
    }
    
    A[0][0] = xx; A[0][1] = xy; A[1][0] = yx; A[1][1] = yy;
    
    Mat CM(2,2,CV_32FC1,A);
    Mat eival(2,1,CV_32FC1);
    Mat eivec(2,2,CV_32FC1);
    
    eigen(CM,eival,eivec);
    
    maj1 = eivec.at<float>(0,0);
    maj2 = eivec.at<float>(0,1);
    min1 = eivec.at<float>(1,0);
    min2 = eivec.at<float>(1,1);
    
    float Head[2]={0,0}, Lfoot[2]={0,0}, Rfoot[2]={0,0};
    float dummy_x, dummy_y;
    float Rmin[2], Rmaj[2], Gmin[2], Gmaj[2];
    
    Rmin[0] = (-1)*maj1;    Rmin[1] = (-1)*maj2;    Rmaj[0] = maj1; Rmaj[1] = maj2;
    Gmin[0] = (-1)*min1;    Gmin[1] = (-1)*min2;    Gmaj[0] = min1; Gmaj[1] = min2;
    
    Mat Ra(1,2,CV_32FC1,Rmin);
    Mat Rb(1,2,CV_32FC1,Rmaj);
    Mat Ga(1,2,CV_32FC1,Gmin);
    Mat Gb(1,2,CV_32FC1,Gmaj);
    Mat dumvec(1,2,CV_32FC1);
    
    float Hmax=0,Lmax=0,Rmax=0,d_dum=0;
    float dum1,dum2,dum3;
    
    for (k = 0; k < size; k++)
    {
        temp = contour_point[k];
        i = temp.y; j = temp.x;
        dummy_x = (float)temp.y - x_mean; dummy_y = (float)temp.x - y_mean;
        dumvec.at<float>(0, 0) = dummy_x;
        dumvec.at<float>(0, 1) = dummy_y;
        
        dum1 = Ra.dot(dumvec);
        if (Hmax < dum1)
        {
            Hmax = dum1;
            Head[0] = j; Head[1] = i;
        }
        
        dum2 = Ga.dot(dumvec);
        if (dum2 > 0)
        {
            d_dum = Rb.dot(dumvec);
            if (d_dum > 0)
            {
                if (Lmax < (d_dum + dum2))
                {
                    Lmax = d_dum + dum2;
                    Lfoot[0] = j, Lfoot[1] = i;
                }
            }
        }
        
        dum3 = Gb.dot(dumvec);
        if (dum3 > 0)
        {
            d_dum = Rb.dot(dumvec);
            if (d_dum > 0)
            {
                if (Rmax < (d_dum + dum3))
                {
                    Rmax = d_dum + dum3;
                    Rfoot[0] = j, Rfoot[1] = i;
                }
            }
        }
    }
    

    vector<Point> Result(3);
    
    Result[0].x = Head[0]; Result[0].y = Head[1];
    Result[1].x = Lfoot[0]; Result[1].y = Lfoot[1];
    Result[2].x = Rfoot[0]; Result[2].y = Rfoot[1];

    return Result;
    
}