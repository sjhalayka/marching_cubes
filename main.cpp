#include "main.h"

int main(void)
{
	float x_grid_max = 1.5;
	float y_grid_max = 1.5;
	float z_grid_max = 1.5;
	float x_grid_min = -x_grid_max;
	float y_grid_min = -y_grid_max;
	float z_grid_min = -z_grid_max;
	size_t x_res = 200;
	size_t y_res = 200;
	size_t z_res = 200;
	bool make_border = true;

	float z_w = 0;
	quaternion C;
	C.x = 0.3f;
	C.y = 0.5f;
	C.z = 0.4f;
	C.w = 0.2f;
	unsigned short int max_iterations = 8;
	float threshold = 4;

	string error_string;
	quaternion_julia_set_equation_parser eqparser;
	if (false == eqparser.setup("Z = sin(Z) + C * sin(Z)", error_string, C))
	{
		cout << "Equation error: " << error_string << endl;
		return -1;
	}

	// When adding a border, use a value that is "much" greater than the threshold.
	const float border_value = 1.0f + threshold;

	vector<triangle> triangles;
	vector<float> xyplane0(x_res*y_res, 0);
	vector<float> xyplane1(x_res*y_res, 0);

	const float x_step_size = (x_grid_max - x_grid_min) / (x_res - 1);
	const float y_step_size = (y_grid_max - y_grid_min) / (y_res - 1);
	const float z_step_size = (z_grid_max - z_grid_min) / (z_res - 1);

	size_t z = 0;

	quaternion Z(x_grid_min, y_grid_min, z_grid_min, z_w);

	// Calculate 0th xy plane.
	for (size_t x = 0; x < x_res; x++, Z.x += x_step_size)
	{
		Z.y = y_grid_min;

		for (size_t y = 0; y < y_res; y++, Z.y += y_step_size)
		{
			if (true == make_border && (x == 0 || y == 0 || z == 0 || x == x_res - 1 || y == y_res - 1 || z == z_res - 1))
				xyplane0[x*y_res + y] = border_value;
			else
				xyplane0[x*y_res + y] = eqparser.iterate(Z, max_iterations, threshold);
		}
	}

	// Prepare for 1st xy plane.
	z++;
	Z.z += z_step_size;

	// Calculate 1st and subsequent xy planes.
	for (; z < z_res; z++, Z.z += z_step_size)
	{
		Z.x = x_grid_min;

		cout << "Calculating triangles from xy-plane pair " << z << " of " << z_res - 1 << endl;

		for (size_t x = 0; x < x_res; x++, Z.x += x_step_size)
		{
			Z.y = y_grid_min;

			for (size_t y = 0; y < y_res; y++, Z.y += y_step_size)
			{
				if (true == make_border && (x == 0 || y == 0 || z == 0 || x == x_res - 1 || y == y_res - 1 || z == z_res - 1))
					xyplane1[x*y_res + y] = border_value;
				else
					xyplane1[x*y_res + y] = eqparser.iterate(Z, max_iterations, threshold);
			}
		}

		// Calculate triangles for the xy-planes corresponding to z - 1 and z by marching cubes.
		tesselate_adjacent_xy_plane_pair(
			xyplane0, xyplane1,
			z - 1,
			triangles,
			threshold, // Use threshold as isovalue.
			x_grid_min, x_grid_max, x_res,
			y_grid_min, y_grid_max, y_res,
			z_grid_min, z_grid_max, z_res);

		// Swap memory pointers (fast) instead of performing a memory copy (slow).
		xyplane1.swap(xyplane0);
	}

	cout << endl;

	if (0 < triangles.size())
		write_triangles_to_binary_stereo_lithography_file(triangles, "out.stl");

	return 0;
}