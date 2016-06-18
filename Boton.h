#ifndef BOTON_H_INCLUDED
#define BOTON_H_INCLUDED

#include <SDL.h>
#include "SDL_ttf.h"

class Boton {
    SDL_Rect r;
    SDL_Surface* textoRender;
    SDL_Texture* textura;
    SDL_Renderer * renderer;
    int x;
    int y;
    int ancho;
    int alto;
public:
    Boton( int x, int y, int ancho, int alto, const char* texto, TTF_Font* font, SDL_Renderer * renderer ) {
        this->x = x;
        this->y = y;
        this->ancho = ancho;
        this->alto = alto;
        this->renderer = renderer;
        r = { x, y, ancho, alto};
        SDL_SetRenderDrawColor( renderer, 0, 255, 255, 255 );
        SDL_RenderFillRect( renderer, &r );
        textoRender = TTF_RenderText_Solid( font, texto, SDL_Color {0,0,0,0} );
        textura = SDL_CreateTextureFromSurface( renderer, textoRender );
    }
    ~Boton() {
        SDL_DestroyTexture( textura );
        SDL_FreeSurface( textoRender );
    }
    void render(){
        SDL_RenderCopy( renderer, textura, nullptr, &r );
    }
    bool validaClic( int xp, int yp ) {
        int x2 = x + ancho;
        int y2 = y + alto;
        return ( x <= xp && xp < x2 ) && ( y <= yp && yp < y2 );
    }
};

#endif // BOTON_H_INCLUDED
