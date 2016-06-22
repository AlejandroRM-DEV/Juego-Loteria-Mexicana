#ifndef BOTON_H_INCLUDED
#define BOTON_H_INCLUDED

#include <SDL.h>
#include "SDL_ttf.h"

class Boton {
    SDL_Rect r;
    SDL_Surface* textoRender;
    SDL_Texture* textura;
    int x;
    int y;
    int x2;
    int y2;
    int ancho;
    int alto;
public:
    Boton( int x, int y, int ancho, int alto, const char* texto, SDL_Renderer * renderer ) {
        this->x = x;
        this->y = y;
        x2 = x + ancho;
        y2 = y + alto;
        r = { x, y, ancho, alto};
        SDL_RenderFillRect( renderer, &r );
        TTF_Font* font = TTF_OpenFont( "arial.ttf", 48 );
        textoRender = TTF_RenderText_Solid( font, texto, SDL_Color {0, 0, 0, 255} );
        TTF_CloseFont( font );
        textura = SDL_CreateTextureFromSurface( renderer, textoRender );
        SDL_RenderCopy( renderer, textura, nullptr, &r );
    }
    ~Boton() {
        SDL_DestroyTexture( textura );
        SDL_FreeSurface( textoRender );
    }

    bool validaClic( int xp, int yp ) {
        return ( x <= xp && xp < x2 ) && ( y <= yp && yp < y2 );
    }
};

#endif // BOTON_H_INCLUDED
