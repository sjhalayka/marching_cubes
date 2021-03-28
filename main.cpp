#include "main.h"

int main(void)
{
	float grid_max = 1.5;
	float grid_min = -grid_max;
	size_t res = 100;

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
	vector<float> xyplane0(res*res, 0);
	vector<float> xyplane1(res*res, 0);

	const float step_size = (grid_max - grid_min) / (res - 1);

	size_t z = 0;

	quaternion Z(grid_min, grid_min, grid_min, z_w);

	// Calculate 0th xy plane.
	for (size_t x = 0; x < res; x++, Z.x += step_size)
	{
		Z.y = grid_min;

		for (size_t y = 0; y < res; y++, Z.y += step_size)
		{
			if (true == make_border && (x == 0 || y == 0 || z == 0 || x == res - 1 || y == res - 1 || z == res - 1))
				xyplane0[x*res + y] = border_value;
			else
				xyplane0[x*res + y] = eqparser.iterate(Z, max_iterations, threshold);
		}
	}

	// Prepare for 1st xy plane.
	z++;
	Z.z += step_size;



	size_t box_count = 0;


	// Calculate 1st and subsequent xy planes.
	for (; z < res; z++, Z.z += step_size)
	{
		Z.x = grid_min;

		cout << "Calculating triangles from xy-plane pair " << z << " of " << res - 1 << endl;

		for (size_t x = 0; x < res; x++, Z.x += step_size)
		{
			Z.y = grid_min;

			for (size_t y = 0; y < res; y++, Z.y += step_size)
			{
				if (true == make_border && (x == 0 || y == 0 || z == 0 || x == res - 1 || y == res - 1 || z == res - 1))
					xyplane1[x*res + y] = border_value;
				else
					xyplane1[x*res + y] = eqparser.iterate(Z, max_iterations, threshold);
			}
		}

		// Calculate triangles for the xy-planes corresponding to z - 1 and z by marching cubes.
		tesselate_adjacent_xy_plane_pair(
			box_count,
			xyplane0, xyplane1,
			z - 1,
			triangles,
			threshold, // Use threshold as isovalue.
			grid_min, grid_max, res,
			grid_min, grid_max, res,
			grid_min, grid_max, res);

		// Swap memory pointers (fast) instead of performing a memory copy (slow).
		xyplane1.swap(xyplane0);
	}

	cout << endl;

	if (0 < triangles.size())
		write_triangles_to_binary_stereo_lithography_file(triangles, "out.stl");

	// Print box-counting dimension
	// Make sure that step_size != 1.0f :)
	cout << logf(static_cast<float>(box_count)) / logf(1.0f/step_size) << endl;


	return 0;
}
