#include <iostream>
#include <vector>
#include <chrono>

#include "Boton.h"
#include "Tablero.h"
#include "Credencial.h"

#define VENTANA_ALTO 680
#define VENTANA_ANCHO 800

#include "ServerPruebaInterfaz.h"

using namespace std;

vector<Imagen*> generarCartas( );
SDL_Texture* renderTexto( const string &texto, SDL_Color color, int tamano,
                          SDL_Renderer *renderer );
void renderTexturaEnRect( SDL_Texture *texto, SDL_Renderer *renderer, int x, int y, int ancho,
                          int alto );
Credencial* pantallaInicio( SDL_Renderer *renderer );
void pantallaJuego( SDL_Renderer *renderer, Credencial* credencial );

int main( int argc, char **argv ) {
    SDL_Init( SDL_INIT_VIDEO );
    IMG_Init( IMG_INIT_PNG );
    IMG_Init( IMG_INIT_JPG );
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow( "Loteria Mexicana", SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED, VENTANA_ANCHO, VENTANA_ALTO, 0 );
    SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, 0 );
    SDL_Surface *icon = IMG_Load( "img/icono.PNG" );
    SDL_SetWindowIcon( window, icon );

    Credencial* credencial = pantallaInicio( renderer );
    if( credencial != nullptr ) {
        pantallaJuego( renderer, credencial );
        delete credencial;
    }

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    TTF_Quit();
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

void renderTexturaEnRect( SDL_Texture *texto, SDL_Renderer *renderer, int x, int y, int ancho,
                          int alto ) {
    SDL_Rect dst = {x, y, ancho, alto};
    SDL_RenderCopy( renderer, texto, nullptr, &dst );
}

SDL_Texture* renderTexto( const string &texto, SDL_Color color, int tamano,
                          SDL_Renderer *renderer ) {
    TTF_Font *font = TTF_OpenFont( "arial.ttf", tamano );
    SDL_Surface *surf = TTF_RenderText_Blended( font, texto.c_str(), color );
    SDL_Texture *texture = SDL_CreateTextureFromSurface( renderer, surf );
    SDL_FreeSurface( surf );
    TTF_CloseFont( font );
    return texture;
}

