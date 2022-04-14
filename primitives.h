#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#include <cmath>

#include <vector>
using std::vector;

#include <set>
using std::set;

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>
using std::sort;

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
	size_t index;

	inline const void normalize(void)
	{
		float len = length();

		if (0.0f != len)
		{
			x /= len;
			y /= len;
		}
	}

	vertex_2(const float src_x = 0, const float src_y = 0)
	{
		x = src_x;
		y = src_y;
		index = 0;
	}

	//inline bool operator==(const vertex_2& right) const
	//{
	//	if (right.x == x && right.y == y)
	//		return true;
	//	else
	//		return false;
	//}

	bool operator==(const vertex_2 & rhs) const
	{
		float fx = fabsf(this->x - rhs.x);
		float fy = fabsf(this->y - rhs.y);

		if (fx < 0.0001 && fy < 0.0001)
			return true;

		return false;
	}

	inline const vertex_2& operator*(const float& right) const
	{
		static vertex_2 temp;

		temp.x = this->x * right;
		temp.y = this->y * right;

		return temp;
	}

	inline const vertex_2& operator/(const float& right) const
	{
		static vertex_2 temp;

		temp.x = this->x / right;
		temp.y = this->y / right;

		return temp;
	}

	inline bool operator<(const vertex_2& right) const
	{
		if (right.x > x)
			return true;
		else if (right.x < x)
			return false;

		if (right.y > y)
			return true;
		else if (right.y < y)
			return false;

		return false;
	}

	inline const vertex_2& operator+(const vertex_2& right) const
	{
		static vertex_2 temp;

		temp.x = this->x + right.x;
		temp.y = this->y + right.y;

		return temp;
	}

	inline const vertex_2& operator-(const vertex_2& right) const
	{
		static vertex_2 temp;

		temp.x = this->x - right.x;
		temp.y = this->y - right.y;

		return temp;
	}

	inline float dot(const vertex_2& right) const
	{
		return x * right.x + y * right.y;
	}

	inline const float self_dot(void)
	{
		return x * x + y * y;
	}

	inline const float length(void)
	{
		return sqrtf(self_dot());
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

	line_segment flip(void)
	{
		vertex_2 temp = vertex[0];
		vertex[0] = vertex[1];
		vertex[1] = temp;

		return *this;
	}

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


class tri_index
{
public:

	size_t prev_index, curr_index, next_index;

	tri_index(void)
	{
		prev_index = curr_index = next_index = 0;
	}

	inline bool operator<(const tri_index& right) const
	{
		if (right.prev_index > prev_index)
			return true;
		else if (right.prev_index < prev_index)
			return false;

		if (right.curr_index > curr_index)
			return true;
		else if (right.curr_index < curr_index)
			return false;

		if (right.next_index > next_index)
			return true;
		else if (right.next_index < next_index)
			return false;

		return false;
	}
};


// Marching squares-related geometric primitives
class line_segment_data
{
public:
	vector<line_segment> line_segments;
	map<size_t, vector<size_t> > line_segment_neighbours;
	vector<vertex_2> face_normals;
	vector<vertex_2> vertices;

	void process_line_segments(void)
	{
		face_normals.clear();
		vertices.clear();

		if (3 > line_segments.size())
			return;

		// Sort vertices, to gain some sense of order
		for (size_t i = 0; i < line_segments.size(); i++)
		{
			if (line_segments[i].vertex[1] < line_segments[i].vertex[0])
			{
				vertex_2 tv = line_segments[i].vertex[0];
				line_segments[i].vertex[0] = line_segments[i].vertex[1];
				line_segments[i].vertex[1] = tv;
			}
		}

		cout << "Welding vertices" << endl;

		// Insert unique vertices into set
		set<vertex_2> vertex_set;

		for (vector<line_segment>::const_iterator i = line_segments.begin(); i != line_segments.end(); i++)
		{
			vertex_set.insert(i->vertex[0]);
			vertex_set.insert(i->vertex[1]);
		}

		cout << "Vertices: " << vertex_set.size() << endl;

		cout << "Generating vertex indices" << endl;

		// Add indices to the vertices
		for (set<vertex_2>::const_iterator i = vertex_set.begin(); i != vertex_set.end(); i++)
		{
			size_t index = vertices.size();
			vertices.push_back(*i);
			vertices[index].index = index;
		}

		vertex_set.clear();

		// Re-insert modifies vertices into set
		for (vector<vertex_2>::const_iterator i = vertices.begin(); i != vertices.end(); i++)
			vertex_set.insert(*i);

		cout << "Assigning vertex indices to line segments" << endl;

		// Find the two vertices for each line segment, by index
		set<vertex_2>::iterator find_iter;

		for (vector<line_segment>::iterator i = line_segments.begin(); i != line_segments.end(); i++)
		{
			find_iter = vertex_set.find(i->vertex[0]);
			i->vertex[0].index = find_iter->index;

			find_iter = vertex_set.find(i->vertex[1]);
			i->vertex[1].index = find_iter->index;
		}

		vertex_set.clear();

		get_all_line_segment_neighbours();

		cout << "Calculating normals" << endl;
		face_normals.resize(line_segments.size());

		// Number of disconnected objects 
		size_t num_objects = 0;

		// Start with the first line segment
		size_t last_unprocessed_index = 0;

		// Keep track of which line segments have been processed
		set<size_t> unprocessed_indices;

		for (size_t i = 0; i < line_segments.size(); i++)
			unprocessed_indices.insert(i);

		// These will be used later for face normal calculation
		vector<tri_index> tri_indices;

		// Get the neighbours for each line segment
		do
		{
			const size_t first_index = last_unprocessed_index;

			size_t prev_index = first_index;
			size_t curr_index = line_segment_neighbours[first_index][0];
			size_t next_index = line_segment_neighbours[curr_index][0];

			tri_index t;
			t.prev_index = prev_index;
			t.curr_index = curr_index;
			t.next_index = next_index;
			tri_indices.push_back(t);
			unprocessed_indices.erase(unprocessed_indices.find(curr_index));

			// For each disconnected object in the image
			do
			{
				size_t index0 = line_segment_neighbours[curr_index][0];
				size_t index1 = line_segment_neighbours[curr_index][1];

				if (index0 == prev_index)
				{
					prev_index = curr_index;
					curr_index = index1;
					next_index = line_segment_neighbours[curr_index][1];

					if (prev_index == next_index)
						next_index = line_segment_neighbours[curr_index][0];
				}
				else if (index1 == prev_index)
				{
					prev_index = curr_index;
					curr_index = index0;
					next_index = line_segment_neighbours[curr_index][0];

					if (prev_index == next_index)
						next_index = line_segment_neighbours[curr_index][1];
				}

				tri_index t;
				t.prev_index = prev_index;
				t.curr_index = curr_index;
				t.next_index = next_index;
				tri_indices.push_back(t);
				unprocessed_indices.erase(unprocessed_indices.find(curr_index));

			} while (curr_index != first_index);

			// Check to see if .begin() will return a valid iterator
			// If so, use it
			// If not, then we're done!
			if (unprocessed_indices.size() > 0)
				last_unprocessed_index = (*unprocessed_indices.begin());

			if (num_objects % 10000 == 0)
				cout << "Found object " << num_objects + 1 << endl;

			num_objects++;

		} while (unprocessed_indices.size() > 0);


		// Finally, use the neighbour data to calculate the line segment normals
		for (size_t i = 0; i < tri_indices.size(); i++)
		{
			size_t prev_index = tri_indices[i].prev_index;
			size_t curr_index = tri_indices[i].curr_index;
			size_t next_index = tri_indices[i].next_index;

			size_t first_vertex_index = 0;
			size_t last_vertex_index = 0;

			if (line_segment_neighbours[prev_index][0] == curr_index)
				first_vertex_index = 0;
			else
				first_vertex_index = 1;

			if (line_segment_neighbours[next_index][0] == curr_index)
				last_vertex_index = 0;
			else
				last_vertex_index = 1;

			// Use the oriented neighbours to get the face normal
			vertex_2 edge = line_segments[prev_index].vertex[first_vertex_index] - line_segments[next_index].vertex[last_vertex_index];
			face_normals[curr_index] = vertex_2(-edge.y, edge.x);
			face_normals[curr_index].normalize();
		}

		cout << "Found " << num_objects << " object(s)." << endl;
	}

protected:
	void get_sorted_points_from_line_segment(size_t ls_index, vector<size_t>& points)
	{
		points.resize(2);

		vector<size_t> sorted_vertex_indices;

		// do end point 0 and 1
		sorted_vertex_indices.push_back(line_segments[ls_index].vertex[0].index);
		sorted_vertex_indices.push_back(line_segments[ls_index].vertex[1].index);
		sort(sorted_vertex_indices.begin(), sorted_vertex_indices.end());
		points[0] = sorted_vertex_indices[0];
		points[1] = sorted_vertex_indices[1];
	}

	void get_all_line_segment_neighbours(void)
	{
		line_segment_neighbours.clear();

		vector<size_t> default_lookup;

		for (size_t i = 0; i < line_segments.size(); i++)
			line_segment_neighbours[i] = default_lookup;

		cout << "Enumerating shared faces" << endl;

		map<size_t, vector<size_t> > neighbours;

		for (size_t i = 0; i < line_segments.size(); i++)
		{
			if (i % 10000 == 0)
				cout << i + 1 << " of " << line_segments.size() << endl;

			vector<size_t> points;
			get_sorted_points_from_line_segment(i, points);

			for (size_t j = 0; j < points.size(); j++)
				neighbours[points[j]].push_back(i);
		}

		cout << "Processing shared faces" << endl;

		size_t count = 0;

		for (map<size_t, vector<size_t> >::const_iterator ci = neighbours.begin(); ci != neighbours.end(); ci++)
		{
			if (count % 10000 == 0)
				cout << count + 1 << " of " << neighbours.size() << endl;

			count++;

			line_segment_neighbours[ci->second[0]].push_back(ci->second[1]);
			line_segment_neighbours[ci->second[1]].push_back(ci->second[0]);
		}
	}
};





#endif
