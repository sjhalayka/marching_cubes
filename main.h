#pragma once





#include <GL/glew.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/misc/cpp/imgui_stdlib.h"


#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>


#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32.lib")

#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "opencv_world4100.lib")

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>	
#include <random>
#include <set>
#include <map>
using namespace std;


#include <opencv2/opencv.hpp>
using namespace cv;


#include "vertex_fragment_shader.h"
#include "vertex_geometry_fragment_shader.h"


vertex_fragment_shader ortho_shader;
vertex_geometry_fragment_shader line_shader;



struct
{
	struct
	{
		GLint tex;
		GLint viewport_width;
		GLint viewport_height;
	}
	ortho_shader_uniforms;

	struct
	{
		GLint colour;
		GLint img_width;
		GLint img_height;
		GLint line_thickness;
	}
	line_shader_uniforms;
}
uniforms;


bool operator< (const glm::vec3& lhs, const glm::vec3& rhs)
{
	return glm::all(glm::lessThan(lhs, rhs));
}

bool operator> (const glm::vec3& lhs, const glm::vec3& rhs)
{
	return glm::all(glm::lessThan(lhs, rhs));
}

class sortable_line_segment
{
public:


	inline bool operator==(const sortable_line_segment& right) const
	{
		if (right.vertices[0] == vertices[0] && right.vertices[1] == vertices[1])
			return true;
		else
			return false;
	}


	inline bool operator<(const sortable_line_segment& right) const
	{
		if (right.vertices[0] > vertices[0])
			return true;
		else if (right.vertices[0] < vertices[0])
			return false;

		if (right.vertices[1] > vertices[1])
			return true;
		else if (right.vertices[1] < vertices[1])
			return false;


		return false;
	}

	void sort(void)
	{

		if (vertices[1] < vertices[0])
		{
			glm::vec3 tempv = vertices[1];
			vertices[1] = vertices[0];
			vertices[0] = tempv;
		}


	}

	glm::vec3 vertices[2];
};


class quad
{
public:
	glm::vec3 vertices[4];
};

// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
// http://geomalgorithms.com/a06-_intersect-2.html



// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(Mat& img, const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;

	img = imread(filename, IMREAD_UNCHANGED);

	if (img.empty())
		return false;

	image_width = img.cols;
	image_height = img.rows;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Upload pixels into texture
	if (img.channels() == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
	else if (img.channels() == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
	else
		return false;

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}








// https://stackoverflow.com/questions/65091012/how-to-split-an-image-into-m-x-n-tiles
std::vector<cv::Mat> splitImage(cv::Mat& image, int M, int N)
{
	// All images should be the same size ...
	int width = image.cols / M;
	int height = image.rows / N;
	// ... except for the Mth column and the Nth row
	int width_last_column = width + (image.cols % width);
	int height_last_row = height + (image.rows % height);

	std::vector<cv::Mat> result;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
		{
			// Compute the region to crop from
			cv::Rect roi(width * j,
				height * i,
				(j == (M - 1)) ? width_last_column : width,
				(i == (N - 1)) ? height_last_row : height);

			result.push_back(image(roi));
		}
	}

	return result;
}


vector<string> left_strings;
int left_selected = -1;
vector<ImVec2> left_uv_mins;
vector<ImVec2> left_uv_maxs;

void left_add_button_func(void)
{
	left_strings.push_back("1.0");
	left_uv_mins.push_back(ImVec2(0, 0));
	left_uv_maxs.push_back(ImVec2(0, 0));

	if (left_strings.size() == 1)
		left_selected = 0;
}

void left_remove_button_func(int i)
{
	left_strings.erase(left_strings.begin() + i);
	left_uv_mins.erase(left_uv_mins.begin() + i);
	left_uv_maxs.erase(left_uv_maxs.begin() + i);

	if (i == left_selected)
		left_selected = -1;
}





vector<string> right_strings;
int right_selected = -1;
vector<ImVec2> right_uv_mins;
vector<ImVec2> right_uv_maxs;


