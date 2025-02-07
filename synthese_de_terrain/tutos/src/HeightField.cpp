#include <fstream>
#include <tutos/include/HeightField.h>
#include <map>
#include <unordered_map>
#include <tutos/include/RoadGenerator.h>


HeightField::HeightField(const vec2 a, const vec2 b, const int nx, const int ny) : ScalarField(a, b, nx, ny)
{
	min_h = FLT_MAX;
	max_h = FLT_MIN;

	for (size_t y = 0; y < ny; y++)
		for (size_t x = 0; x < nx; x++)
		{
			if (h[y][x] < min_h)
				min_h = h[y][x];

			if (h[y][x] > max_h)
				max_h = h[y][x];
		}
}

HeightField::HeightField(Image& img) : ScalarField(vec2(0, 0), vec2(img.width(), img.height()), img.width(), img.height())
{
	min_h = FLT_MAX;
	max_h = FLT_MIN;

	for (size_t y = 0; y < ny; y++)
		for (size_t x = 0; x < nx; x++)
			h[y][x] = img(x, y).r * 500;
}


vec3 HeightField::Vertex(const int x, const int y) const
{
	return vec3(x * d.x, h[y][x], y * d.y);
}

vec3 HeightField::Normal(const int x, const int y)
{
	vec2 g = Gradient(x, y);
	return vec3(-g.x, 1, -g.y);
}

Image HeightField::Shade()
{
	Image img(nx, ny);
	vec3 light_direction(0.5, 0.2, 0); 
	light_direction = normalize(light_direction);

	for (size_t y = 0; y < ny; y++)
		for (size_t x = 0; x < nx; x++)
		{
			vec3 normal = Normal(x, y);
			float cos_theta = dot(light_direction, normal);

			if (cos_theta > 0)
				img.setPixel(y, x, Color(cos_theta));
			else
				img.setPixel(y, x, Color(0));
		}

	return img;
}

Image HeightField::CreatePropertieImage(HeightFieldType hft)
{
	Image img(ny, nx);

	for (size_t y = 0; y < ny; y++)
		for (size_t x = 0; x < nx; x++)

			if (hft == stream_area)
				img.setPixel(y, x, Color(Slope(x, y)));
			else if (hft == average_slope)
				img.setPixel(y, x, Color(AverageSlope(x, y)));

	return img;
}


void HeightField::Export()
{
	std::ofstream file;
	file.open("tutos/data/heightfield.obj");

	// writing vertices
	for (size_t y = 0; y < ny; y++)
		for (size_t x = 0; x < nx; x++)
		{ 
			vec3 v = Vertex(x, y);
			file << "v " << v.x << " " << v.y << " " << v.z << "\n";
		}

	// writing triangles
	for (size_t y = 0; y < ny - 1; y++)
		for (size_t x = 1; x < nx; x++)
		{
			int index_vertex = y * nx + x;
			int index_vertex2 = (y+1) * nx + x;
			file << "f " << index_vertex  << " " << index_vertex2 << " " << index_vertex + 1 << "\n";
			file << "f " << index_vertex + 1 << " " << index_vertex2<< " " << index_vertex2 + 1 << "\n";
		}
	file.close();

	printf("Export done\n");
}

float HeightField::Height(const int x, const int y) const
{
	return h[y][x] - min_h;
}

float HeightField::Slope(const int x, const int y)
{
	return Gradient(x, y).length();
}

float HeightField::AverageSlope(const int x, const int y)
{
	float avg_slope = 0;
	int nb_samples = 0;

	if (x - 1 >= 0) {
		avg_slope += Slope(x - 1, y);
		nb_samples++;
	}
	if (x + 1  < nx) {
		avg_slope += Slope(x + 1, y);
		nb_samples++;
	}
	if (y - 1 >= 0) {
		avg_slope += Slope(x, y - 1);
		nb_samples++;
	}
	if (y + 1 < ny) {
		avg_slope += Slope(x, y + 1);
		nb_samples++;
	}

	return avg_slope / nb_samples;
}

bool HeightField::comp(std::pair<float, std::pair<int, int>> a, std::pair<float, std::pair<int, int>> b)
{
	return a.first > b.first;
}

