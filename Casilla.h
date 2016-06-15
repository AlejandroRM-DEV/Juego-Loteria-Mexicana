#ifndef CASILLA_H_INCLUDED
#define CASILLA_H_INCLUDED

#define CASILLA_ALTO 160
#define CASILLA_ANCHO 104

#include "Imagen.h"

class Casilla {
private:
    int x;
    int y;
    SDL_Texture * textura;
    SDL_Rect region;
    Imagen* imagen;
    bool marcada;

public:
    Casilla( int x, int y, Imagen* imagen, SDL_Renderer * renderer ) {
        this->x = x;
        this->y = y;
        this->imagen = imagen;
        region = { x, y, CASILLA_ANCHO, CASILLA_ALTO };
        textura = SDL_CreateTextureFromSurface( renderer, imagen->dameImagen() );
        SDL_RenderCopy( renderer, textura, nullptr, &region );
        this->marcada  = false;
    }
    ~Casilla() {
        SDL_DestroyTexture( textura );
    }


};

#endif // CASILLA_H_INCLUDED