void right_add_button_func(void)
{
	right_strings.push_back("1.0");
	right_uv_mins.push_back(ImVec2(0, 0));
	right_uv_maxs.push_back(ImVec2(0, 0));

	if (right_strings.size() == 1)
		right_selected = 0;
}

void right_remove_button_func(int i)
{
	right_strings.erase(right_strings.begin() + i);
	right_uv_mins.erase(right_uv_mins.begin() + i);
	right_uv_maxs.erase(right_uv_maxs.begin() + i);

	if (i == right_selected)
		right_selected = -1;
}



ImVec2 image_anchor(0, 0);


// http://www.songho.ca/opengl/gl_transform.html

complex<float> get_window_coords_from_ndc_coords(size_t viewport_width, size_t viewport_height, complex<float>& src_coords)
{
	float x_w = viewport_width / 2.0f * src_coords.real() + viewport_width / 2.0f;
	float y_w = viewport_height / 2.0f * src_coords.imag() + viewport_height / 2.0f;

	return complex<float>(x_w, y_w);
}

complex<float> get_ndc_coords_from_window_coords(size_t viewport_width, size_t viewport_height, complex<float>& src_coords)
{
	float x_ndc = (2.0f * src_coords.real() / viewport_width) - 1.0f;
	float y_ndc = (2.0f * src_coords.imag() / viewport_height) - 1.0f;

	return complex<float>(x_ndc, y_ndc);
}


class background_chunk
{
public:
	vector<ImVec2> indices;
	ImVec2 region_min;
	ImVec2 region_max;
};

class background_tile
{
public:
	int tile_size;
	ImVec2 uv_min;
	ImVec2 uv_max;
};

int tiles_per_dimension = 2000;
int tiles_per_chunk_dimension = 5;


vector<background_chunk> background_chunks;
vector<background_tile> background_tiles;

float zoom_factor = 1.0f;
float last_mousewheel = 0.0f;


bool point_in_quad(const glm::vec3& point, const quad& q)
{
	if (point.x < q.vertices[0].x || point.x > q.vertices[3].x)
		return false;

	if (point.y < q.vertices[2].y || point.y > q.vertices[3].y)
		return false;

	return true;
}

bool point_in_polygon(glm::vec3 point, vector<glm::vec3> polygon)
{
	const size_t num_vertices = polygon.size();
	double x = point.x, y = point.y;
	bool inside = false;

	// Store the first point in the polygon and initialize
	// the second point
	glm::vec3 p1 = polygon[0], p2;

	// Loop through each edge in the polygon
	for (int i = 1; i <= num_vertices; i++) {
		// Get the next point in the polygon
		p2 = polygon[i % num_vertices];

		// Check if the point is above the minimum y
		// coordinate of the edge
		if (y > min(p1.y, p2.y)) {
			// Check if the point is below the maximum y
			// coordinate of the edge
			if (y <= max(p1.y, p2.y)) {
				// Check if the point is to the left of the
				// maximum x coordinate of the edge
				if (x <= max(p1.x, p2.x)) {
					// Calculate the x-intersection of the
					// line connecting the point to the edge
					double x_intersection
						= (y - p1.y) * (p2.x - p1.x)
						/ (p2.y - p1.y)
						+ p1.x;

					// Check if the point is on the same
					// line as the edge or to the left of
					// the x-intersection
					if (p1.x == p2.x
						|| x <= x_intersection) {
						// Flip the inside flag
						inside = !inside;
					}
				}
			}
		}

		// Store the current point as the first point for
		// the next iteration
		p1 = p2;
	}

	// Return the value of the inside flag
	return inside;
}









