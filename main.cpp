#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip> //Para formatear string
#include <cstring>
#include <cstdint> //int16_t

#include "Boton.h"
#include "Tablero.h"
#include "Credencial.h"

#define VENTANA_ALTO 680
#define VENTANA_ANCHO 800

struct Jugadores {
    char nombre1[10];
    int16_t ganados1;
    char nombre2[10];
    int16_t ganados2;
    char nombre3[10];
    int16_t ganados3;
    char nombre4[10];
    int16_t ganados4;
};

#include "ServerPruebaInterfaz.h"

static Imagen imgFondo1( -1, "img/presentacion.JPG" );
static Imagen imgFondo2( -1, "img/todas1.PNG" );
static Imagen imgFondo3( -1, "img/todas2.PNG" );
static Imagen imgtxtTitulo( -1, "img/txt/titulo.PNG" );
static Imagen imgtxtLoteria( -1, "img/txt/loteria.PNG" );
static Imagen imgtxtEsperando( -1, "img/txt/esperando_jugadores.PNG" );
static Imagen imgtxtNombre( -1, "img/txt/nombre_jugador.PNG" );
static Imagen imgtxtLanzada( -1, "img/txt/lanzada.PNG" );
static Imagen imgtxtServidor( -1, "img/txt/servidor.PNG" );

using namespace std;

string formatoJugador( string nombre, int ganados );
vector<Imagen*> generarCartas( );
vector<SDL_Texture*> fondos( SDL_Renderer *renderer );
SDL_Color selColorMarcador( const char* otro, const char* local );
SDL_Texture* renderTexto( const string &texto, SDL_Color color, int tamano,
                          SDL_Renderer *renderer );
Credencial* pantallaInicio( SDL_Renderer *renderer );
void crearPuntajes( char* datosJugadores, string jugadorLocal, SDL_Renderer *renderer );
void pantallaJuego( SDL_Renderer *renderer, Credencial* credencial );
void fondoPantalla( vector<SDL_Texture*>& texturas, SDL_Renderer *renderer );
bool esperarJugadores( SDL_Renderer *renderer, Credencial* credencial );
void renderTexturaEnRect( SDL_Texture *texto, SDL_Renderer *renderer, int x, int y, int ancho,
                          int alto );

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

void fondoPantalla( vector<SDL_Texture*>& texturas, SDL_Renderer *renderer ) {
    static int x = 0;
    x = ( ( ++x ) < 2760 ) ? x : 0;
    renderTexturaEnRect( texturas[0], renderer, x, 73, 800, 535 );
    renderTexturaEnRect( texturas[1], renderer, x - 980, 90, 980, 500 );
    renderTexturaEnRect( texturas[2], renderer, x - 1960, 90, 980, 500 );
    renderTexturaEnRect( texturas[0], renderer, x - 2760, 73, 800, 535 );
    renderTexturaEnRect( texturas[3], renderer, 0, 10, 800, 96 );
}

vector<SDL_Texture*> fondos( SDL_Renderer *renderer ) {
    vector<SDL_Texture*> texturas( 4 );
    texturas[0] = SDL_CreateTextureFromSurface( renderer, imgFondo1.imagenSurface() );
    texturas[1] = SDL_CreateTextureFromSurface( renderer, imgFondo2.imagenSurface() );
    texturas[2] = SDL_CreateTextureFromSurface( renderer, imgFondo3.imagenSurface() );
    texturas[3] = SDL_CreateTextureFromSurface( renderer, imgtxtTitulo.imagenSurface() );
    return texturas;
}

string formatoJugador( string nombre, int ganados ) {
    stringstream ss;
    ss << nombre  << setw( 8 ) << setfill( '_' ) << ganados;
    return ss.str();
}

SDL_Color selColorMarcador( const char* otro, const char* local ) {
    return ( strcmp( otro, local ) == 0 ) ?
           SDL_Color { 0, 0, 255,  SDL_ALPHA_OPAQUE }:
           SDL_Color { 0, 0, 0, SDL_ALPHA_OPAQUE };
}

