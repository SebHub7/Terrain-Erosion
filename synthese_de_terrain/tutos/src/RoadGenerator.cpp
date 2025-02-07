#include <tutos/include/RoadGenerator.h>

RoadGenerator::RoadGenerator()
{

}


float RoadGenerator::Cost(HeightField& hf, int x, int y, int x2, int y2) const
{
	float is_in_water = hf.GetH()[y2][x2] == -9 ? 1 : 0;
	return hf.Slope(x2, y2) + is_in_water;
}


std::vector<std::pair<int, int>> RoadGenerator::Dijkstra(HeightField& hf, int sx, int sy, int dx, int dy)
{
	int nx = hf.getNx();
	int ny = hf.getNy();
	Grid dist(vec2(), vec2(), nx, ny);
	Grid pred(vec2(), vec2(), nx, ny);
	Grid vertex_in_p(vec2(), vec2(), nx, ny);
	int nb_vertex = 0;
	const int DIRECTIONS[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };

	// initialisation
	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
		{
			dist(x, y, FLT_MAX);
			pred(x, y, -1);
			vertex_in_p(x, y, 0);
		}

	dist(sx, sy, 0);

	// calcul des distances minimales depuis la source
	while (nb_vertex < nx * ny)
	{
		float min_dist = FLT_MAX;
		int vx = 0, vy = 0;

		for (int y = 0; y < ny; y++)
			for (int x = 0; x < nx; x++)
			{ 
				if (vertex_in_p(x, y) == 0 && dist(x, y) < min_dist)
				{
					min_dist = dist(x, y);
					vx = x;
					vy = y;
				}
			}
		vertex_in_p(vx, vy, 1);
		nb_vertex++;


		for (int i = 0; i < 8; i++)
		{
			int newx = vx + DIRECTIONS[i][0];
			int newy = vy + DIRECTIONS[i][1];

			if (newx >= 0 && newx < nx && newy >= 0 && newy < ny
				&& vertex_in_p(newx, newy) == 0)
			{
				float alt = dist(vx, vy) + Cost(hf, vx, vy, newx, newy);
				if (dist(newx, newy) > alt)
				{
					dist(newx, newy, alt);
					pred(newx, newy, vy * nx + vx);
				}
			}
		}
	}

	// récupération du chemin
	int cx = dx;
	int cy = dy;
	std::vector<std::pair<int, int>> shortest_path;

	while (cx >= 0 && cy >= 0)
	{
		shortest_path.push_back({ cx, cy });
		cx = (int)pred(cx, cy) % nx;
		cy = (int)(pred(cx, cy) / nx);
	}

	return shortest_path;
}