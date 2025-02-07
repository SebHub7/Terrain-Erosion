#include <tutos/include/ScalarField.h>
#include <tutos/include/FastNoise.h>

void ScalarField::GetFieldBounds(Point& min, Point& max)
{
    float min_h, max_h;
    FindMinAndMaxValue(min_h, max_h);
    min = Point(a.x, a.y, min_h);
    max = Point(b.x, b.y, max_h);
}

void ScalarField::FindMinAndMaxValue(float& min_h, float& max_h)
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

void ScalarField::SaveScalarFieldAsImage(std::string file_name)
{
    Image img(nx, ny);

    NormalizeImage(img);
    write_image(img, file_name.c_str());
}

void ScalarField::NormalizeImage(Image& img)
{
    float min_h, max_h;
    FindMinAndMaxValue(min_h, max_h);

    for (size_t y = 0; y < ny; y++)
        for (size_t x = 0; x < nx; x++)
            img.setPixel(y, x, Color((h[y][x] + abs(min_h)) / (max_h + abs(min_h))));
}

void ScalarField::SaveGradientsImage()
{
    Image img(nx, ny);

    for (size_t y = 0; y < ny; y++)
        for (size_t x = 0; x < nx; x++)
        {
            vec2 g = Gradient(x, y);
            img.setPixel(y, x, Color(abs(g.x), 0, abs(g.y)));
        }

    write_image(img, "tutos/data/gradients.png");
}


void ScalarField::clamp(float threshold)
{
    for (size_t y = 0; y < ny; y++)
        for (size_t x = 0; x < nx; x++)
            if (h[y][x] < threshold)
                h[y][x] = threshold;
}

void ScalarField::Smooth(float iteration)
{
    float directions[8][2] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };
    Grid new_grid(a, b, nx, ny);
    float** h_grid = new_grid.GetH();

    for (int k = 0; k < iteration; k++)
    {
        // convolution
        for (size_t y = 0; y < ny; y++)
            for (size_t x = 0; x < nx; x++)
            {
                float res = 0;
                int nb_neighbors = 0;

                for (int i = 0; i < 8; i++)
                {
                    int new_x = x + directions[i][0];
                    int new_y = y + directions[i][1];

                    if (new_x >= 0 && new_x < nx && new_y >= 0 && new_y < ny)
                    {
                        res += h[new_y][new_x];
                        nb_neighbors++;
                    }
                }

                h_grid[y][x] = res / nb_neighbors;
            }

        // copie dans le champs calaire
        for (size_t y = 0; y < ny; y++)
            for (size_t x = 0; x < nx; x++)
                h[y][x] = h_grid[y][x];
    }
}

void ScalarField::ScaleField(float factor)
{
    for (size_t y = 0; y < ny; y++)
        for (size_t x = 0; x < nx; x++)
            h[y][x] *= factor;
}

ScalarField::ScalarField(const vec2 a, const vec2 b, const int nx, const int nz) : Grid(a, b, nx, nz)
{
    FastNoise fn;
    //fn.SetSeed(rand()%10000);
    //.SetSeed(1986);
    fn.SetSeed(200);

    /*h[y][x] = fn.GetNoise(x, y) * 15
        + fn.GetNoise(x * 2, y * 2) * 3;*/
    for (int y = 0; y < ny; y++)
        for (int x = 0; x < nx; x++)
            h[y][x] = fn.GetNoise(x / 2.0f, y / 2.0f) * 20
            + fn.GetNoise(x * 2, y * 2) * 3;
}

ScalarField::ScalarField(ScalarField& sf, const gridType gt) : Grid(sf.a, sf.b, sf.nx, sf.ny)
{
    for (int y = 0; y < sf.ny; y++)
        for (int x = 0; x < sf.nx; x++)

            if (gt == gradientNorm)
                h[y][x] = sf.Gradient(x, y).length();
            else if (gt == laplacian)
                h[y][x] = sf.Laplacian(x, y);
}

ScalarField::ScalarField(const Grid& g) : Grid(g) {}

vec2 ScalarField::Gradient(const int x, const int y)
{
    vec2 grad(h[y][x], h[y][x]);
    vec2 grad2(h[y][x], h[y][x]);

    int nb_x = 1;
    int nb_y = 1;
    
    if (x - 1 >= 0)
        grad.x = h[y][x - 1];

    if (x + 1 < nx)
        grad2.x = h[y][x + 1];

    if (y - 1 >= 0)
        grad.y = h[y - 1][x];

    if (y + 1 < nx)
        grad2.y = h[y + 1][x];

    if (x != 0 && x != nx - 1)
        nb_x++;

    if (y != 0 && y != ny - 1)
        nb_y++;

    grad.x = (grad.x - grad2.x) / nb_x;
    grad.y = (grad.y - grad2.y) / nb_y;

    return grad;
}

float ScalarField::Laplacian(const int x, const int y)
{
    float laplacian = 0;
    int nb_samples = 0;

    float height = h[y][x];
    int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    for (int i = 0; i < 4; i++)

        if (x + directions[i][0] >= 0 && y + directions[i][1] >= 0
            && x + directions[i][0] < nx && y + directions[i][1] < ny)
        {
            laplacian += h[y + directions[i][1]][x + directions[i][0]];
            nb_samples++;
        }

    return laplacian / nb_samples - h[y][x];
}
