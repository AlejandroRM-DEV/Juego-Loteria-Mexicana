#ifndef IMAGEN_H_INCLUDED
#define IMAGEN_H_INCLUDED

#include <iostream>

#include <SDL.h>
#include <SDL_image.h>

using namespace std;

class Imagen {
    char id;
    string ruta;
    SDL_Surface * img;
public:
    Imagen( int id, string ruta ) {
        this->id = id;
        this->ruta = ruta;
        img = IMG_Load( ruta.c_str() );
    }
    ~Imagen() {
        SDL_FreeSurface( img );
    }
    Imagen* copia() {
        return new Imagen( id, ruta );
    }
    void escalaGris() {
        SDL_Surface * colorImg = img;
        img = SDL_ConvertSurfaceFormat( img, SDL_PIXELFORMAT_ARGB8888, 0 );
        SDL_FreeSurface( colorImg );

        Uint32 * pixels = ( Uint32 * )img->pixels;
        for ( int y = 0; y < img->h; y++ ) {
            for ( int x = 0; x < img->w; x++ ) {
                Uint32 pixel = pixels[y * img->w + x];
                Uint8 r = pixel >> 16 & 0xFF, g = pixel >> 8 & 0xFF, b = pixel & 0xFF;
                Uint8 v = 0.212671f * r + 0.715160f * g + 0.072169f * b;
                pixel = ( 0xFF << 24 ) | ( v << 16 ) | ( v << 8 ) | v;
                pixels[y * img->w + x] = pixel;
            }
        }
    }
    SDL_Surface* imagenSurface() {
        return img;
    }
    char dameID() {
        return id;
    }
};

#endif // IMAGEN_H_INCLUDED
