
#ifndef _APP_H
#define _APP_H

#include "window.h"


//! \addtogroup application utilitaires pour creer une application

//! \file
/*! squelette d'application: creation d'une fenetre, d'un contexte openGL et gestion des evenements.
    tuto7.cpp et tuto8.cpp presentent un exemple simple d'utilisation.

    la class App expose les fonctionnalites de window.h, elles sont juste presentees differemment.
    les fonctions globales de window.h sont toujours utilisables (a part run() qui est remplace par App::run()).
*/

//! classe application.
class App
{
public:
    //! constructeur, dimensions de la fenetre et version d'openGL.
    App( const int width, const int height, const int major= 3, const int minor= 3, const int samples= 0 );
    virtual ~App( );

    //! a deriver pour creer les objets openGL. renvoie -1 pour indiquer une erreur, 0 sinon.
    virtual int init( ) = 0;
    //! a deriver pour detruire les objets openGL. renvoie -1 pour indiquer une erreur, 0 sinon.
    virtual int quit( ) = 0;

    //! a deriver et redefinir pour animer les objets en fonction du temps. 
    virtual int update( const float time, const float delta ) { return 0; }
    //! a deriver pour afficher les objets. renvoie 1 pour continuer, 0 pour fermer l'application.
    virtual int render( ) = 0;

    //! execution de l'application.
    int run( );
    
protected:
    virtual int prerender( ) { return update(global_time(), delta_time()); }
    virtual int postrender( ) { return 0; }

    void vsync_off( );
    
    bool added_point = false;
    int mousex, mousey;
    Window m_window;
    Context m_context;
    
    bool sync;
};


#endif // _APP_H
