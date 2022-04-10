#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#include <cmath>



class vertex_3
{
public:
	inline vertex_3(void) : x(0.0f), y(0.0f), z(0.0f) { /*default constructor*/ }
	inline vertex_3(const float src_x, const float src_y, const float src_z) : x(src_x), y(src_y), z(src_z) { /* custom constructor */ }

	inline bool operator==(const vertex_3& right) const
	{
		if (right.x == x && right.y == y && right.z == z)
			return true;
		else
			return false;
	}

	inline bool operator<(const vertex_3& right) const
	{
		if (right.x > x)
			return true;
		else if (right.x < x)
			return false;

		if (right.y > y)
			return true;
		else if (right.y < y)
			return false;

		if (right.z > z)
			return true;
		else if (right.z < z)
			return false;

		return false;
	}

	inline const vertex_3& operator-(const vertex_3& right) const
	{
		static vertex_3 temp;

		temp.x = this->x - right.x;
		temp.y = this->y - right.y;
		temp.z = this->z - right.z;

		return temp;
	}

	inline const vertex_3& operator+(const vertex_3& right) const
	{
		static vertex_3 temp;

		temp.x = this->x + right.x;
		temp.y = this->y + right.y;
		temp.z = this->z + right.z;

		return temp;
	}

	inline const vertex_3& operator*(const float& right) const
	{
		static vertex_3 temp;

		temp.x = this->x * right;
		temp.y = this->y * right;
		temp.z = this->z * right;

		return temp;
	}

	inline const vertex_3& operator/(const float& right) const
	{
		static vertex_3 temp;

		temp.x = this->x / right;
		temp.y = this->y / right;
		temp.z = this->z / right;

		return temp;
	}

	inline const vertex_3& cross(const vertex_3& right) const
	{
		static vertex_3 temp;

		temp.x = y * right.z - z * right.y;
		temp.y = z * right.x - x * right.z;
		temp.z = x * right.y - y * right.x;

		return temp;
	}

	inline float dot(const vertex_3& right) const
	{
		return x * right.x + y * right.y + z * right.z;
	}

	inline const float self_dot(void)
	{
		return x * x + y * y + z * z;
	}

	inline const float length(void)
	{
		return std::sqrt(self_dot());
	}

	inline const void normalize(void)
	{
		float len = length();

		if (0.0f != len)
		{
			x /= len;
			y /= len;
			z /= len;
		}
	}

	inline void zero(void)
	{
		x = y = z = 0;
	}

	// https://www.cs.helsinki.fi/group/goa/mallinnus/3dtransf/3drot.html

	inline void rotate_x(const float& radians)
	{
		float t_y = y;

		y = t_y * cos(radians) + z * sin(radians);
		z = t_y * -sin(radians) + z * cos(radians);
	}

	inline void rotate_y(const float& radians)
	{
		float t_x = x;

		x = t_x * cos(radians) + z * -sin(radians);
		z = t_x * sin(radians) + z * cos(radians);
	}

	inline void rotate_z(const float& radians)
	{
		float t_x = x;

		x = t_x * cos(radians) + y * sin(radians);
		y = t_x * -sin(radians) + y * cos(radians);
	}

	float x, y, z;
};





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

		return 0.5f*a;
	}
};


class line_segment
{
public:
	vertex_2 vertex[2];

	float length(void)
	{
		return sqrtf( pow(vertex[0].x - vertex[1].x, 2.0f) + pow(vertex[0].y - vertex[1].y, 2.0f) );
	}
};


class colour_3
{
public:
	float r, g, b;
};

#endif
