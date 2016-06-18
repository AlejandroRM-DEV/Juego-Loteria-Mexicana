#ifndef CASILLA_H_INCLUDED
#define CASILLA_H_INCLUDED

#define CASILLA_ALTO 160
#define CASILLA_ANCHO 104

#include "Imagen.h"

class Casilla {
private:
    int x;
    int y;
    int x2;
    int y2;
    bool marcada;
    Imagen* imagen;
    SDL_Rect region;
    SDL_Texture * textura;
    SDL_Renderer * renderer;

public:
    Casilla( int x, int y, SDL_Renderer * renderer ) {
        this->x = x;
        this->y = y;
        this->x2 = x + CASILLA_ANCHO;
        this->y2 = y + CASILLA_ALTO;
        this->region = { x, y, CASILLA_ANCHO, CASILLA_ALTO };
        this->renderer = renderer;
        this->imagen = nullptr;
    }

    ~Casilla() {
        SDL_DestroyTexture( textura );
        delete imagen;
    }
    void reiniciar( Imagen* imagen ) {
        if( this->imagen != nullptr ) {
            delete imagen;
            SDL_DestroyTexture( textura );
        }

        this->marcada  = false;
        this->imagen = imagen->copia();
        this->textura = SDL_CreateTextureFromSurface( renderer, imagen->imagenSurface() );
        SDL_RenderCopy( renderer, textura, nullptr, &region );
    }
    bool estaMarcada() {
        return marcada;
    }
    char idCarta() {
        return imagen->dameID();
    }
    bool validaClic( int xp, int yp ) {
        return ( x <= xp && xp < x2 ) && ( y <= yp && yp < y2 );
    }
    void marcar( ) {
        imagen->escalaGris();
        SDL_DestroyTexture( textura );
        textura = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
                                     imagen->imagenSurface()->w, imagen->imagenSurface()->h );
        SDL_UpdateTexture( textura, nullptr, imagen->imagenSurface()->pixels,
                           imagen->imagenSurface()->w * sizeof( Uint32 ) );
        SDL_RenderCopy( renderer, textura, nullptr, &region );
        marcada = true;
    }
};

#endif // CASILLA_H_INCLUDED
