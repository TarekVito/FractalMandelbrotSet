#ifndef IPOINT_H
#define IPOINT_H 

class IPoint {
public:
	double x,y;
	IPoint(double _x,double _y)
	{
		x=_x;y=_y;
	}
	IPoint operator+(const IPoint& b)
	{
		return IPoint(x+b.x,y+b.y);
	}
	double absSQ()
	{
		return x*x + y*y;
	}
	IPoint sq()
	{
		return IPoint(x*x - y*y,2*x*y);
	}
};

#endif