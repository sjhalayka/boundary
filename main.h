#ifndef MS_H
#define MS_H





#include "primitives.h"
#include "marching_squares.h"
#include "image.h"

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

#include <deque>
using std::deque;


#include <Windows.h>
#include <opencv2/opencv.hpp>
using namespace cv;

#pragma comment(lib, "opencv_world3416.lib")




class contour
{
public:
	deque<line_segment> d;
};




void render_image(int& argc, char**& argv);
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

// Only one copy of the contours are needed, because we are using type count = 2
vector<contour> final_contours;
vector<vector<vertex_2>> normals;

void merge_contours(vector<contour>& c, vector<contour>& fc)
{
	if (c.size() == 0)
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
		//cout << "Closed loop" << endl;
		fc.push_back(c[c.size() - 1]);
		c.pop_back();
		return;
	}
	else
	{
		//cout << "not a closed loop" << endl;

		for (size_t i = 0; i < c.size() - 1; i++)
		{
			vertex_2 first0 = c[i].d[0].vertex[0];
			vertex_2 last1 = c[i].d[c[i].d.size() - 1].vertex[1];

			vertex_2 first1 = c[i].d[0].vertex[1];
			vertex_2 last0 = c[i].d[c[i].d.size() - 1].vertex[0];

			if (first_end_vertex == last0)
			{
				// found match, prepend data
				//cout << "prepend data flipped last0" << endl;

				for (size_t j = 0; j < c[i].d.size(); j++)
					c[c.size() - 1].d.push_front(c[i].d[j].flip());

				c.erase(c.begin() + i);

				return;
			}
			else if (first_end_vertex == last1)
			{
				// found match, prepend data
				//cout << "prepend data last1" << endl;

				for (size_t j = 0; j < c[i].d.size(); j++)
					c[c.size() - 1].d.push_front(c[i].d[j]);

				c.erase(c.begin() + i);

				return;
			}
			else if (last_end_vertex == first0)
			{
				// found match, append data
				//cout << "append data first0" << endl;

				for (size_t j = 0; j < c[i].d.size(); j++)
					c[c.size() - 1].d.push_back(c[i].d[j]);

				c.erase(c.begin() + i);

				return;
			}
			else if (last_end_vertex == first1)
			{
				// found match, append data
				//cout << "append data flipped first1" << endl;

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
	const vertex_3& orig, const vertex_3& dir,
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

bool get_index(size_t& out)
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

			if (distance != 0)
				total_distance += 1.0f / powf(distance, 1);
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
					running_value += 1.0f / powf(distance, 1);
				else
					running_value -= 1.0f / powf(distance, 1);
			}
		}
	}

	return running_value;
}



vector<float> opencv_blur(const vector<float>& image, const size_t num_iterations)
{
	Mat m = Mat(marching_squares_resolution, marching_squares_resolution, CV_32FC1);
	memcpy(m.data, image.data(), image.size() * sizeof(float));

	for (size_t i = 0; i < num_iterations; i++)
		GaussianBlur(m, m, Size(25, 25), 1, 1);

	vector<float> temp_image = image;
	memcpy(&temp_image[0], m.data, image.size() * sizeof(float));

	return temp_image;
}

vector<float> opencv_sharpen(const vector<float>& image)
{
	Mat m = Mat(marching_squares_resolution, marching_squares_resolution, CV_32FC1);
	memcpy(m.data, image.data(), image.size() * sizeof(float));

	Mat kernel3 = (Mat_<double>(3, 3) <<
		0, -1, 0,
		-1, 5, -1,
		0, -1, 0);

	filter2D(m, m, -1, kernel3, Point(-1, -1), 0, BORDER_DEFAULT);

	vector<float> temp_image = image;
	memcpy(&temp_image[0], m.data, image.size() * sizeof(float));

	return temp_image;


}






vector<vector<float>> opencv_lerp(const vector<vector<float>> &images0, const vector<vector<float>>& images1, size_t target_res, double factor)
{
	vector<vector<float>> temp_images;

	for (size_t i = 0; i < images0.size(); i++)
	{
		Mat m0 = Mat(target_res, target_res, CV_32FC1);
		memcpy(m0.data, images0[i].data(), images0[i].size() * sizeof(float));

		Mat m1 = Mat(target_res, target_res, CV_32FC1);
		memcpy(m1.data, images1[i].data(), images1[i].size() * sizeof(float));

		Mat result;
		addWeighted(m0, 1.0 - factor, m1, factor, 0.0, result);

		vector<float> temp_image(target_res * target_res);
		memcpy(&temp_image[0], result.data, temp_image.size() * sizeof(float));

		temp_images.push_back(temp_image);
	}
	
	return temp_images;
}




