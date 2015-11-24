//
//  Resampling.hpp
//  Design_project
//
//  Created by 이용주 on 2015. 11. 7..
//  Copyright © 2015년 Raymond. All rights reserved.
//

#ifndef Resampling_hpp
#define Resampling_hpp

#include "opencv_inc.h"
#include <stdio.h>

vector<vector<Point> > Resampling(vector<Point> * input_contour,vector<Point>* refer_point);
Mat Draw_Resampling(vector<vector<Point> > Segment_array,Mat Size_refe_image);

#endif /* Resampling_hpp */
