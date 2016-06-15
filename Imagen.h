#ifndef IMAGEN_H_INCLUDED
#define IMAGEN_H_INCLUDED

#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

using namespace std;

class Imagen {
    int id;
    SDL_Surface * img;
public:
    Imagen( int id, string ruta ) {
        this->id = id;
        img = IMG_Load( ruta.c_str() );
    }
    ~Imagen() {
        SDL_FreeSurface( img );
    }
    SDL_Surface* dameImagen() {
        return img;
    }
    int dameID() {
        return id;
    }
};

#endif // IMAGEN_H_INCLUDED
