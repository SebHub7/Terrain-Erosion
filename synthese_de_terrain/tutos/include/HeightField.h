#pragma once
#include "ScalarField.h"
#include <image.h>
#include "TreeBuffers.h"


enum HeightFieldType { average_slope, stream_area };


class HeightField : public ScalarField
{
public:

	HeightField(const vec2 a, const vec2 b, const int nx = 100, const int ny = 100);

	HeightField(Image& img);

	/* Renvoie la position du sommet 3D au point (x, y)*/
	vec3 Vertex(const int x, const int y) const;

	/* Renvoie la normale du sommet 3D au point (x, y)*/
	vec3 Normal(const int x, const int y);

	/* Crée une image en prenant en compte l'éclairement des sommets */
	Image Shade();

	/* Renvoie une image avec le type défini (slope, average slope) */
	Image CreatePropertieImage(HeightFieldType hft);

	/* Exporte la carte de hauteur au format .obj */
	void Export();

	/* Renvoi la hauteur du point par rapport au point le plus bas (min_h) */
	float Height(const int x, const int y) const;

	/* Renvoie la pente au point (i,j) */
	float Slope(const int x, const int y);

	/* Renvoie la pente moyenne au point (i,j) */
	float AverageSlope(const int x, const int y);

	/* fontion de comparaison pour trier sommets */
	static bool comp(std::pair<float, std::pair<int, int>> a, std::pair<float, std::pair<int, int>> b);

	/* Renvoie une grid avec les aires de drainage */
	ScalarField StreamArea();

	/* Calcul le stream power erosion sur tous les sommets */
	void HeightField::StreamPowerErosion();

	/* Calcul le wetnesIndex */
	ScalarField WetnessIndex(ScalarField& s_stream_area);

	/* Applique le breaching au terrain */
	void CompleteBreach();

	void PlaceVegetation(TreeBuffers& treeb);

	void GenerateShortestPathImage(int sx, int sy, int dx, int dy);

private:

	float min_h, max_h;
};