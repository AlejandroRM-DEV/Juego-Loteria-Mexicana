#include <iostream>
#include <vector>
#include <sstream>

#include "Tablero.h"

#include "SDL_ttf.h"

#define VENTANA_ALTO 680
#define VENTANA_ANCHO 800

using namespace std;

vector<Imagen*> generarCartas( );
char* pruebaSeleccionados();

int main( int argc, char **argv ) {
    vector<Imagen*> cartas;
    Tablero* tablero;
    SDL_Event event;
    bool quit;

    SDL_Init( SDL_INIT_VIDEO );
    SDL_Window * window = SDL_CreateWindow( "Loteria Mexicana", SDL_WINDOWPOS_UNDEFINED,
                                            SDL_WINDOWPOS_UNDEFINED, VENTANA_ANCHO, VENTANA_ALTO, 0 );
    SDL_Renderer * renderer = SDL_CreateRenderer( window, -1, 0 );
    IMG_Init( IMG_INIT_PNG );
    SDL_Surface * icon = IMG_Load( "img/icono.PNG" );
    SDL_SetWindowIcon( window, icon );
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE  );
    SDL_RenderClear( renderer );

    IMG_Init( IMG_INIT_JPG );
    cartas = generarCartas();
    tablero = new Tablero( cartas, pruebaSeleccionados(), renderer );
    SDL_RenderPresent( renderer );
    quit = false;

    SDL_Texture * texturaCartaLanzada;
    SDL_Rect regionCartaLanzada = { 515, 70, 228, 350 };

    texturaCartaLanzada = SDL_CreateTextureFromSurface( renderer, cartas[rand() % 54]->dameImagen() );
    SDL_RenderCopy( renderer, texturaCartaLanzada, nullptr, &regionCartaLanzada );
    SDL_RenderPresent( renderer );

    TTF_Init();

    TTF_Font* arial = TTF_OpenFont( "arial.ttf", 50 );
    SDL_Color negro = {0, 0, 0, 0};
    SDL_Surface* regionTituloLanzamiento = TTF_RenderText_Solid( arial, "Lanzada",negro );
    SDL_Texture* texturaTituloLanzamiento = SDL_CreateTextureFromSurface( renderer, regionTituloLanzamiento );
    SDL_Rect marcoTituloLanzamiento= {528,20,200,40};
    SDL_RenderCopy( renderer, texturaTituloLanzamiento, nullptr, &marcoTituloLanzamiento );

    SDL_Rect r= { 528, 550, 200, 50};
    SDL_SetRenderDrawColor( renderer, 0, 255, 255, 255 );
    SDL_RenderFillRect( renderer, &r );
    SDL_Surface* q1 = TTF_RenderText_Solid( arial, "  Loteria  ",negro );
    SDL_Texture* q2 = SDL_CreateTextureFromSurface( renderer, q1 );
    SDL_RenderCopy( renderer, q2, nullptr, &r );

    SDL_RenderPresent( renderer );

    while ( !quit ) {
        SDL_WaitEvent( &event );

        switch ( event.type ) {
        case SDL_QUIT:
            quit = true;
            break;
        }
    }
    TTF_CloseFont(arial);
    SDL_DestroyTexture( texturaCartaLanzada );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );

    TTF_Quit();
    delete tablero;
    IMG_Quit();
    SDL_Quit();

    return 0;
}

vector<Imagen*> generarCartas( ) {
    vector<Imagen*> cartas( 54 );
    for( int i = 0; i < 54; i++ ) {
        stringstream ss;
        ss << "img/cartas/" << ( i + 1 )  << ".JPG";
        cartas[i] = new Imagen( ( i + 1 ), ss.str() );
    }
    return cartas;
}

char* pruebaSeleccionados() {
    char *seleccionados = new char[16] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    return seleccionados;
}
