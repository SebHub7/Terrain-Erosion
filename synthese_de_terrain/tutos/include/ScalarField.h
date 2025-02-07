#pragma once
#include "Grid.h"
#include <image_io.h>


typedef int index;

class ScalarField : public Grid
{
public:

    ScalarField(const vec2 a, const vec2 b, const int nx = 100, const int ny = 100);

    ScalarField(ScalarField& sf, const gridType gt);

    ScalarField(const Grid& g);

    /* Nombre de points sur l'axe x */
    int getNx() { return nx; }

    /* Nombre depoints sur l'axe z */
    int getNy() { return ny; }

    void GetFieldBounds(Point& min, Point& max);

    /* Trouve la valeur minimum et maximum du champ scalaire */
    void FindMinAndMaxValue(float& min_h, float& max_h);

    /* Sauvegarder le champ scalaire en une image */
    void SaveScalarFieldAsImage(std::string file_name);

    /* Normalize le champ scalaire entre 0 et 1 pour l'image */
    void NormalizeImage(Image& img);

    /* Sauvegarde une image des gradients */
    void SaveGradientsImage();

    /* Seuille le champ scalaire, les valeurs en dessous de threshold seront mises à 0 */
    void clamp(float threshold);

    void Smooth(float iteration);

    void ScaleField(float factor);

    vec2 Gradient(const int x, const int y);

    float Laplacian(const int x, const int y);

};