vector<float> opencv_resize(const vector<float>& image, size_t target_res)
{
	Mat m = Mat(2, 2, CV_32FC1);
	memcpy(m.data, image.data(), image.size() * sizeof(float));

	float x = static_cast<float>(target_res) / 2;

	resize(m, m, cv::Size(), x, x, INTER_LINEAR);
	vector<float> temp_image(target_res * target_res);
	memcpy(&temp_image[0], m.data, temp_image.size() * sizeof(float));

	return temp_image;
}

vector<float> resize_from_2by2(const vector<float>& image, size_t target_res)
{
	vector<float> temp_image(target_res * target_res, 0.0f);

	const float upper_left = image[0];
	const float upper_right = image[1];
	const float lower_right = image[2];
	const float lower_left = image[3];

	for (size_t i = 0; i < target_res; i++)
	{
		for (size_t j = 0; j < target_res; j++)
		{
			size_t index = i * target_res + j;

			if (i < target_res / 2)
				if (j < target_res / 2)
					temp_image[index] = upper_left;
				else
					temp_image[index] = upper_right;
			else
				if (j < target_res / 2)
					temp_image[index] = lower_right;
				else
					temp_image[index] = lower_left;
		}
	}

	return temp_image;
}






vector<vector<float>> get_data(size_t target_res)
{
	vector<vector<float>> images;

	srand(1234);

	inverse_width = 1.0f / template_width;
	step_size = template_width / static_cast<float>(target_res - 1);
	template_height = step_size * (target_res - 1);

	train_points.clear();
	line_segments.clear();
	triangles.clear();
	colours.clear();

	train_points.resize(type_count);
	line_segments.resize(type_count);
	triangles.resize(type_count);
	colours.resize(type_count);

	for (size_t i = 0; i < colours.size(); i++)
	{
		colours[i].r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colours[i].g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colours[i].b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}

	for (size_t i = 0; i < type_count; i++)
	{
		for (size_t j = 0; j < 5; j++)
		{
			float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			x -= 0.5f;
			y -= 0.5f;
			x *= template_width;
			y *= template_width;

			if (i == 0)
			{
				if (y < 0)
				{
					y = -y;
				}
			}
			else
			{
				if (y > 0)
				{
					y = -y;
				}
			}


			vertex_2 v;
			v.x = x;
			v.y = y;

			train_points[i].push_back(v);
		}
	}




	for (size_t i = 0; i < type_count; i++)
	{
		grid_x_min = -template_width * 0.5f;
		grid_y_max = template_height * 0.5f;

		// Generate geometric primitives using marching squares.
		grid_square g;

		float grid_x_pos = grid_x_min; // Start at minimum x.
		float grid_y_pos = grid_y_max; // Start at maximum y.

		vector<float> image(target_res * target_res, 0.0f);

		// Begin march.
		for (size_t y = 0; y < target_res; y++, grid_y_pos -= step_size, grid_x_pos = grid_x_min)
			for (size_t x = 0; x < target_res; x++, grid_x_pos += step_size)
				image[y * target_res + x] = get_value(i, vertex_2(grid_x_pos, grid_y_pos));

		images.push_back(image);

		//// Convolve image here...


		//float_grayscale luma;
		//luma.px = target_res;
		//luma.py = target_res;

		//luma.pixel_data = image;
		//write_float_grayscale_to_tga("out0.tga", luma);

		////image = opencv_blur(image, 100);
		////luma.pixel_data = image;
		////write_float_grayscale_to_tga("out1.tga", luma);

		////image = opencv_blur(image, 150);
		////luma.pixel_data = image;
		////write_float_grayscale_to_tga("out2.tga", luma);








		// Convert image to contours
		grid_x_pos = grid_x_min; // Start at minimum x.
		grid_y_pos = grid_y_max; // Start at maximum y.

		// Begin march.
		for (size_t y = 0; y < target_res - 1; y++, grid_y_pos -= step_size, grid_x_pos = grid_x_min)
		{
			for (size_t x = 0; x < target_res - 1; x++, grid_x_pos += step_size)
			{
				// Corner vertex order: 03
				//                      12
				// e.g.: clockwise, as in OpenGL
				g.vertex[0] = vertex_2(grid_x_pos, grid_y_pos);
				g.vertex[1] = vertex_2(grid_x_pos, grid_y_pos - step_size);
				g.vertex[2] = vertex_2(grid_x_pos + step_size, grid_y_pos - step_size);
				g.vertex[3] = vertex_2(grid_x_pos + step_size, grid_y_pos);

				g.value[0] = image[y * target_res + x];
				g.value[1] = image[(y + 1) * target_res + x];
				g.value[2] = image[(y + 1) * target_res + (x + 1)];
				g.value[3] = image[y * target_res + (x + 1)];

				g.generate_primitives(line_segments[i], triangles[i], isovalue);
			}
		}
	}

	return images;
}





