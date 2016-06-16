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
        this->marcada  = false;
        this->imagen = imagen->copia();
        this->region = { x, y, CASILLA_ANCHO, CASILLA_ALTO };
        this->textura = SDL_CreateTextureFromSurface( renderer, imagen->dameImagenSurface() );
        SDL_RenderCopy( renderer, textura, nullptr, &region );
    }
    ~Casilla() {
        SDL_DestroyTexture( textura );
        delete imagen;
    }
    bool estaMarcada(){
        return marcada;
    }
    char idCarta(){
        return imagen->dameID();
    }
    bool validaClic(int xp, int yp){
        int x2 = x + CASILLA_ANCHO;
        int y2 = y + CASILLA_ALTO;
        return (x<= xp && xp < x2) && (y<= yp && yp < y2);
    }
    void marcar( SDL_Renderer * renderer ) {
        imagen->escalaGris();
        SDL_DestroyTexture( textura );
        textura = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
                                     imagen->dameImagenSurface()->w, imagen->dameImagenSurface()->h );
        SDL_UpdateTexture( textura, nullptr, imagen->dameImagenSurface()->pixels,
                           imagen->dameImagenSurface()->w * sizeof( Uint32 ) );
        SDL_RenderCopy( renderer, textura, nullptr, &region );
        marcada = true;
    }
};

#endif // CASILLA_H_INCLUDED
