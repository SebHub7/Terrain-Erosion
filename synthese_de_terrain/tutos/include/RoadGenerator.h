#pragma once
#include <image.h>
#include "HeightField.h"

class RoadGenerator
{
public:
	RoadGenerator();

	float Cost(HeightField& hf, int x, int y, int x2, int y2) const;
	std::vector<std::pair<int, int>> Dijkstra(HeightField& hf, int sx, int sy, int dx, int dy);

private:


};