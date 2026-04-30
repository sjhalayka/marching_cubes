#include "main.h"

int main(void)
{
	const float grid_max = 1.5;
	const float grid_min = -grid_max;
	const size_t res = 300;

	const bool make_border = true;

	const float z_w = 0;
	quaternion C;
	C.x = 0.3f;
	C.y = 0.5f;
	C.z = 0.4f;
	C.w = 0.2f;
	const unsigned short int max_iterations = 8;
	const float threshold = 4;

	string error_string;
	quaternion_julia_set_equation_parser eqparser;
	if (false == eqparser.setup("Z = sin(Z) + C*sin(Z)", error_string, C))
	{
		cout << "Equation error: " << error_string << endl;
		return -1;
	}

	vector<pair<double, double>> thresholds;
	thresholds.push_back(pair<double, double>(0, threshold*0.25 - 0.1));
	thresholds.push_back(pair<double, double>(threshold * 0.25, threshold*0.5 - 0.1));
	thresholds.push_back(pair<double, double>(threshold * 0.5, threshold));

	for (size_t i = 0; i < thresholds.size(); i++)
	{
		const double min_threshold = thresholds[i].first;
		const double max_threshold = thresholds[i].second;

		const double mid_threshold = (max_threshold + min_threshold) * 0.5;

		// When adding a border, use a value that is greater than the threshold.
		const float border_value = 1.0f + threshold;

		vector<triangle> triangles;
		vector<float> xyplane0(res * res, 0);
		vector<float> xyplane1(res * res, 0);

		const float step_size = (grid_max - grid_min) / (res - 1);

		size_t z = 0;

		quaternion Z(grid_min, grid_min, grid_min, z_w);

		// Calculate xy plane 0.
		for (size_t x = 0; x < res; x++, Z.x += step_size)
		{
			Z.y = grid_min;

			for (size_t y = 0; y < res; y++, Z.y += step_size)
			{
				if (true == make_border && (x == 0 || y == 0 || z == 0 || x == res - 1 || y == res - 1 || z == res - 1))
					xyplane0[x * res + y] = border_value;
				else
				{
					if (z < res / 2)
					{
						xyplane0[x * res + y] = eqparser.iterate(Z, max_iterations, threshold);
						xyplane0[x * res + y] = fabsf(xyplane0[x * res + y] - mid_threshold);
					}
					else
						xyplane0[x * res + y] = border_value;
				}
			}
		}

		// Prepare for xy plane 1.
		z++;
		Z.z += step_size;

		size_t box_count = 0;

		// Calculate xy planes 1 and greater.
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
						xyplane1[x * res + y] = border_value;
					else
					{
						if (z < res / 2)
						{
							xyplane1[x * res + y] = eqparser.iterate(Z, max_iterations, threshold);
							xyplane1[x * res + y] = fabsf(xyplane1[x * res + y] - mid_threshold);
						}
						else
							xyplane1[x * res + y] = border_value;
					}
				}
			}

			// Calculate triangles for the xy-planes corresponding to z - 1 and z by marching cubes.
			tesselate_adjacent_xy_plane_pair(
				box_count,
				xyplane0, xyplane1,
				z - 1,
				triangles,
				max_threshold - mid_threshold, // Use threshold as isovalue.
				grid_min, grid_max, res,
				grid_min, grid_max, res,
				grid_min, grid_max, res);

			// Swap memory pointers (fast) instead of performing a memory copy (slow).
			xyplane1.swap(xyplane0);
		}

		cout << endl;

		if (0 < triangles.size())
		{
			ostringstream oss;
			oss << "out" << i << ".stl";
			write_triangles_to_binary_stereo_lithography_file(triangles, oss.str().c_str());
		}

		// Print box-counting dimension
		// Make sure that step_size != 1.0f :)
		cout << "Box counting dimension: " << logf(static_cast<float>(box_count)) / logf(1.0f / step_size) << endl;
	}

	return 0;
}
