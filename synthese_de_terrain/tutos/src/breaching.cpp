#pragma once
#include <queue>
#include <tutos/include/Grid.h>
#include <tutos/include/HeightField.h>


const int DIRECTIONS = 8;
const int dx[DIRECTIONS] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int dy[DIRECTIONS] = { 0, -1, -1, -1, 0, 1, 1, 1 };
float flat = 0.0001;


struct QPoint
{
    QPoint(int x, int y) : value{ x,y } {};
    inline int x()const { return value[0]; }
    inline int y()const { return value[1]; }

private:
    int value[2];
};


using Element = std::pair<double, QPoint>;

struct comp
{

    // Compare elevation, then x, then y
    bool operator()(const Element& a, const Element& b)
    {
        const QPoint& lhs = a.second;
        const QPoint& rhs = b.second;

        if (a.first > b.first) return true;
        else if ((a.first == b.first) && (lhs.x() > rhs.x() || (lhs.x() == rhs.x() && lhs.y() > rhs.y())))
            return true;
        else return false;
    }
};

using ZPriorityQueue = std::priority_queue< Element, std::vector<Element>, comp >;

enum LindsayMode
{
    COMPLETE_BREACHING,
    SELECTIVE_BREACHING,
    CONSTRAINED_BREACHING
};

enum LindsayCellType
{
    UNVISITED,
    VISITED,
    EDGE
};


/*!
\brief Breach depressions.

Depression breaching drills a path from a depression's pit cell (its lowest
point) along the least-cost (Priority-Flood) path to the nearest cell
outside the depression to have the same or lower elevation.

See https://rbarnes.org/

See https://github.com/r-barnes/richdem

See https://github.com/r-barnes/richdem/blob/master/include/richdem/depressions/Lindsay2016.hpp

\author John Lindsay, implementation by Richard Barnes (rbarnes@umn.edu).
*/
void HeightField::CompleteBreach()
{
    int mode = COMPLETE_BREACHING;
    bool fill_depressions = false;

    // Heightfield properties

    const int NO_BACK_LINK = -1;

    Grid backlinks(a, b, nx, ny);
    Grid visited(a, b, nx, ny);
    Grid pits(a, b, nx, ny);
    std::vector<int> flood_array;
    ZPriorityQueue pq;

    int total_pits = 0;

    // Seed the priority queue
    for (int x = 0; x < nx; x++)
    {
        for (int y = 0; y < ny; y++)
        {
            // Valid edge cells go on priority-queue
            if (x == 0 || x == (nx - 1) || y == 0 || y == (ny - 1))
            {
                pq.emplace(h[y][x], QPoint(x, y));
                visited.GetH()[y][x] = LindsayCellType::EDGE;
                continue;
            }

            // Determine if this is an edge cell, gather information used to determine if it is a pit cell
            double lowest_neighbour = std::numeric_limits<double>::max();
            for (int n = 0; n < DIRECTIONS; n++)
            {
                const int px = x + dx[n];
                const int py = y + dy[n];

                // No need for an inGrid check here because edge cells are filtered above
                // Used for identifying the lowest neighbour
                lowest_neighbour = std::min(h[py][px], (float)lowest_neighbour);
            }

            // This is a pit cell if it is lower than any of its neighbours. In this
            // case: raise the cell to be just lower than its lowest neighbour. This
            // makes the breaching/tunneling procedures work better.
            if (h[y][x] < lowest_neighbour)
            {
                h[y][x] = lowest_neighbour - 2.0 * flat;
            }
            // Since depressions might have flat bottoms, we treat flats as pits. Mark 
            // flat/pits as such now.
            if (h[y][x] <= lowest_neighbour)
            {
                pits.GetH()[y][x] = true;
                total_pits++;
            }
        }
    }

    // The Priority-Flood operation assures that we reach pit cells by passing into
    // depressions over the outlet of minimal elevation on their edge.
    while (!pq.empty())
    {
        const Element c = pq.top();
        pq.pop();

        const QPoint p = c.second;

        //T Cell is a pit, consider doing some breaching: locate a cell that is lower than the pit cell, or an edge cell
        if (pits.GetH()[p.y()][p.x()])
        {
            int cc = p.y() * nx + p.x();                                  //Current cell on the path
            double target_height = h[p.y()][p.x()];                 //Depth to which the cell currently being considered should be carved

            if (mode == COMPLETE_BREACHING)
            {
                //Trace path back to a cell low enough for the path to drain into it, or to an edge of the DEM
                while (cc != NO_BACK_LINK && h[p.y()][p.x()] >= target_height)
                {
                    h[p.y()][p.x()] = target_height;
                    cc = backlinks.GetH()[p.y()][p.x()]; // Follow path back
                    target_height = target_height - 2.0 * flat; //Decrease target depth slightly for each cell on path to ensure drainage
                }
            }
            else
            {

            }

            --total_pits;
            if (total_pits == 0)
                break;
        }

        //Looks for neighbours which are either unvisited or pits
        for (int n = 0; n < DIRECTIONS; n++)
        {
            const int px = p.x() + dx[n];
            const int py = p.y() + dy[n];

            if (px >= 0 && px < nx && py >= 0 && py < ny)
                continue;
            if (visited.GetH()[py][px] != LindsayCellType::UNVISITED)
                continue;

            const double my_e = h[py][px];

            //The neighbour is unvisited. Add it to the queue
            pq.emplace(my_e, QPoint(px, py));

            if (fill_depressions)
                flood_array.emplace_back(py * nx + px);

            visited.GetH()[py][px] = LindsayCellType::VISITED;
            backlinks.GetH()[py][px] = p.y() * nx + p.x();
        }

        if (mode != COMPLETE_BREACHING && fill_depressions)
        {
            for (const auto f : flood_array)
            {
                int parent = backlinks.at(f);
                if (at(f) <= at(parent))
                {
                    int x = f % nx;
                    int y = f / nx;
                    h[y][x] = nextafter(at(parent), std::numeric_limits<double>::max());
                }
            }
        }
    }
}