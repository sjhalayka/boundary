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
GLint win_x = 640, win_y = 480;
GLfloat camera_z = 1.25;
float background_colour = 0.33;


const size_t marching_squares_resolution = 128;
float pow_factor = 10.0f;
float template_width = 0;
float template_height = 0;
float step_size = 0;
float isovalue = 0;
float inverse_width = 0;
float grid_x_min = 0;
float grid_y_max = 0;


const size_t type_count = 4;
vector<vector<vertex_2>> train_points;
vector<vector<line_segment>> line_segments;
vector<vector<triangle>> triangles;
vector<colour_3> colours;


vertex_2 test_point(0, 0);
size_t test_point_index = 0;

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
				total_distance += 1.0f / pow(distance, pow_factor);
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
					running_value += 1.0 / pow(distance, pow_factor);
				else
					running_value -= 1.0 / pow(distance, pow_factor);
			}
		}
	}

	return running_value;
}






#endif
