//
//  Contour.hpp
//  Design_project_rebuild
//
//  Created by 이용주 on 2015. 11. 3..
//  Copyright © 2015년 Raymond. All rights reserved.
//

#ifndef Contour_hpp
#define Contour_hpp

#include <stdio.h>
#include "opencv_essential_include.h"

Mat contour(Mat* input_image, vector<vector<Point> >* contour_vector);

#endif /* Contour_hpp */
