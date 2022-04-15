#ifndef MS_H
#define MS_H

#include "primitives.h"
#include "marching_squares.h"

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

#include <deque>
using std::deque;


class contour
{
public:
	deque<line_segment> d;
};




void render_image(int &argc, char ** &argv);
void idle_func(void);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);


// g++ *.cpp -framework GLUT -framework OpenGL
#include <GL/glut.h>



// A C++ namespace wouldn't hurt here...

// OpenGL viewport parameters.
GLint win_id = 0;
GLint win_x = 480, win_y = 480;
GLfloat camera_z = 1.25f;
float background_colour = 0.33f;

bool do_border = false;
const size_t type_count = 2;
const size_t marching_squares_resolution = 64; // Minimum is 2

float template_width = 1;
float template_height = 0;
float step_size = 0;
float isovalue = 0;
float inverse_width = 0;
float grid_x_min = 0;
float grid_y_max = 0;


vector<vector<vertex_2>> train_points;
vector<vector<line_segment>> line_segments;

// Only one of these are needed, because we are using type count = 2

vector<contour> final_contours;
vector<vector<vertex_2>> normals;

void merge_contours(vector<contour> &c, vector<contour> &fc)
{
	if(c.size() == 0)
		return;

	if (c.size() == 1)
	{
		fc.push_back(c[0]);
		c.pop_back();
		return;
	}

	vertex_2 first_end_vertex = c[c.size() - 1].d[0].vertex[0];
	vertex_2 last_end_vertex = c[c.size() - 1].d[c[c.size() - 1].d.size() - 1].vertex[1];

	if (first_end_vertex == last_end_vertex)
	{
		cout << "Closed loop" << endl;
		fc.push_back(c[c.size() - 1]);
		c.pop_back();
		return;
	}
	else
	{
		cout << "not a closed loop" << endl;

		for (size_t i = 0; i < c.size() - 1; i++)
		{
			vertex_2 first0 = c[i].d[0].vertex[0];
			vertex_2 last1 = c[i].d[c[i].d.size() - 1].vertex[1];

			vertex_2 first1 = c[i].d[0].vertex[1];
			vertex_2 last0 = c[i].d[c[i].d.size() - 1].vertex[0];



			if (first_end_vertex == last0)
			{
				// found match, prepend data
				cout << "prepend data flipped last0" << endl;

				for (size_t j = 0; j < c[i].d.size(); j++)
					c[c.size() - 1].d.push_front(c[i].d[j].flip());

				c.erase(c.begin() + i);

				return;
			}
			else if (first_end_vertex == last1)
			{
				// found match, prepend data
				cout << "prepend data last1" << endl;

				for (size_t j = 0; j < c[i].d.size(); j++)
					c[c.size() - 1].d.push_front(c[i].d[j]);

				c.erase(c.begin() + i);

				return;
			}
			else if (last_end_vertex == first0)
			{
				// found match, append data
				cout << "append data first0" << endl;

				for (size_t j = 0; j < c[i].d.size(); j++)
					c[c.size() - 1].d.push_back(c[i].d[j]);

				c.erase(c.begin() + i);

				return;
			}
			else if (last_end_vertex == first1)
			{
				// found match, append data
				cout << "append data flipped first1" << endl;

				for (size_t j = 0; j < c[i].d.size(); j++)
					c[c.size() - 1].d.push_back(c[i].d[j].flip());

				c.erase(c.begin() + i);

				return;
			}
		}



		// no match found
		fc.push_back(c[c.size() - 1]);
		c.pop_back();

		return;



	}
}

vector<vector<triangle>> triangles;
vector<colour_3> colours;


vertex_2 test_point(0, 0);
size_t test_point_index = 0;






