#include <iostream>
#include <vector>
#include <sstream>

#include "Boton.h"
#include "Tablero.h"

#define VENTANA_ALTO 680
#define VENTANA_ANCHO 800

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
    do {
        tablero->reiniciar( pruebaSeleccionados() );

        texturaCartaLanzada = SDL_CreateTextureFromSurface( renderer,
                                            cartas[0]->dameImagenSurface() );
        SDL_RenderCopy( renderer, texturaCartaLanzada, nullptr, &rectCartaLanzada );
        tablero->agregarCartaLanzada( 1 );
        tablero->agregarCartaLanzada( 2 );
        tablero->agregarCartaLanzada( 3 );

        SDL_RenderPresent( renderer );
        quit = false;
        while ( !quit ) {
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
            /**
            *    Verificar si ya se lanzo una nueva carta
            */
            SDL_RenderPresent( renderer );
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

int* pruebaSeleccionados() {
    int *seleccionados = new int[16] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    return seleccionados;
}
