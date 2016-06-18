#include <iostream>
#include <vector>
#include <sstream>

#include "Boton.h"
#include "Tablero.h"

#define VENTANA_ALTO 680
#define VENTANA_ANCHO 800

#include "ServerPruebaInterfaz.h"

using namespace std;

vector<Imagen*> generarCartas( );
int* pruebaSeleccionados();

int main( int argc, char **argv ) {
    vector<Imagen*> cartas;
    Tablero* tablero;
    Boton* btnLoteria;
    SDL_Event event;
    bool quit;

    SDL_Init( SDL_INIT_VIDEO );
    SDL_Window * window = SDL_CreateWindow( "Loteria Mexicana", SDL_WINDOWPOS_UNDEFINED,
                                            SDL_WINDOWPOS_UNDEFINED, VENTANA_ANCHO, VENTANA_ALTO, 0 );
    SDL_Renderer * renderer = SDL_CreateRenderer( window, -1, 0 );
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE  );
    SDL_RenderClear( renderer );

    IMG_Init( IMG_INIT_PNG );
    SDL_Surface * icon = IMG_Load( "img/icono.PNG" );
    SDL_SetWindowIcon( window, icon );

    TTF_Init();
    TTF_Font* font = TTF_OpenFont( "arial.ttf", 50 );

    SDL_Surface* regionTituloLanzamiento = TTF_RenderText_Solid( font, "Lanzada", SDL_Color {0, 0, 0, 0}  );
    SDL_Texture* texturaTituloLanzamiento = SDL_CreateTextureFromSurface( renderer,
                                            regionTituloLanzamiento );
    SDL_Rect rectTituloLanzamiento = {528, 20, 200, 40};

    IMG_Init( IMG_INIT_JPG );
    cartas = generarCartas();
    tablero = new Tablero( cartas, renderer );
    btnLoteria = new Boton( 528, 550, 200, 50, "  Loteria  ", font, renderer );

    /**
    *    Pedir y validar datos del servidor y  hacer la conexion
    */

    btnLoteria->render();
    SDL_RenderCopy( renderer, texturaTituloLanzamiento, nullptr, &rectTituloLanzamiento );
    SDL_Rect rectCartaLanzada = { 515, 70, 228, 350 };
    SDL_Texture* texturaCartaLanzada;
    /**
    *  iniciamos un nuevo juego, notificamos estar listos y recibimos ids de cartas
    */
    MiniServidor serv;  // Borrar despues
    do {
        tablero->reiniciar( serv.pruebaSeleccionados() );
        serv.iniciarReloj();
        quit = false;
        while ( !quit ) {
            SDL_RenderPresent( renderer );
            while ( SDL_PollEvent( &event ) ) {
                switch ( event.type ) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if( tablero->verficaClic( event.motion.x, event.motion.y ) ) {
                        cout << "Enviando al servidor info" << endl;
                    } else if( btnLoteria->validaClic( event.motion.x, event.motion.y ) ) {
                        cout << "clic en loteria" << endl;
                    }
                    break;
                }
            }
            if( serv.hayQueLeer() ) {
                int id = serv.lanzar();
                texturaCartaLanzada = SDL_CreateTextureFromSurface( renderer, cartas[id - 1]->imagenSurface() );
                tablero->agregarCartaLanzada( id );
                SDL_RenderCopy( renderer, texturaCartaLanzada, nullptr, &rectCartaLanzada );
            }
        }
    } while ( !quit );

    TTF_CloseFont( font );
    TTF_Quit();
    SDL_DestroyTexture( texturaCartaLanzada );
    delete btnLoteria;
    delete tablero;
    IMG_Quit();
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
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