vector<vector<float>> get_data(const vector<vector<float>>&src_images, size_t target_res)
{
	vector<vector<float>> images;

	srand(1234);

	inverse_width = 1.0f / template_width;
	step_size = template_width / static_cast<float>(target_res - 1);
	template_height = step_size * (target_res - 1);

	train_points.clear();
	line_segments.clear();
	triangles.clear();
	colours.clear();

	train_points.resize(type_count);
	line_segments.resize(type_count);
	triangles.resize(type_count);
	colours.resize(type_count);

	for (size_t i = 0; i < colours.size(); i++)
	{
		colours[i].r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colours[i].g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colours[i].b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}

	for (size_t i = 0; i < type_count; i++)
	{
		for (size_t j = 0; j < 5; j++)
		{
			float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			x -= 0.5f;
			y -= 0.5f;
			x *= template_width;
			y *= template_width;

			if (i == 0)
			{
				if (y < 0)
				{
					y = -y;
				}
			}
			else
			{
				if (y > 0)
				{
					y = -y;
				}
			}


			vertex_2 v;
			v.x = x;
			v.y = y;

			train_points[i].push_back(v);
		}
	}




	for (size_t i = 0; i < type_count; i++)
	{
		grid_x_min = -template_width * 0.5f;
		grid_y_max = template_height * 0.5f;

		// Generate geometric primitives using marching squares.
		grid_square g;

		float grid_x_pos = grid_x_min; // Start at minimum x.
		float grid_y_pos = grid_y_max; // Start at maximum y.

		vector<float> image(target_res * target_res, 0.0f);

		// Begin march.
		for (size_t y = 0; y < target_res; y++, grid_y_pos -= step_size, grid_x_pos = grid_x_min)
			for (size_t x = 0; x < target_res; x++, grid_x_pos += step_size)
				image[y * target_res + x] = src_images[i][y * target_res + x];// get_value(i, vertex_2(grid_x_pos, grid_y_pos));

		images.push_back(image);


		// Convert image to contours
		grid_x_pos = grid_x_min; // Start at minimum x.
		grid_y_pos = grid_y_max; // Start at maximum y.

		// Begin march.
		for (size_t y = 0; y < target_res - 1; y++, grid_y_pos -= step_size, grid_x_pos = grid_x_min)
		{
			for (size_t x = 0; x < target_res - 1; x++, grid_x_pos += step_size)
			{
				// Corner vertex order: 03
				//                      12
				// e.g.: clockwise, as in OpenGL
				g.vertex[0] = vertex_2(grid_x_pos, grid_y_pos);
				g.vertex[1] = vertex_2(grid_x_pos, grid_y_pos - step_size);
				g.vertex[2] = vertex_2(grid_x_pos + step_size, grid_y_pos - step_size);
				g.vertex[3] = vertex_2(grid_x_pos + step_size, grid_y_pos);

				g.value[0] = image[y * target_res + x];
				g.value[1] = image[(y + 1) * target_res + x];
				g.value[2] = image[(y + 1) * target_res + (x + 1)];
				g.value[3] = image[y * target_res + (x + 1)];

				g.generate_primitives(line_segments[i], triangles[i], isovalue);
			}
		}
	}

	return images;
}





#endif
