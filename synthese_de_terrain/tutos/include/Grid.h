#pragma once
#include <vec.h>
#include <vector>

enum gridType { gradientNorm, laplacian };

class Grid
{
protected:
    vec2 a, b;
    vec2 d; // diagonale d'une cellule

    int nx, ny; // nombre de sommets en x et en z
    float** h;

public:
    Grid(vec2 a1, vec2 b1, int nx = 100, int ny = 100) : a(a1), b(b1), nx(nx), ny(ny) 
    {
        d = vec2( abs(a.x - b.x) / nx, abs(a.y - b.y) / ny );
        h = new float* [ny];

        for (int y = 0; y < ny; y++)
            h[y] = new float[nx];
    }

    Grid(const Grid& g) : a(g.a), b(g.b), nx(g.nx), ny(g.ny)
    {
        h = new float* [ny];

        for (int y = 0; y < ny; y++)
            h[y] = new float[nx];

        for (int y = 0; y < ny; y++)
            for (int x = 0; x < nx; x++)
                h[y][x] = g.h[y][x];
    }
    
    ~Grid()
    {
        for (int y = 0; y < ny; y++)
            delete h[y];

        delete h;
    }

    float at(int v_index)
    {
        int x = v_index % nx;
        int y = v_index / nx;
        return h[y][x];
    }

    void operator() (int x, int y, float v)
    {
        h[y][x] = v;
    }

    float operator() (int x, int y)
    {
        return h[y][x];
    }

    float**& GetH() { return h; }

    virtual int getNx() { return nx; }
    virtual int getNy() { return ny; }
    virtual vec2 getD() { return d; }
};