ScalarField HeightField::StreamArea()
{
	// pair( height, (x, y) ), trie des sommets par hauteur
	std::vector<std::pair<float, std::pair<int, int>>> drain_areas;
	
	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
			drain_areas.push_back( std::pair<float, std::pair<int, int>>(Height(x, y), std::pair<int, int>(x, y)) );

	std::sort(drain_areas.begin(), drain_areas.end(), HeightField::comp);


	// init des aires de drainage à 1
	Grid grid_drain_areas(a, b, nx, ny);

	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
			grid_drain_areas.GetH()[y][x] = 1;


	// calcul des aires de drainage

	for (auto& v : drain_areas)
	{
		int x = v.second.first;
		int y = v.second.second;

		vec2 grad = Gradient(x, y);

		if (length(grad) > 0)
		{
			grad = normalize(grad);
			grad.x = x + std::round(grad.x);
			grad.y = y + std::round(grad.y);

			if (grad.x >= 0 && grad.x < nx && grad.y >= 0 && grad.y < ny)
				grid_drain_areas.GetH()[(int)grad.y][(int)grad.x] += grid_drain_areas.GetH()[y][x];
		}
	}

	// baisser les hautes fréquences
	/*for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
			if (grid_drain_areas.GetH()[y][x] != 1.0f)
				grid_drain_areas.GetH()[y][x] = log(grid_drain_areas.GetH()[y][x])*/;

	return ScalarField(grid_drain_areas);
}

void HeightField::StreamPowerErosion()
{
	ScalarField drain_areas = StreamArea();
	float** drain = drain_areas.GetH();
	Grid new_heights(a, b, nx, ny);
	float delta_t = 0.01;

	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
			new_heights.GetH()[y][x] = delta_t * pow(drain[y][x], 0.5f) * Slope(x, y);

	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
			h[y][x] -= new_heights.GetH()[y][x];
}

ScalarField HeightField::WetnessIndex(ScalarField& s_stream_aera)
{
	Grid wetness(a, b, nx, ny);

	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
		{
			float s = Slope(x, y);
			if (s != 0)
				wetness.GetH()[x][y] = log(s_stream_aera.GetH()[y][x]) / (s + 1);
			else
				wetness.GetH()[x][y] = log(s_stream_aera.GetH()[y][x]) / 0.001;
		}

	return ScalarField(wetness);
}


void HeightField::PlaceVegetation(TreeBuffers& treeb)
{
	treeb.translations.clear();
	Image vegetation_img(nx, ny);
	Image stream_area_img = read_image("tutos/data/stream_areas.png");
	Image wetness_img = read_image("tutos/data/wetness_index.png");
	Image road_img = read_image("tutos/data/road_img.png");

	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++)
		{
			Color c;
			float sa = stream_area_img(y, x).r;
			float w = wetness_img(x, y).r;

			if (h[y][x] < 10)
				if (Slope(x, y) < 0.4)
					c = Color(0.25, 0.59, 0) * (1 - w) + Color(0, 0.42, 0) * w;
				else
					c = Color(0.91, 0.81, 0.65) * (1 - w) + Color(0.25, 0.16, 0) * w;
			else
				c = Color(1);

			if (sa > 0.05)
				c = Color(0, 0, 1) * sa;

			if (h[y][x] == -9)
				c = Color(0, 0, 1);

			/*if (road_img(y, x).g == 0)
				c = Color(1, 0, 0);*/

			vegetation_img.setPixel(y, x, c);

			if (Slope(x, y) < 0.4
				&& h[y][x] < 9 // n'est pas sur la neige
				&& h[y][x] > -8.5 // n'est pas sur l'eau
				&& rand() % (1 + (int)(40.0f * (1.0f - w))) == 0)
				treeb.translations.push_back(vec3(x * d.x, h[y][x], y * d.y));
		}
	write_image(vegetation_img, "tutos/data/vegetation.png");
}


void HeightField::GenerateShortestPathImage(int sx, int sy, int dx, int dy)
{
	Image road_img(nx, ny, Color(1));

	RoadGenerator rg;
	std::vector<std::pair<int, int>> shortest_path = rg.Dijkstra(*this, sx, sy, dx, dy);
	const int DIRECTIONS[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

	for (int i = 0; i < shortest_path.size(); i++)
	{
		int x = shortest_path[i].first;
		int y = shortest_path[i].second;

		road_img.setPixel(y, x, Color(1, 0, 0));

		for (int k = 0; k < 4; k++)
		{
			int newx = x + DIRECTIONS[k][0];
			int newy = y + DIRECTIONS[k][1];

			if (newx >= 0 && newx < nx && newy >= 0 && newy < ny)
				road_img.setPixel(newy, newx, Color(1, 0, 0));
		}
	}

	write_image(road_img, "tutos/data/road_img.png");
}