bool ray_intersects_triangle(
	const vertex_3 & orig, const vertex_3& dir,
	const vertex_3& v0, const vertex_3& v1, const vertex_3& v2,
	float& t)
{
	// Triangles exist in the z = 0 plane

	float smallest_x = FLT_MAX;
	float smallest_y = FLT_MAX;
	float greatest_x = -FLT_MAX;
	float greatest_y = -FLT_MAX;

	if (v0.x > greatest_x) greatest_x = v0.x;
	if (v1.x > greatest_x) greatest_x = v1.x;
	if (v2.x > greatest_x) greatest_x = v2.x;
	if (v0.x < smallest_x) smallest_x = v0.x;
	if (v1.x < smallest_x) smallest_x = v1.x;
	if (v2.x < smallest_x) smallest_x = v2.x;
	if (v0.y > greatest_y) greatest_y = v0.y;
	if (v1.y > greatest_y) greatest_y = v1.y;
	if (v2.y > greatest_y) greatest_y = v2.y;
	if (v0.y < smallest_y) smallest_y = v0.y;
	if (v1.y < smallest_y) smallest_y = v1.y;
	if (v2.y < smallest_y) smallest_y = v2.y;

	// Point lies outside of the triangle's bounding square
	if (orig.x < smallest_x || orig.x > greatest_x || orig.y < smallest_y || orig.y > greatest_y)
		return false;

	// compute plane's normal
	vertex_3 v0v1 = v1 - v0;
	vertex_3 v0v2 = v2 - v0;
	// no need to normalize
	vertex_3 N = v0v1.cross(v0v2); // N 
	float area2 = N.length();

	// Step 1: finding P

	// check if ray and plane are parallel ?
	float NdotRayDirection = N.dot(dir);
	if (fabs(NdotRayDirection) < 0.00001) // almost 0 
		return false; // they are parallel so they don't intersect ! 

	// compute d parameter using equation 2
	float d = -N.dot(v0);

	// compute t (equation 3)
	t = -(N.dot(orig) + d) / NdotRayDirection;

	// check if the triangle is in behind the ray
	if (t < 0) return false; // the triangle is behind 

	// compute the intersection point using equation 1
	vertex_3 P = orig + dir * t;

	// Step 2: inside-outside test
	vertex_3 C; // vector perpendicular to triangle's plane 

	// edge 0
	vertex_3 edge0 = v1 - v0;
	vertex_3 vp0 = P - v0;
	C = edge0.cross(vp0);
	if (N.dot(C) < 0) return false; // P is on the right side 

	// edge 1
	vertex_3 edge1 = v2 - v1;
	vertex_3 vp1 = P - v1;
	C = edge1.cross(vp1);
	if (N.dot(C) < 0)  return false; // P is on the right side 

	// edge 2
	vertex_3 edge2 = v0 - v2;
	vertex_3 vp2 = P - v2;
	C = edge2.cross(vp2);
	if (N.dot(C) < 0) return false; // P is on the right side; 

	return true; // this ray hits the triangle 
}

bool ray_intersects_triangle_vector(size_t index)
{
	const vertex_3 origin(test_point.x, test_point.y, 0);
	const vertex_3 ray(test_point.x, test_point.y, -1);

	for (size_t i = 0; i < triangles[index].size(); i++)
	{
		vertex_3 v0(
			triangles[index][i].vertex[0].x,
			triangles[index][i].vertex[0].y,
			0);

		vertex_3 v1(
			triangles[index][i].vertex[1].x,
			triangles[index][i].vertex[1].y,
			0);

		vertex_3 v2(
			triangles[index][i].vertex[2].x,
			triangles[index][i].vertex[2].y,
			0);

		float out = 0;

		if (ray_intersects_triangle(origin, ray, v0, v1, v2, out))
			return true;
	}

	return false;
}

bool get_index(size_t &out)
{
	for (size_t i = 0; i < type_count; i++)
	{
		if (ray_intersects_triangle_vector(i))
		{
			out = i;
			return true;
		}
	}

	return false;
}



size_t get_closest_index(const vertex_2 v)
{
	map<float, size_t> distance_index_map;

	for (size_t i = 0; i < type_count; i++)
	{
		float total_distance = 0;

		for (size_t j = 0; j < train_points[i].size(); j++)
		{
			line_segment ls;
			ls.vertex[0] = v;
			ls.vertex[1] = train_points[i][j];

			float distance = ls.length();

			if(distance != 0)
				total_distance += 1.0f / distance;
		}

		distance_index_map[total_distance] = i;
	}

	map<float, size_t>::reverse_iterator ci = distance_index_map.rbegin();

	float closest_distance = ci->first;
	size_t closest_index = ci->second;

	return closest_index;
}



float get_value(const size_t index, const vertex_2 v)
{
	float running_value = 0;

	for (size_t i = 0; i < type_count; i++)
	{
		for (size_t j = 0; j < train_points[i].size(); j++)
		{
			line_segment ls;
			ls.vertex[0] = v;
			ls.vertex[1] = train_points[i][j];

			float distance = ls.length();

			if (distance != 0)
			{
				if (index == i)
					running_value += 1.0f / distance;
				else
					running_value -= 1.0f / distance;
			}
		}
	}

	return running_value;
}






#endif
