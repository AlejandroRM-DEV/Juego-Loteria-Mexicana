#ifndef BOTON_H_INCLUDED
#define BOTON_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include "Imagen.h"

class Boton {
    Imagen* img;
    SDL_Texture* textura;
    int x;
    int y;
    int x2;
    int y2;
    int ancho;
    int alto;
public:
    Boton( int x, int y, int ancho, int alto, const char* ruta, SDL_Renderer * renderer ) {
        this->x = x;
        this->y = y;
        x2 = x + ancho;
        y2 = y + alto;
        SDL_Rect r = { x, y, ancho, alto};
        SDL_RenderFillRect( renderer, &r );
        img = new Imagen( -1, ruta );
        textura = SDL_CreateTextureFromSurface( renderer, img->imagenSurface() );
        SDL_RenderCopy( renderer, textura, nullptr, &r  );
    }
    ~Boton() {
        SDL_DestroyTexture( textura );
        delete img;
    }

    bool validaClic( int xp, int yp ) {
        return ( x <= xp && xp < x2 ) && ( y <= yp && yp < y2 );
    }
};

#endif // BOTON_H_INCLUDED
