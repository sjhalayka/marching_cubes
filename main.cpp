#include "main.h"


float x_grid_max = 1.5;
float y_grid_max = 1.5;
float z_grid_max = 1.5;
float x_grid_min = -x_grid_max;
float y_grid_min = -y_grid_max;
float z_grid_min = -z_grid_max;
size_t x_res = 10;
size_t y_res = 10;
size_t z_res = 10;
float threshold = 1;
bool make_border = true;


int main(void)
{
	// When adding a border, use a value that is "much" greater than the threshold.
	const float border_value = 1.0f + threshold;

	vector<triangle> triangles;
	vector<float> xyplane0(x_res*y_res, 0);
	vector<float> xyplane1(x_res*y_res, 0);

	size_t z = 0;

	cout << "Calculating xy-plane " << z + 1 << " of " << z_res << endl;

	// Calculate 0th xy plane.
	for (size_t x = 0; x < x_res; x++)
	{
		for (size_t y = 0; y < y_res; y++)
		{
			if (true == make_border && (x == 0 || y == 0 || z == 0 || x == x_res - 1 || y == y_res - 1 || z == z_res - 1))
				xyplane0[x*y_res + y] = border_value;
			else
				xyplane0[x*y_res + y] = static_cast<float>(rand() % 2);
		}
	}

	// Prepare for 1st xy plane.
	z++;

	// Calculate 1st and subsequent xy planes.
	for (; z < z_res; z++)
	{
		cout << "Calculating xy-plane " << z + 1 << " of " << z_res << endl;

		for (size_t x = 0; x < x_res; x++)
		{
			for (size_t y = 0; y < y_res; y++)
			{
				if (true == make_border && (x == 0 || y == 0 || z == 0 || x == x_res - 1 || y == y_res - 1 || z == z_res - 1))
					xyplane1[x*y_res + y] = border_value;
				else
					xyplane1[x*y_res + y] = static_cast<float>(rand() % 2);
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