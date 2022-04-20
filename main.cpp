#include "main.h"

// Cat image from: http://www.iacuc.arizona.edu/training/cats/index.html
int main(int argc, char** argv)
{
	vector<vector<float>> two_by_two = get_data(2);
	vector<vector<float>> res_by_res = get_data(marching_squares_resolution);

	for (size_t i = 0; i < two_by_two.size(); i++)
		//two_by_two[i] = resize_from_2by2(two_by_two[i], marching_squares_resolution);
		two_by_two[i] = opencv_resize(two_by_two[i], marching_squares_resolution);


	vector<vector<float>> morphed_images = opencv_lerp(two_by_two, res_by_res, marching_squares_resolution, 0);

	if(marching_squares_resolution == 2)
		two_by_two = get_data(two_by_two, marching_squares_resolution);
	else
		res_by_res = get_data(morphed_images, marching_squares_resolution);



	//float_grayscale luma;


	//luma.px = marching_squares_resolution;
	//luma.py = marching_squares_resolution;
	//luma.pixel_data = image;
	//write_float_grayscale_to_tga("image.tga", luma);


	//luma.px = marching_squares_resolution;
	//luma.py = marching_squares_resolution;
	//luma.pixel_data = image2;
	//write_float_grayscale_to_tga("image2.tga", luma);




	if (false == get_index(test_point_index))
		test_point_index = get_closest_index(test_point);


	vector<contour> contours;

	for (size_t i = 0; i < line_segments[0].size(); i++)
	{
		contour c;
		line_segment ls = line_segments[0][i];

		//if (line_segments[0][i].vertex[1] < line_segments[0][i].vertex[0])
		//{
		//	vertex_2 vtemp = line_segments[0][i].vertex[0];
		//	line_segments[0][i].vertex[0] = line_segments[0][i].vertex[1];
		//	line_segments[0][i].vertex[1] = vtemp;

		//}

		c.d.push_back(ls);
		contours.push_back(c);
	}

	//cout << contours.size() << endl;

	while (contours.size() > 0)
		merge_contours(contours, final_contours);

	//for (size_t i = 0; i < 10; i++)
	//{
	//	contours = final_contours;
	//	final_contours.clear();

	//	while (contours.size() > 0)
	//		merge_contours(contours, final_contours);
	//}


	//cout << final_contours.size() << endl;
	
	// Get normals
	for (size_t i = 0; i < final_contours.size(); i++)
	{
		vector<vertex_2> n;

		for (size_t j = 0; j < final_contours[i].d.size(); j++)
		{
			vertex_2 edge = final_contours[i].d[j].vertex[1] - final_contours[i].d[j].vertex[0];
			
			vertex_2 normal = vertex_2(-edge.y, edge.x);
			normal.normalize();
			n.push_back(normal);
		}

		normals.push_back(n);
	}

	for (size_t i = 0; i < final_contours.size(); i++)
	{
		float per_contour_curvature = 0;

		if (final_contours[i].d.size() < 2)
		{
			cout << "zero " << per_contour_curvature << endl;
			continue;
		}

		const vertex_2 first_end_vertex = final_contours[i].d[0].vertex[0];
		const vertex_2 last_end_vertex = final_contours[i].d[final_contours[i].d.size() - 1].vertex[1];
		const bool is_closed = (first_end_vertex == last_end_vertex);

		for (size_t j = 0; j < final_contours[i].d.size(); j++)
		{
			vertex_2 normal = normals[i][j];

			// Set these by default
			vertex_2 prev_normal = normal;
			vertex_2 next_normal = normal;

			if (j == 0)
			{
				if (is_closed)
				{
					prev_normal = normals[i][final_contours[i].d.size() - 1];
					next_normal = normals[i][j + 1];
				}
			}
			else if (j == final_contours[i].d.size() - 1)
			{
				if (is_closed)
				{
					prev_normal = normals[i][j - 1];
					next_normal = normals[i][0];
				}
			}
			else
			{
				prev_normal = normals[i][j - 1];
				next_normal = normals[i][j + 1];
			}

			// Calculate curvature here
			float d_i = normal.dot(prev_normal) + normal.dot(next_normal);
			d_i /= 2.0f;

			// Normalize the average dot product to get the curvature
			float k_i = (1.0f - d_i) / 2.0f;

			per_contour_curvature += k_i;
		}

		if(is_closed)
			cout << "closed non-zero " << per_contour_curvature << endl;
		else
			cout << "open non-zero " << per_contour_curvature << endl;
	}


	render_image(argc, argv);

	return 0;
}

void render_image(int &argc, char ** &argv)
{
	// Initialize OpenGL.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("Marching Squares");
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);

	glShadeModel(GL_FLAT);
	glClearColor(background_colour, background_colour, background_colour, 1);

	// Begin rendering.
	glutMainLoop();

	// Cleanup OpenGL.
	glutDestroyWindow(win_id);
}

void idle_func(void)
{
	glutPostRedisplay();
}