void crearPuntajes( char* datosJugadores, string jugadorLocal, SDL_Renderer *renderer ) {
    struct Jugadores jugadores;
    vector<SDL_Texture*> texturasJugadores;
    SDL_Color color;

    memcpy( &jugadores, datosJugadores, sizeof( Jugadores ) );
    color = selColorMarcador( jugadores.nombre1, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        renderTexto( formatoJugador( jugadores.nombre1, jugadores.ganados1 ).c_str(), color, 32, renderer )
    );
    color = selColorMarcador( jugadores.nombre2, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        renderTexto( formatoJugador( jugadores.nombre2, jugadores.ganados2 ).c_str(), color, 32, renderer )
    );
    color = selColorMarcador( jugadores.nombre3, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        renderTexto( formatoJugador( jugadores.nombre3, jugadores.ganados3 ).c_str(), color, 32, renderer )
    );
    color = selColorMarcador( jugadores.nombre4, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        renderTexto( formatoJugador( jugadores.nombre4, jugadores.ganados4 ).c_str(), color, 32, renderer )
    );

    renderTexturaEnRect( texturasJugadores.at( 0 ), renderer, 508, 520, 240, 24 );
    renderTexturaEnRect( texturasJugadores.at( 1 ), renderer, 508, 550, 240, 24 );
    renderTexturaEnRect( texturasJugadores.at( 2 ), renderer, 508, 580, 240, 24 );
    renderTexturaEnRect( texturasJugadores.at( 3 ), renderer, 508, 610, 240, 24 );
    for( SDL_Texture* textura : texturasJugadores ) {
        SDL_DestroyTexture( textura );
    }
}


Credencial* pantallaInicio( SDL_Renderer *renderer ) {
    SDL_Texture *texturaServidor, *texturaTextoIngresado, *textoJugador, *texturaError;
    vector<SDL_Texture*> texturasFondos =  fondos( renderer );
    SDL_Event event;
    std::chrono::steady_clock::time_point tInicio;
    string msjError, textoIngresado;
    bool error, terminado, conectado;
    Credencial* credencial = nullptr;
    int servidorfd;

    texturaServidor = SDL_CreateTextureFromSurface( renderer, imgtxtServidor.imagenSurface() );
    textoJugador = SDL_CreateTextureFromSurface( renderer, imgtxtNombre.imagenSurface() );
    SDL_Rect hint = { 0, 300, 800, 100 };
    error = terminado = conectado = false;
    textoIngresado = "127.0.0.1";
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
                        if( textoIngresado.empty() ) {
                            error = true;
                            msjError = "No se ingreso el nombre del host";
                        } else if( textoIngresado == "ERROR" ) {
                            error = true;
                            msjError = "Aqui va el mensaje de error, si ocurre alguno";
                        } else {
                            conectado = true;
                            textoIngresado = "Jugador 00";
                            servidorfd = 1;
                        }
                    } else {
                        /**
                        *   El servidor validara que el nombre de usuario este disponible,
                        *   de ser asi establecer terminado = true; y retornar
                        */
                        if( textoIngresado.empty() ) {
                            error = true;
                            msjError = "Debe elegir un nombre de jugador";
                        } else if( textoIngresado.size() > 10 ) {
                            error = true;
                            msjError = "Maximo 10 caracteres para el nombre de jugador";
                        } else {
                            terminado = true;
                            credencial = new Credencial( servidorfd, textoIngresado );
                        }
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

        if( std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - tInicio ).count() >= 500 ) {
            tInicio = std::chrono::steady_clock::now();
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
            SDL_RenderClear( renderer );
            fondoPantalla( texturasFondos, renderer );
            SDL_RenderFillRect( renderer, &hint );
            if( !conectado ) {
                renderTexturaEnRect( texturaServidor, renderer, 0, 310, 800, 50 );
            } else {
                renderTexturaEnRect( textoJugador, renderer, 0, 310, 800, 50 );
            }
            renderTexturaEnRect( texturaTextoIngresado, renderer, ( 400 - textoIngresado.size() * 7 ), 360,
                                 textoIngresado.size() * 14, 40 );
            if( error ) {
                texturaError = renderTexto( msjError.c_str(), SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                renderTexturaEnRect( texturaError, renderer, 0, 650, msjError.size() * 10, 24 );
            }
            SDL_RenderPresent( renderer );
            SDL_Delay( 10 );
        }
        SDL_StopTextInput();
    }

    SDL_DestroyTexture( texturaTextoIngresado );
    SDL_DestroyTexture( texturaServidor );
    SDL_DestroyTexture( textoJugador );
    for( SDL_Texture* textura : texturasFondos ) {
        SDL_DestroyTexture( textura );
    }
    return credencial;
}

