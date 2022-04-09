#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#include <cmath>

class vertex_2
{
public:
	float x;
	float y;

	vertex_2(const float src_x = 0, const float src_y = 0)
	{
		x = src_x;
		y = src_y;
	}

	inline bool operator==(const vertex_2 &right) const
	{
		if(right.x == x && right.y == y)
			return true;
		else
			return false;
	}

	inline bool operator<(const vertex_2 &right) const
	{
		if(right.x > x)
			return true;
		else if(right.x < x)
			return false;
			
		if(right.y > y)
			return true;
		else if(right.y < y)
			return false;

		return false;
	}

	inline const vertex_2& operator-(const vertex_2 &right) const
	{
		static vertex_2 temp;

		temp.x = this->x - right.x;
		temp.y = this->y - right.y;

		return temp;
	}

	inline float dot(const vertex_2 &right) const
	{
		return x*right.x + y*right.y;
	}

	inline const float self_dot(void)
	{
		return x*x + y*y;
	}

	inline const float length(void)
	{
		return sqrt(self_dot());
	}
};

class triangle
{
public:
	vertex_2 vertex[3];

	inline float area(void)
	{
		if(vertex[0] == vertex[1] || vertex[0] == vertex[2] || vertex[1] == vertex[2])
			return 0;

		float a = (vertex[1].x - vertex[0].x)*(vertex[2].y - vertex[0].y) - (vertex[2].x - vertex[0].x)*(vertex[1].y - vertex[0].y);

		return 0.5*a;
	}
};


class line_segment
{
public:
	vertex_2 vertex[2];

	float length(void)
	{
		return sqrt( pow(vertex[0].x - vertex[1].x, 2.0) + pow(vertex[0].y - vertex[1].y, 2.0) );
	}
};


class colour_3
{
public:
	float r, g, b;
};

#endif