// Resize window.
void reshape_func(int width, int height)
{
	if(width < 1)
		width = 1;

	if(height < 1)
		height = 1;

	win_x = width;
	win_y = height;

	// Viewport setup.
	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, static_cast<GLfloat>(win_x)/static_cast<GLfloat>(win_y), 0.1, 10);
	gluLookAt(0, 0, camera_z, 0, 0, 0, 0, 1, 0);
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	{
		test_point.y += 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	case 's':
	{
		test_point.y -= 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	case 'a':
	{
		test_point.x -= 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	case 'd':
	{
		test_point.x += 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	default:
		break;
	}
}

// Visualization.
void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Scale all geometric primitives so that template width == 1.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(inverse_width, inverse_width, inverse_width);

	// Render a dark background.
	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
		glVertex2f(-template_width*0.5f, template_height * 0.5f);
		glVertex2f(-template_width * 0.5f, -template_height * 0.5f);
		glVertex2f(template_width * 0.5f, -template_height * 0.5f);
		glVertex2f(template_width * 0.5f, template_height * 0.5f);
	glEnd();

	// Render image area.





	glBegin(GL_TRIANGLES);

	for (size_t i = 0; i < triangles.size(); i++)
	{
		glColor3f(
			(colours[i].r + 1.0f) / 2.0f, 
			(colours[i].g + 1.0f) / 2.0f,
			(colours[i].b + 1.0f) / 2.0f);

		for (size_t j = 0; j < triangles[i].size(); j++)
		{
			glVertex2f(triangles[i][j].vertex[0].x, triangles[i][j].vertex[0].y);
			glVertex2f(triangles[i][j].vertex[1].x, triangles[i][j].vertex[1].y);
			glVertex2f(triangles[i][j].vertex[2].x, triangles[i][j].vertex[2].y);
		}
	}
	glEnd();



	//// Draw triangle outlines
	//glBegin(GL_LINES);

	//for (size_t i = 0; i < triangles.size(); i++)
	//{
	//	glColor3f(
	//		(colours[i].r),
	//		(colours[i].g),
	//		(colours[i].b));

	//	for (size_t j = 0; j < triangles[i].size(); j++)
	//	{
	//		glVertex2f(triangles[i][j].vertex[0].x, triangles[i][j].vertex[0].y);
	//		glVertex2f(triangles[i][j].vertex[1].x, triangles[i][j].vertex[1].y);

	//		glVertex2f(triangles[i][j].vertex[1].x, triangles[i][j].vertex[1].y);
	//		glVertex2f(triangles[i][j].vertex[2].x, triangles[i][j].vertex[2].y);

	//		glVertex2f(triangles[i][j].vertex[2].x, triangles[i][j].vertex[2].y);
	//		glVertex2f(triangles[i][j].vertex[0].x, triangles[i][j].vertex[0].y);

	//	}
	//}
	//glEnd();



	glLineWidth(1);
	glBegin(GL_LINES);

	//srand(1234);


	// Draw contours
	for (size_t i = 0; i < final_contours.size(); i++)
	{
		glColor3f(0, 0, 0);// static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));// colours[i].r, colours[i].g, colours[i].b);

		for (size_t j = 0; j < final_contours[i].d.size(); j++)
		{
			glVertex2f(final_contours[i].d[j].vertex[0].x, final_contours[i].d[j].vertex[0].y);
			glVertex2f(final_contours[i].d[j].vertex[1].x, final_contours[i].d[j].vertex[1].y);

			// Draw normal
			//vertex_2 v((final_contours[i].d[j].vertex[0].x + final_contours[i].d[j].vertex[1].x) * 0.5f,
			//	(final_contours[i].d[j].vertex[0].y + final_contours[i].d[j].vertex[1].y) * 0.5f);

			//glVertex2f(v.x, v.y);
			//glVertex2f(v.x + normals[i][j].x / 50, v.y + normals[i][j].y / 50);
		}
	}

	glEnd();
	glLineWidth(1);



	// Render image outline edges.
	//glLineWidth(2);
	//glBegin(GL_LINES);

	//for(size_t i = 0; i < line_segments.size(); i++)
	//{
	//	glColor3f(colours[i].r, colours[i].g, colours[i].b);

	//	for (size_t j = 0; j < line_segments[i].size(); j++)
	//	{
	//		glVertex2f(line_segments[i][j].vertex[0].x, line_segments[i][j].vertex[0].y);
	//		glVertex2f(line_segments[i][j].vertex[1].x, line_segments[i][j].vertex[1].y);
	//	}
	//}

	//glEnd();
	//glLineWidth(1);


	glPointSize(12);

	glBegin(GL_POINTS);

	for (size_t i = 0; i < train_points.size(); i++)
	{
		glColor3f(0, 0, 0);

		for (size_t j = 0; j < train_points[i].size(); j++)
		{
			glVertex2f(train_points[i][j].x, train_points[i][j].y);
		}
	}
	glEnd();

	glPointSize(8);

	glBegin(GL_POINTS);

	for (size_t i = 0; i < train_points.size(); i++)
	{
		glColor3f(colours[i].r, colours[i].g, colours[i].b);

		for (size_t j = 0; j < train_points[i].size(); j++)
		{
			glVertex2f(train_points[i][j].x, train_points[i][j].y);
		}
	}

	//glColor3f(colours[test_point_index].r, colours[test_point_index].g, colours[test_point_index].b);
	//glVertex2f(test_point.x, test_point.y);

	glEnd();

	glFlush();
}