/**
    Pantalla de espera de jugadores antes del inicio de cada partida.
    Cuando se completan 4 jugadores el servidor los da a conocer y se inicia la partida.
        - La pantalla se muestra cuando es juego nuevo.
        - Algun jugador abandono la partida.
**/
bool esperarJugadores( SDL_Renderer *renderer, Credencial* credencial ) {
    char* datosJugadores;
    SDL_Event event;
    SDL_Rect hint = { 0, 300, 800, 100 };
    SDL_Texture* texturaEsperando = SDL_CreateTextureFromSurface( renderer,
                                    imgtxtEsperando.imagenSurface() );
    vector<SDL_Texture*> texturasFondos =  fondos( renderer );
    bool listo = false, retorno = true;
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
    std::chrono::steady_clock::time_point tInicio = std::chrono::steady_clock::now();
    while( !listo ) {
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
            case SDL_QUIT:
                retorno = false;
                listo = true;
                break;
            case SDL_KEYDOWN:
                listo = true;
                break;
            }
        }
        if( std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - tInicio ).count() >= 500 ) {
            SDL_RenderClear( renderer );
            fondoPantalla( texturasFondos, renderer );
            SDL_RenderFillRect( renderer, &hint );
            renderTexturaEnRect( texturaEsperando, renderer, 0, 300, 800, 100 );
            SDL_RenderPresent( renderer );
            SDL_Delay( 10 );
        }
    }
    SDL_RenderClear( renderer );
    MiniServidor serv;  // USAR credencial->dameServidorfd();
    datosJugadores = serv.datosJugadores();
    crearPuntajes( datosJugadores, credencial->dameJugador(), renderer );
    delete datosJugadores;
    for( SDL_Texture* textura : texturasFondos ) {
        SDL_DestroyTexture( textura );
    }
    SDL_DestroyTexture( texturaEsperando );
    return retorno;
}

void pantallaJuego( SDL_Renderer *renderer, Credencial* credencial ) {
    Boton btnLoteria( 528, 440, 200, 50, "img/botonLoteria.PNG", renderer );
    Tablero tablero( renderer );
    bool terminado, salir, habilitaBoton;
    vector<Imagen*> cartas;
    SDL_Texture *texturaTxtLanzada, *texturaCartaLanzada, *texturaGanador, *texturaLoteria;
    SDL_Event event;

    MiniServidor serv;  // Borrar despues

    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE  );
    SDL_RenderClear( renderer );
    cartas = generarCartas();
    texturaLoteria = SDL_CreateTextureFromSurface( renderer, imgtxtLoteria.imagenSurface() );
    SDL_Rect rectCartaLanzada = { 515, 70, 228, 350 };
    texturaTxtLanzada = SDL_CreateTextureFromSurface( renderer, imgtxtLanzada.imagenSurface() );
    salir = false;
    do {
        if( !esperarJugadores( renderer, credencial ) ) {
            break;
        }

        btnLoteria.render( renderer );
        SDL_SetRenderDrawColor( renderer, 224, 224, 224, SDL_ALPHA_OPAQUE );
        renderTexturaEnRect( texturaTxtLanzada, renderer, 528, 20, 200, 40 );
        SDL_RenderFillRect( renderer, &rectCartaLanzada );
        tablero.reiniciar( serv.pruebaSeleccionados() );
        terminado = habilitaBoton = false;

        serv.iniciarReloj();

        do {
            SDL_RenderPresent( renderer );
            while ( SDL_PollEvent( &event ) ) {
                switch ( event.type ) {
                case SDL_QUIT:
                    salir = true;
                    terminado = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if( tablero.verficaClic( event.motion.x, event.motion.y ) ) {
                        if( tablero.restantes() == 0 ) {
                            habilitaBoton = true;
                        }
                    } else if( habilitaBoton && btnLoteria.validaClic( event.motion.x, event.motion.y ) ) {
                        /*
                            Enviar al servidor que ya ganamos
                        */
                        cout << "LOTERIA" << endl;
                        terminado = true;
                    }
                    break;
                }
            }

            if( terminado && !salir ) {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
                SDL_Rect hint = { 0, 0, 800, 680 };
                SDL_RenderFillRect( renderer, &hint );
                renderTexturaEnRect( texturaLoteria, renderer, 0, 200, 800, 180 );
                texturaGanador  = renderTexto( "de AlejandroR", SDL_Color { 0, 0, 0, 255 },
                                               110, renderer );
                renderTexturaEnRect( texturaGanador, renderer, 180, 360, 440, 110 );
                SDL_DestroyTexture( texturaGanador );
                SDL_RenderPresent( renderer );
                SDL_Delay( 5000 );
                terminado = true;
            } else if( serv.hayQueLeer() ) {
                int id = serv.lanzar();
                texturaCartaLanzada = SDL_CreateTextureFromSurface( renderer, cartas[id - 1]->imagenSurface() );
                tablero.agregarCartaLanzada( id );
                SDL_RenderCopy( renderer, texturaCartaLanzada, nullptr, &rectCartaLanzada );
                SDL_DestroyTexture( texturaCartaLanzada );
            }
        } while ( !terminado );
        SDL_RenderClear( renderer );
    } while ( !salir );

    SDL_DestroyTexture( texturaTxtLanzada );
    SDL_DestroyTexture( texturaLoteria );
    for( Imagen* img : cartas ) {
        delete img;
    }
}
