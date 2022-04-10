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
const size_t marching_squares_resolution = 3;
float pow_factor = 10;
float template_width = 1;
float template_height = 0;
float step_size = 0;
float isovalue = 0;
float inverse_width = 0;
float grid_x_min = 0;
float grid_y_max = 0;


vector<vector<vertex_2>> train_points;
vector<vector<line_segment>> line_segments;
vector<vector<triangle>> triangles;
vector<colour_3> colours;


vertex_2 test_point(0, 0);
size_t test_point_index = 0;

bool ray_intersects_triangle(
	const vertex_3 & orig, const vertex_3& dir,
	const vertex_3& v0, const vertex_3& v1, const vertex_3& v2,
	float& t)
{
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
	const vertex_3 ray(test_point.x, test_point.y, 1);

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

		float out;

		if (ray_intersects_triangle(origin, ray, v0, v1, v2, out))
		{
			cout << "found hit " << index << endl;
			return true;
		}
	}

	return false;
}



//
//size_t get_closest_index(const vertex_2 v)
//{
//	map<float, size_t> distance_index_map;
//
//	for (size_t i = 0; i < type_count; i++)
//	{
//		float total_distance = 0;
//
//		for (size_t j = 0; j < train_points[i].size(); j++)
//		{
//			line_segment ls;
//			ls.vertex[0] = v;
//			ls.vertex[1] = train_points[i][j];
//
//			float distance = ls.length();
//
//			if(distance != 0)
//				total_distance += 1.0f / powf(distance, pow_factor);
//		}
//
//		distance_index_map[total_distance] = i;
//	}
//
//	map<float, size_t>::reverse_iterator ci = distance_index_map.rbegin();
//
//	float closest_distance = ci->first;
//	size_t closest_index = ci->second;
//
//	return closest_index;
//}



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
					running_value += 1.0f / powf(distance, pow_factor);
				else
					running_value -= 1.0f / powf(distance, pow_factor);
			}
		}
	}

	return running_value;
}






#endif