bool draw_textured_quad(bool quit_upon_collision, int mouse_x, int mouse_y, vector<quad>& quads, GLuint shader_program, long signed int x, long signed int y, long signed int tile_size, long signed int win_width, long signed int win_height, GLuint tex_handle, ImVec2 uv_min, ImVec2 uv_max)
{
	complex<float> v0w(static_cast<float>(x), static_cast<float>(y));
	complex<float> v1w(static_cast<float>(x), static_cast<float>(y + tile_size));
	complex<float> v2w(static_cast<float>(x + tile_size), static_cast<float>(y + tile_size));
	complex<float> v3w(static_cast<float>(x + tile_size), static_cast<float>(y));

	v0w.real(v0w.real() * zoom_factor);
	v0w.imag(v0w.imag() * zoom_factor);
	v1w.real(v1w.real() * zoom_factor);
	v1w.imag(v1w.imag() * zoom_factor);
	v2w.real(v2w.real() * zoom_factor);
	v2w.imag(v2w.imag() * zoom_factor);
	v3w.real(v3w.real() * zoom_factor);
	v3w.imag(v3w.imag() * zoom_factor);



	complex<float> v0ndc = get_ndc_coords_from_window_coords(win_width, win_height, v0w);
	complex<float> v1ndc = get_ndc_coords_from_window_coords(win_width, win_height, v1w);
	complex<float> v2ndc = get_ndc_coords_from_window_coords(win_width, win_height, v2w);
	complex<float> v3ndc = get_ndc_coords_from_window_coords(win_width, win_height, v3w);

	size_t count = 0;

	if (!(v0ndc.real() < -1 || v0ndc.real() > 1 || v0ndc.imag() < -1 || v0ndc.imag() > 1))
		count++;

	if (!(v1ndc.real() < -1 || v1ndc.real() > 1 || v1ndc.imag() < -1 || v1ndc.imag() > 1))
		count++;

	if (!(v2ndc.real() < -1 || v2ndc.real() > 1 || v2ndc.imag() < -1 || v2ndc.imag() > 1))
		count++;

	if (!(v3ndc.real() < -1 || v3ndc.real() > 1 || v3ndc.imag() < -1 || v3ndc.imag() > 1))
		count++;

	if (count == 0)
		return false;


	quad q;
	q.vertices[0].x = v0w.real();
	q.vertices[0].y = win_height - v0w.imag();
	q.vertices[1].x = v1w.real();
	q.vertices[1].y = win_height - v1w.imag();
	q.vertices[2].x = v2w.real();
	q.vertices[2].y = win_height - v2w.imag();
	q.vertices[3].x = v3w.real();
	q.vertices[3].y = win_height - v3w.imag();
	quads.push_back(q);

	vector<glm::vec3> points;
	points.push_back(q.vertices[0]);
	points.push_back(q.vertices[1]);
	points.push_back(q.vertices[2]);
	points.push_back(q.vertices[3]);

	glm::vec3 mouse_pos(mouse_x, mouse_y, 0);

	bool inside = point_in_polygon(mouse_pos, points);

	if (quit_upon_collision)
		return inside;

	static GLuint vao = 0, vbo = 0, ibo = 0;

	if (!glIsVertexArray(vao))
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
	}

	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const GLfloat vertexData[] = {
		//	  X             Y             Z		  U         V     
			  v0ndc.real(), v0ndc.imag(), 0,      uv_min.x, uv_max.y,
			  v1ndc.real(), v1ndc.imag(), 0,      uv_min.x, uv_min.y,
			  v2ndc.real(), v2ndc.imag(), 0,      uv_max.x, uv_min.y,
			  v3ndc.real(), v3ndc.imag(), 0,      uv_max.x, uv_max.y,
	};

	// https://raw.githubusercontent.com/progschj/OpenGL-Examples/master/03texture.cpp

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 5, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char*)0 + 3 * sizeof(GLfloat));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	static const GLuint indexData[] = {
		3,1,0,
		2,1,3,
	};

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 2 * 3, indexData, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glUseProgram(ortho_shader.get_program());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_handle);

	glUniform1i(uniforms.ortho_shader_uniforms.tex, 0);
	glUniform1i(uniforms.ortho_shader_uniforms.viewport_width, win_width);
	glUniform1i(uniforms.ortho_shader_uniforms.viewport_height, win_height);

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);




	return inside;
}


