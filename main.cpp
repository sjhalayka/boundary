#include "main.h"

// Cat image from: http://www.iacuc.arizona.edu/training/cats/index.html
int main(int argc, char** argv)
{
	srand(123);

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
		for (size_t j = 0; j < 10; j++)
		{
			float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			x -= 0.5f;
			y -= 0.5f;

			vertex_2 v;
			v.x = x;
			v.y = y;

			train_points[i].push_back(v);
		}
	}

	const size_t marching_squares_resolution = 64;

	// Get amplitude mask width.
	template_width = 1.0;
	inverse_width = 1.0 / template_width;
	step_size = template_width / static_cast<double>(marching_squares_resolution - 1);
	template_height = step_size * (marching_squares_resolution - 1); // Assumes square image.

	// Get marching squares isovalue.
	isovalue = 0;

	for (size_t i = 0; i < type_count; i++)
	{
		grid_x_min = -template_width / 2.0;
		grid_y_max = template_height / 2.0;

		// Generate geometric primitives using marching squares.
		grid_square g;

		double grid_x_pos = grid_x_min; // Start at minimum x.
		double grid_y_pos = grid_y_max; // Start at maximum y.

		// Begin march.
		for (short unsigned int y = 0; y < marching_squares_resolution - 1; y++, grid_y_pos -= step_size, grid_x_pos = grid_x_min)
		{
			for (short unsigned int x = 0; x < marching_squares_resolution - 1; x++, grid_x_pos += step_size)
			{
				// Corner vertex order: 03
				//                      12
				// e.g.: clockwise, as in OpenGL
				g.vertex[0] = vertex_2(grid_x_pos, grid_y_pos);
				g.vertex[1] = vertex_2(grid_x_pos, grid_y_pos - step_size);
				g.vertex[2] = vertex_2(grid_x_pos + step_size, grid_y_pos - step_size);
				g.vertex[3] = vertex_2(grid_x_pos + step_size, grid_y_pos);

				g.value[0] = get_value(i, g.vertex[0]);
				g.value[1] = get_value(i, g.vertex[1]);
				g.value[2] = get_value(i, g.vertex[2]);
				g.value[3] = get_value(i, g.vertex[3]);

				if (x == 0)
				{
					g.value[0] = -1;
					g.value[1] = -1;
				}

				if (x == marching_squares_resolution - 2)
				{
					g.value[2] = -1;
					g.value[3] = -1;
				}

				if (y == 0)
				{
					g.value[0] = -1;
					g.value[3] = -1;
				}

				if (y == marching_squares_resolution - 2)
				{
					g.value[1] = -1;
					g.value[2] = -1;
				}

				g.generate_primitives(line_segments[i], triangles[i], isovalue);
			}
		}

	}

	// Render the Targa image underneath the associated geometric primitives,
	// using OpenGL fixed-pipeline functionality.
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
		glVertex2f(-template_width/2.0, template_height/2.0);
		glVertex2f(-template_width/2.0, -template_height/2.0);
		glVertex2f(template_width/2.0, -template_height/2.0);
		glVertex2f(template_width/2.0, template_height/2.0);
	glEnd();

	// Render image area.





	glBegin(GL_TRIANGLES);

	for (size_t i = 0; i < triangles.size(); i++)
	{
		glColor3f(
			(colours[i].r + 1.0) / 2.0f, 
			(colours[i].g + 1.0) / 2.0f,
			(colours[i].b + 1.0) / 2.0f);

		for (size_t j = 0; j < triangles[i].size(); j++)
		{
			glVertex2f(triangles[i][j].vertex[0].x, triangles[i][j].vertex[0].y);
			glVertex2f(triangles[i][j].vertex[1].x, triangles[i][j].vertex[1].y);
			glVertex2f(triangles[i][j].vertex[2].x, triangles[i][j].vertex[2].y);
		}
	}
	glEnd();

	// Render image outline edge length.
	glLineWidth(2);
	glBegin(GL_LINES);

	for(size_t i = 0; i < line_segments.size(); i++)
	{
		glColor3f(colours[i].r, colours[i].g, colours[i].b);

		for (size_t j = 0; j < line_segments[i].size(); j++)
		{
			glVertex2f(line_segments[i][j].vertex[0].x, line_segments[i][j].vertex[0].y);
			glVertex2f(line_segments[i][j].vertex[1].x, line_segments[i][j].vertex[1].y);
		}
	}

	glEnd();
	glLineWidth(1);


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
	glEnd();

	glFlush();
}
