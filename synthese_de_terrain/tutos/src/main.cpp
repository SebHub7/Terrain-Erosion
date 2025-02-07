#include "program.h"
#include "app.h"
#include <orbiter.h>
#include <uniforms.h>
#include <tutos/include/ScalarField.h>
#include <image.h>
#include <tutos/include/HeightField.h>
#include <tutos/include/TerrainBuffers.h>
#include <tutos/include/TreeBuffers.h>


class TP : public App
{
public:
    TP() : App(800, 600) {}

    void cameraControl()
    {
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);

        if (mb & SDL_BUTTON(1))
            camera.rotation(mx, my);
        else if (mb & SDL_BUTTON(3))
            camera.translation((float)mx / (float)window_width(), (float)my / (float)window_height());
        else if (mb & SDL_BUTTON(2))
            camera.move(mx);

        SDL_MouseWheelEvent wheel = wheel_event();
        if (wheel.y != 0)
        {
            clear_wheel_event();
            camera.move(8.f * wheel.y);
        }
    }

    int init()
    {
        srand(time(NULL));
        Image everest = read_image("tutos/data/everest.png");

        hf = new HeightField(vec2(0, 0), vec2(100, 100), 300, 300);
        //hf = new HeightField(everest);
        hf->clamp(-9);
        hf->Export();
        hf->SaveScalarFieldAsImage("tutos/data/terrain.png");
        write_image(hf->Shade(), "tutos/data/shade.png");
        hf->SaveGradientsImage();

        ScalarField gradient_sf(*hf, gradientNorm);
        ScalarField laplacian_sf(*hf, laplacian);
        gradient_sf.SaveScalarFieldAsImage("tutos/data/norme_gradient.png");
        laplacian_sf.SaveScalarFieldAsImage("tutos/data/laplacian.png");

        ScalarField sf_stream_areas = hf->StreamArea();
        ScalarField sf_wetness_index = hf->WetnessIndex(sf_stream_areas);
        sf_stream_areas.SaveScalarFieldAsImage("tutos/data/stream_areas.png");
        sf_wetness_index.SaveScalarFieldAsImage("tutos/data/wetness_index.png");

        //hf->GenerateShortestPathImage(0, 0, 299, 299);
        hf->PlaceVegetation(treeb);

        //Image img = read_image("tutos/data/shade.png");
        //Image img = read_image("tutos/data/stream_areas.png");
        //Image img = read_image("tutos/data/wetness_index.png");
        //Image img = read_image("tutos/data/gradients.png");
        //Image img = read_image("tutos/data/laplacian.png");
        //Image img = read_image("tutos/data/norme_gradient.png");
        Image img = read_image("tutos/data/vegetation.png");
        //Image img = read_image("tutos/data/road_img.png");
        tb.AddVertexAndNormals(*hf);
        tb.CreateBuffers(img);
        treeb.CreateBuffers();
 
        Point min, max;
        hf->GetFieldBounds(min, max);
        camera.lookat(min, max);

        return 0;
    }

    int quit()
    {
        delete hf;
        tb.FreeBuffers();
        treeb.FreeBuffers();
        return 0;
    }

    int render()
    {
        if (GetEPressed())
        {
            nb_iter++;
            printf("%i\n", nb_iter);
            hf->StreamPowerErosion();

            if (nb_iter % 8 == 0)
                hf->Smooth(1);

            //hf->CompleteBreach();
            tb.UpdateBuffers(*hf);
            write_image(hf->Shade(), "tutos/data/shade.png");

            /*ScalarField gradient_sf(*hf, gradientNorm);
            gradient_sf.SaveScalarFieldAsImage("tutos/data/norme_gradient.png");

            ScalarField sf_stream_areas = hf->StreamArea();
            ScalarField sf_wetness_index = hf->WetnessIndex(sf_stream_areas);
            sf_stream_areas.SaveScalarFieldAsImage("tutos/data/stream_areas.png");
            sf_wetness_index.SaveScalarFieldAsImage("tutos/data/wetness_index.png");

            hf->PlaceVegetation(treeb);
            treeb.FreeBuffers();
            treeb.CreateBuffers();*/

            ScalarField laplacian_sf(*hf, laplacian);
            laplacian_sf.SaveScalarFieldAsImage("tutos/data/laplacian.png");
        }

        cameraControl();
        tb.draw(camera);
        treeb.draw(camera);

        return 1;
    }

protected:

    HeightField* hf;
    Orbiter camera;
    TerrainBuffers tb;
    TreeBuffers treeb;

    unsigned int nb_iter = 0;
};


int main(int argc, char** argv)
{
    TP tp;
    tp.run();

    return 0;
}