void draw_quad_line_loop(glm::vec3 colour, int win_width, int win_height, float line_thickness, quad q)
{
	complex<float> v0w(static_cast<float>(q.vertices[0].x), static_cast<float>(q.vertices[0].y));
	complex<float> v1w(static_cast<float>(q.vertices[1].x), static_cast<float>(q.vertices[1].y));
	complex<float> v2w(static_cast<float>(q.vertices[2].x), static_cast<float>(q.vertices[2].y));
	complex<float> v3w(static_cast<float>(q.vertices[3].x), static_cast<float>(q.vertices[3].y));

	complex<float> v0ndc = get_ndc_coords_from_window_coords(win_width, win_height, v0w);
	complex<float> v1ndc = get_ndc_coords_from_window_coords(win_width, win_height, v1w);
	complex<float> v2ndc = get_ndc_coords_from_window_coords(win_width, win_height, v2w);
	complex<float> v3ndc = get_ndc_coords_from_window_coords(win_width, win_height, v3w);

	size_t count = 0;

	if (!(v0ndc.real() < -1 || v0ndc.real() > 1 || v0ndc.imag() < -1 || v0ndc.imag() > 1))
		count++;

	if (!(v1ndc.real() < -1 || v1ndc.real() > 1 || v1ndc.imag() < -1 || v1ndc.imag() > 1))
		count++;

	if (!(v2ndc.real() < -1 || v2ndc.real() > 1 || v2ndc.imag() < -1 || v2ndc.imag() > 1))
		count++;

	if (!(v3ndc.real() < -1 || v3ndc.real() > 1 || v3ndc.imag() < -1 || v3ndc.imag() > 1))
		count++;

	if (count == 0)
		return;

	glUseProgram(line_shader.get_program());

	glUniform3f(uniforms.line_shader_uniforms.colour, colour.x, colour.y, colour.z);
	glUniform1i(uniforms.line_shader_uniforms.img_width, win_width);
	glUniform1i(uniforms.line_shader_uniforms.img_height, win_height);
	glUniform1f(uniforms.line_shader_uniforms.line_thickness, line_thickness);

	GLuint components_per_vertex = 3;
	GLuint components_per_position = 3;

	static GLuint axis_buffer = 0;

	if (!glIsBuffer(axis_buffer))
		glGenBuffers(1, &axis_buffer);

	vector<GLfloat> flat_data;
	flat_data.push_back(v0ndc.real());
	flat_data.push_back(v0ndc.imag());
	flat_data.push_back(0.0f);

	flat_data.push_back(v1ndc.real());
	flat_data.push_back(v1ndc.imag());
	flat_data.push_back(0.0f);

	flat_data.push_back(v2ndc.real());
	flat_data.push_back(v2ndc.imag());
	flat_data.push_back(0.0f);

	flat_data.push_back(v3ndc.real());
	flat_data.push_back(v3ndc.imag());
	flat_data.push_back(0.0f);

	GLuint num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(line_shader.get_program(), "position"));
	glVertexAttribPointer(glGetAttribLocation(line_shader.get_program(), "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glDrawArrays(GL_LINE_LOOP, 0, num_vertices);

	//glDeleteBuffers(1, &axis_buffer);
}








void draw_circle_line_loop(glm::vec3 colour, int win_width, int win_height, float line_thickness, glm::vec3 centre_point, float radius, float steps)
{
	static const float pi = 4.0f * atanf(1.0f);

	vector<complex<float>> circle_points;

	float slice = 2.0f * pi / steps;

	for (int i = 0; i < steps; i++)
	{
		float angle = slice * i;
		float x = centre_point.x + radius * cos(angle);
		float y = centre_point.y + radius * sin(angle);

		circle_points.push_back(complex<float>(x, y));
	}


	vector<complex<float>> v_points;

	for (int i = 0; i < circle_points.size(); i++)
	{
		complex<float> v(static_cast<float>(circle_points[i].real()), static_cast<float>(circle_points[i].imag()));
		v_points.push_back(v);
	}

	vector<complex<float>> ndc_points;

	for (int i = 0; i < v_points.size(); i++)
	{
		complex<float> ndc = get_ndc_coords_from_window_coords(win_width, win_height, v_points[i]);
		ndc_points.push_back(ndc);
	}



	glUseProgram(line_shader.get_program());

	glUniform3f(uniforms.line_shader_uniforms.colour, colour.x, colour.y, colour.z);
	glUniform1i(uniforms.line_shader_uniforms.img_width, win_width);
	glUniform1i(uniforms.line_shader_uniforms.img_height, win_height);
	glUniform1f(uniforms.line_shader_uniforms.line_thickness, line_thickness);


	GLuint components_per_vertex = 3;
	GLuint components_per_position = 3;

	GLuint axis_buffer;

	glGenBuffers(1, &axis_buffer);


	vector<GLfloat> flat_data;

	for (int i = 0; i < ndc_points.size(); i++)
	{
		flat_data.push_back(ndc_points[i].real());
		flat_data.push_back(ndc_points[i].imag());
		flat_data.push_back(0.0f);
	}


	GLuint num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;


	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(line_shader.get_program(), "position"));
	glVertexAttribPointer(glGetAttribLocation(line_shader.get_program(), "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glDrawArrays(GL_LINE_LOOP, 0, num_vertices);

	glDeleteBuffers(1, &axis_buffer);
}


//
//void draw_line_segment(glm::vec3 colour, int win_width, int win_height, float line_thickness, const sortable_line_segment& ls)
//{
//	glUseProgram(line_shader.get_program());
//
//	glUniform3f(uniforms.line_shader_uniforms.colour, colour.x, colour.y, colour.z);
//	glUniform1i(uniforms.line_shader_uniforms.img_width, win_width);
//	glUniform1i(uniforms.line_shader_uniforms.img_height, win_height);
//	glUniform1f(uniforms.line_shader_uniforms.line_thickness, line_thickness);
//
//	GLuint components_per_vertex = 3;
//	GLuint components_per_position = 3;
//
//	GLuint axis_buffer;
//
//	glGenBuffers(1, &axis_buffer);
//
//	complex<float> v0w(static_cast<float>(ls.vertices[0].x), static_cast<float>(ls.vertices[0].y));
//	complex<float> v1w(static_cast<float>(ls.vertices[1].x), static_cast<float>(ls.vertices[1].y));
//
//	complex<float> v0ndc = get_ndc_coords_from_window_coords(win_width, win_height, v0w);
//	complex<float> v1ndc = get_ndc_coords_from_window_coords(win_width, win_height, v1w);
//
//
//	vector<GLfloat> flat_data;
//	flat_data.push_back(v0ndc.real());
//	flat_data.push_back(v0ndc.imag());
//	flat_data.push_back(0.0f);
//
//	flat_data.push_back(v1ndc.real());
//	flat_data.push_back(v1ndc.imag());
//	flat_data.push_back(0.0f);
//
//	GLuint num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;
//
//	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
//	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(glGetAttribLocation(line_shader.get_program(), "position"));
//	glVertexAttribPointer(glGetAttribLocation(line_shader.get_program(), "position"),
//		components_per_position,
//		GL_FLOAT,
//		GL_FALSE,
//		components_per_vertex * sizeof(GLfloat),
//		NULL);
//
//	glDrawArrays(GL_LINES, 0, num_vertices);
//
//	glDeleteBuffers(1, &axis_buffer);
//}


bool get_quad_ndc_data(vector<float>& vertex_data, vector<GLuint>& index_data, long signed int x, long signed int y, long signed int tile_size, long signed int win_width, long signed int win_height, ImVec2 uv_min, ImVec2 uv_max)
{
	complex<float> v0w(static_cast<float>(x), static_cast<float>(y));
	complex<float> v1w(static_cast<float>(x), static_cast<float>(y + tile_size));
	complex<float> v2w(static_cast<float>(x + tile_size), static_cast<float>(y + tile_size));
	complex<float> v3w(static_cast<float>(x + tile_size), static_cast<float>(y));

	v0w.real(v0w.real() * zoom_factor);
	v0w.imag(v0w.imag() * zoom_factor);
	v1w.real(v1w.real() * zoom_factor);
	v1w.imag(v1w.imag() * zoom_factor);
	v2w.real(v2w.real() * zoom_factor);
	v2w.imag(v2w.imag() * zoom_factor);
	v3w.real(v3w.real() * zoom_factor);
	v3w.imag(v3w.imag() * zoom_factor);

	complex<float> v0ndc = get_ndc_coords_from_window_coords(win_width, win_height, v0w);
	complex<float> v1ndc = get_ndc_coords_from_window_coords(win_width, win_height, v1w);
	complex<float> v2ndc = get_ndc_coords_from_window_coords(win_width, win_height, v2w);
	complex<float> v3ndc = get_ndc_coords_from_window_coords(win_width, win_height, v3w);

	size_t count = 0;

	if (!(v0ndc.real() < -1 || v0ndc.real() > 1 || v0ndc.imag() < -1 || v0ndc.imag() > 1))
		count++;

	if (!(v1ndc.real() < -1 || v1ndc.real() > 1 || v1ndc.imag() < -1 || v1ndc.imag() > 1))
		count++;

	if (!(v2ndc.real() < -1 || v2ndc.real() > 1 || v2ndc.imag() < -1 || v2ndc.imag() > 1))
		count++;

	if (!(v3ndc.real() < -1 || v3ndc.real() > 1 || v3ndc.imag() < -1 || v3ndc.imag() > 1))
		count++;

	if (count == 0)
		return false;

	vertex_data.push_back(v0ndc.real());
	vertex_data.push_back(v0ndc.imag());
	vertex_data.push_back(0);
	vertex_data.push_back(uv_min.x);
	vertex_data.push_back(uv_max.y);

	vertex_data.push_back(v1ndc.real());
	vertex_data.push_back(v1ndc.imag());
	vertex_data.push_back(0);
	vertex_data.push_back(uv_min.x);
	vertex_data.push_back(uv_min.y);

	vertex_data.push_back(v2ndc.real());
	vertex_data.push_back(v2ndc.imag());
	vertex_data.push_back(0);
	vertex_data.push_back(uv_max.x);
	vertex_data.push_back(uv_min.y);

	vertex_data.push_back(v3ndc.real());
	vertex_data.push_back(v3ndc.imag());
	vertex_data.push_back(0);
	vertex_data.push_back(uv_max.x);
	vertex_data.push_back(uv_max.y);

	size_t vertex_data_index = vertex_data.size();

	index_data.push_back(0 + vertex_data_index / 5 - 4);
	index_data.push_back(1 + vertex_data_index / 5 - 4);
	index_data.push_back(2 + vertex_data_index / 5 - 4);
	index_data.push_back(0 + vertex_data_index / 5 - 4);
	index_data.push_back(2 + vertex_data_index / 5 - 4);
	index_data.push_back(3 + vertex_data_index / 5 - 4);

	return true;
}


void draw_quad_ndc_data(vector<float>& vertex_data, vector<GLuint>& index_data, GLuint tex_handle, long signed int win_width, long signed int win_height)
{
	static GLuint vao = 0, vbo = 0, ibo = 0;

	if (!glIsVertexArray(vao))
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
	}

	// https://raw.githubusercontent.com/progschj/OpenGL-Examples/master/03texture.cpp

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (char*)0 + 3 * sizeof(GLfloat));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index_data.size(), &index_data[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glUseProgram(ortho_shader.get_program());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_handle);

	glUniform1i(uniforms.ortho_shader_uniforms.tex, 0);
	glUniform1i(uniforms.ortho_shader_uniforms.viewport_width, win_width);
	glUniform1i(uniforms.ortho_shader_uniforms.viewport_height, win_height);

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, index_data.size(), GL_UNSIGNED_INT, 0);
}
