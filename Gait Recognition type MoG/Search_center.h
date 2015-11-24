#ifndef SEARCH_CENTER_H
#define SEARCH_CENTER_H

typedef struct _coordinate
{
	int x;
	int y;
}Coordinate;

void Search_center(Mat* Input_image,Coordinate * center);

#endif