Credencial* pantallaInicio( SDL_Renderer *renderer ) {
    SDL_Texture *texturaServidor, *texturaTextoIngresado, *textoJugador, *texturaFondo1, *texturaFondo2,
                *texturaFondo3, *texturaTitulo, *texturaError;
    SDL_Event event;
    std::chrono::steady_clock::time_point tInicio, tFin;
    string msjError, textoIngresado;
    bool error, terminado, conectado;
    Credencial* credencial = nullptr;
    int servidorfd, x;

    texturaServidor = renderTexto( "Dirección del servidor", SDL_Color { 0, 0, 0, 255 }, 48,
                                   renderer );
    textoJugador = renderTexto( "  Nombre del Jugador  ", SDL_Color { 0, 0, 0, 255 }, 48, renderer );
    Imagen* imgFondo1 = new Imagen( -1, "img/presentacion.JPG" );
    Imagen* imgFondo2 = new Imagen( -1, "img/todas1.PNG" );
    Imagen* imgFondo3 = new Imagen( -1, "img/todas2.PNG" );
    Imagen* imgTitulo = new Imagen( -1, "img/titulo.PNG" );
    texturaFondo1 = SDL_CreateTextureFromSurface( renderer, imgFondo1->imagenSurface() );
    texturaFondo2 = SDL_CreateTextureFromSurface( renderer, imgFondo2->imagenSurface() );
    texturaFondo3 = SDL_CreateTextureFromSurface( renderer, imgFondo3->imagenSurface() );
    texturaTitulo = SDL_CreateTextureFromSurface( renderer, imgTitulo->imagenSurface() );
    SDL_Rect hint = { 0, 300, 800, 80 };

    error = terminado = conectado = false;
    textoIngresado = "127.0.0.1";
    x = 0;
    tInicio = std::chrono::steady_clock::now();
    while ( !terminado ) {
        SDL_StartTextInput();
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
            case SDL_QUIT:
                servidorfd = -1;
                terminado = true;
            case SDL_KEYDOWN:
                if( event.key.keysym.sym == SDLK_BACKSPACE ) {
                    if( !textoIngresado.empty() ) {
                        textoIngresado.pop_back();
                    }
                } else if( event.key.keysym.sym == SDLK_RETURN ) {
                    if( !conectado ) {
                        /**
                        *   Validar todos los errores al crear y establecer conexiones
                        */
                        if( textoIngresado == "ERROR" ) {
                            error = true;
                            msjError = "Aqui va el mensaje de error, si ocurre alguno";
                        } else {
                            conectado = true;
                            textoIngresado = "Jugador X";
                            servidorfd = 1;
                        }
                    } else {
                        /**
                        *   El servidor validara que el nombre de usuario este disponible,
                        *   de ser asi establecer terminado = true; y retornar
                        */
                        terminado = true;
                        credencial = new Credencial( servidorfd, textoIngresado );
                    }
                }
                break;
            case SDL_TEXTINPUT:
                textoIngresado.append( event.text.text );
                break;
            case SDL_TEXTEDITING:
                textoIngresado.append( event.edit.text );
                break;
            }
            texturaTextoIngresado = renderTexto( textoIngresado.c_str(), SDL_Color { 0, 0, 0, 255 }, 48,
                                                 renderer );
        }

        tFin = std::chrono::steady_clock::now();
        if( std::chrono::duration_cast<std::chrono::microseconds>( tFin - tInicio ).count() >= 20000 ) {
            tInicio = std::chrono::steady_clock::now();
            x = ( ( ++x ) < 2760 ) ? x : 0;
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
            SDL_RenderClear( renderer );
            renderTexturaEnRect( texturaFondo1, renderer, x, 73, 800, 535 );
            renderTexturaEnRect( texturaFondo2, renderer, x - 980, 90, 980, 500 );
            renderTexturaEnRect( texturaFondo3, renderer, x - 1960, 90, 980, 500 );
            renderTexturaEnRect( texturaFondo1, renderer, x - 2760, 73, 800, 535 );
            renderTexturaEnRect( texturaTitulo, renderer, 0, 10, 800, 96 );
            SDL_RenderFillRect( renderer, &hint );
            if( !conectado ) {
                renderTexturaEnRect( texturaServidor, renderer, 268, 300, 264, 40 );
            } else {
                renderTexturaEnRect( textoJugador, renderer, 268, 300, 264, 40 );
            }
            renderTexturaEnRect( texturaTextoIngresado, renderer, ( 400 - textoIngresado.size() * 7 ), 340,
                                 textoIngresado.size() * 14, 40 );
            if( error ) {
                texturaError = renderTexto( msjError.c_str(), SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                renderTexturaEnRect( texturaError, renderer, 0, 650, msjError.size() * 10, 24 );
            }
            SDL_RenderPresent( renderer );
        }
        SDL_StopTextInput();
    }

    SDL_DestroyTexture( texturaTextoIngresado );
    SDL_DestroyTexture( texturaServidor );
    SDL_DestroyTexture( textoJugador );
    SDL_DestroyTexture( texturaFondo1 );
    SDL_DestroyTexture( texturaFondo2 );
    SDL_DestroyTexture( texturaFondo3 );
    SDL_DestroyTexture( texturaTitulo );
    delete imgFondo1;
    delete imgFondo2;
    delete imgFondo3;
    delete imgTitulo;
    return credencial;
}

void pantallaJuego( SDL_Renderer *renderer, Credencial* credencial ) {
    SDL_Texture *textura, *texturaCartaLanzada;
    SDL_Event event;
    Boton* btnLoteria;
    Tablero* tablero;
    vector<Imagen*> cartas;
    bool terminado;

    /****/
    cout << credencial->dameServidorfd() << " " << credencial->dameJugador() << endl;
    /****/
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE  );
    SDL_RenderClear( renderer );

    cartas = generarCartas();
    tablero = new Tablero( renderer );

    SDL_SetRenderDrawColor( renderer, 224, 224, 224, SDL_ALPHA_OPAQUE );
    btnLoteria = new Boton( 528, 550, 200, 50, "img/botonLoteria.PNG", renderer );

    textura = renderTexto( "Lanzada", SDL_Color { 0, 0, 0, 255 }, 48, renderer );
    renderTexturaEnRect( textura, renderer, 528, 20, 200, 40 );

    SDL_Rect rectCartaLanzada = { 515, 70, 228, 350 };
    SDL_SetRenderDrawColor( renderer, 224, 224, 224, SDL_ALPHA_OPAQUE );

    MiniServidor serv;  // Borrar despues
    do {
        SDL_RenderFillRect( renderer, &rectCartaLanzada );
        tablero->reiniciar( serv.pruebaSeleccionados() );
        serv.iniciarReloj();
        terminado = false;
        while ( !terminado ) {
            SDL_RenderPresent( renderer );
            while ( SDL_PollEvent( &event ) ) {
                switch ( event.type ) {
                case SDL_QUIT:
                    terminado = true;
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
    } while ( !terminado );

    SDL_DestroyTexture( textura );
    SDL_DestroyTexture( texturaCartaLanzada );
    delete btnLoteria;
    delete tablero;
    for( Imagen* img : cartas ) {
        delete img;
    }
}
