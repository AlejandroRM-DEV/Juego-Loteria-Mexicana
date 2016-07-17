#include <iostream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <cstdint>

#include <SDL2/SDL_ttf.h>

#include "Boton.h"
#include "Tablero.h"
#include "Credencial.h"

#define VENTANA_ALTO 680
#define VENTANA_ANCHO 800

#define SIZE_OF_STRUCT_TABLERO 48
#define TOTAL_CARTAS 16
#define TAMANO_NOMBRE 10
#define TAMANO_BUFFER 128

using namespace std;

enum Comandos : unsigned char { NOMBRE_REG, NOMBRE_OK, NOMBRE_OCUPADO, NUEVA_PARTIDA, LANZAMIENTO, LOTERIA, GANADOR};

struct Marcador {
    char nombre1[TAMANO_NOMBRE + 1];
    int16_t ganados1;
    char nombre2[TAMANO_NOMBRE + 1];
    int16_t ganados2;
    char nombre3[TAMANO_NOMBRE + 1];
    int16_t ganados3;
    char nombre4[TAMANO_NOMBRE + 1];
    int16_t ganados4;
};

static Imagen imgFondo1( -1, "img/presentacion.jpg" );
static Imagen imgFondo2( -1, "img/todas1.png" );
static Imagen imgFondo3( -1, "img/todas2.png" );
static Imagen imgtxtTitulo( -1, "img/txt/titulo.png" );
static Imagen imgtxtLoteria( -1, "img/txt/loteria.png" );
static Imagen imgtxtEsperando( -1, "img/txt/esperando_jugadores.png" );
static Imagen imgtxtNombre( -1, "img/txt/nombre_jugador.png" );
static Imagen imgtxtLanzada( -1, "img/txt/lanzada.png" );
static Imagen imgtxtServidor( -1, "img/txt/servidor.png" );

bool sdl_quit = false;

string formatoJugador( string nombre, uint16_t ganados );
vector<Imagen*> generarCartas( );
vector<SDL_Texture*> crearFondosPantalla( SDL_Renderer *renderer );
bool pantallaInicio( Credencial* credencial, SDL_Renderer *renderer );
bool pantallaSalaPrevia( SDL_Renderer *renderer, Credencial* credencial, Tablero &tablero );
void pantallaJuego( SDL_Renderer *renderer, Credencial* credencial );
void fondoPantalla( vector<SDL_Texture*>& texturas, SDL_Renderer *renderer );
void crearMarcador( char* bytesJugadores, string jugadorLocal, SDL_Renderer *renderer );
void texturaEnCuadro( SDL_Texture *texto, SDL_Renderer *renderer, int x, int y, int ancho, int alto );
SDL_Color colorMarcador( const char* nombre, const char* nombrePropio );
SDL_Texture* texturaTexto( const string &texto, SDL_Color color, int tamano, SDL_Renderer *renderer );

int main( int argc, char **argv ) {
    Credencial* credencial;
    SDL_Init( SDL_INIT_VIDEO );
    IMG_Init( IMG_INIT_PNG );
    IMG_Init( IMG_INIT_JPG );
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow( "Loteria Mexicana", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                           VENTANA_ANCHO, VENTANA_ALTO, 0 );
    SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, 0 );
    SDL_Surface *icon = IMG_Load( "img/icono.png" );
    SDL_SetWindowIcon( window, icon );
    do {
        credencial = new Credencial();
        if( pantallaInicio( credencial, renderer ) ) {
            pantallaJuego( renderer, credencial );
        }
        delete credencial;
    } while( !sdl_quit );
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
        ss << "img/cartas/" << ( i + 1 )  << ".jpg";
        cartas[i] = new Imagen( ( i + 1 ), ss.str() );
    }
    return cartas;
}

void texturaEnCuadro( SDL_Texture *texto, SDL_Renderer *renderer, int x, int y, int ancho, int alto ) {
    SDL_Rect cuadro = {x, y, ancho, alto};
    SDL_RenderCopy( renderer, texto, nullptr, &cuadro );
}

SDL_Texture* texturaTexto( const string &texto, SDL_Color color, int tamano, SDL_Renderer *renderer ) {
    TTF_Font *fuente = TTF_OpenFont( "arial.ttf", tamano );
    SDL_Surface *superficie = TTF_RenderText_Blended( fuente, texto.c_str(), color );
    SDL_Texture *textura = SDL_CreateTextureFromSurface( renderer, superficie );
    SDL_FreeSurface( superficie );
    TTF_CloseFont( fuente );
    return textura;
}

void fondoPantalla( vector<SDL_Texture*>& texturas, SDL_Renderer *renderer ) {
    static int x = 0;
    x = ( ( ++x ) < 2760 ) ? x : 0;
    texturaEnCuadro( texturas[0], renderer, x, 73, 800, 535 );
    texturaEnCuadro( texturas[1], renderer, x - 980, 90, 980, 500 );
    texturaEnCuadro( texturas[2], renderer, x - 1960, 90, 980, 500 );
    texturaEnCuadro( texturas[0], renderer, x - 2760, 73, 800, 535 );
    texturaEnCuadro( texturas[3], renderer, 0, 10, 800, 96 );
}

vector<SDL_Texture*> crearFondosPantalla( SDL_Renderer *renderer ) {
    vector<SDL_Texture*> texturas( 4 );
    texturas[0] = SDL_CreateTextureFromSurface( renderer, imgFondo1.imagenSurface() );
    texturas[1] = SDL_CreateTextureFromSurface( renderer, imgFondo2.imagenSurface() );
    texturas[2] = SDL_CreateTextureFromSurface( renderer, imgFondo3.imagenSurface() );
    texturas[3] = SDL_CreateTextureFromSurface( renderer, imgtxtTitulo.imagenSurface() );
    return texturas;
}

string formatoJugador( string nombre, uint16_t ganados ) {
    stringstream ss;
    ss << nombre  << setw( 8 ) << setfill( '_' ) << ganados;
    return ss.str();
}

SDL_Color colorMarcador( const char* nombre, const char* nombrePropio ) {
    return ( strcmp( nombre, nombrePropio ) == 0 ) ?
           SDL_Color { 0, 0, 255,  SDL_ALPHA_OPAQUE }:
           SDL_Color { 0, 0, 0, SDL_ALPHA_OPAQUE };
}

void crearMarcador( char* bytesJugadores, string jugadorLocal, SDL_Renderer *renderer ) {
    struct Marcador marcador;
    vector<SDL_Texture*> texturasJugadores;
    SDL_Color color;
    memset( &marcador, 0, sizeof ( Marcador ) );
    memcpy( &marcador.nombre1, &bytesJugadores[0], TAMANO_NOMBRE );
    memcpy( &marcador.ganados1, &bytesJugadores[10], 2 );
    memcpy( &marcador.nombre2, &bytesJugadores[12], TAMANO_NOMBRE );
    memcpy( &marcador.ganados2, &bytesJugadores[22], 2 );
    memcpy( &marcador.nombre3, &bytesJugadores[24], TAMANO_NOMBRE );
    memcpy( &marcador.ganados3, &bytesJugadores[34],  2 );
    memcpy( &marcador.nombre4, &bytesJugadores[36], TAMANO_NOMBRE );
    memcpy( &marcador.ganados4, &bytesJugadores[46], 2 );

    marcador.ganados1 = ntohs( marcador.ganados1 );
    marcador.ganados2 = ntohs( marcador.ganados2 );
    marcador.ganados3 = ntohs( marcador.ganados3 );
    marcador.ganados4 = ntohs( marcador.ganados4 );

    color = colorMarcador( marcador.nombre1, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        texturaTexto( formatoJugador( marcador.nombre1, marcador.ganados1 ).c_str(), color, 32, renderer )
    );
    color = colorMarcador( marcador.nombre2, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        texturaTexto( formatoJugador( marcador.nombre2, marcador.ganados2 ).c_str(), color, 32, renderer )
    );
    color = colorMarcador( marcador.nombre3, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        texturaTexto( formatoJugador( marcador.nombre3, marcador.ganados3 ).c_str(), color, 32, renderer )
    );
    color = colorMarcador( marcador.nombre4, jugadorLocal.c_str() );
    texturasJugadores.push_back(
        texturaTexto( formatoJugador( marcador.nombre4, marcador.ganados4 ).c_str(), color, 32, renderer )
    );

    texturaEnCuadro( texturasJugadores.at( 0 ), renderer, 508, 520, 240, 24 );
    texturaEnCuadro( texturasJugadores.at( 1 ), renderer, 508, 550, 240, 24 );
    texturaEnCuadro( texturasJugadores.at( 2 ), renderer, 508, 580, 240, 24 );
    texturaEnCuadro( texturasJugadores.at( 3 ), renderer, 508, 610, 240, 24 );
    for( SDL_Texture* textura : texturasJugadores ) {
        SDL_DestroyTexture( textura );
    }
}

bool pantallaInicio( Credencial* credencial, SDL_Renderer *renderer ) {
    SDL_Texture *texturaServidor, *texturaTextoIngresado, *textoJugador, *texturaError;
    SDL_Rect cuadro = { 0, 300, 800, 100 };
    SDL_Event event;
    Comandos comando;
    struct addrinfo hints;
    char buffer[TAMANO_BUFFER];
    int totalBytes, msjErrorLen;
    bool error, terminado, conectado, retorno;
    vector<SDL_Texture*> texturasFondos =  crearFondosPantalla( renderer );
    string textoIngresado;

    texturaServidor = SDL_CreateTextureFromSurface( renderer, imgtxtServidor.imagenSurface() );
    textoJugador = SDL_CreateTextureFromSurface( renderer, imgtxtNombre.imagenSurface() );
    error = terminado = conectado = false;
    textoIngresado = "127.0.0.1";

    memset( &hints, 0, sizeof ( addrinfo ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    /*
        Si no se puede crear el socket cerramos la aplicacion, no podriamos continuar
    */
    if( !credencial->dameSocket()->socket( AF_INET, SOCK_STREAM, 0 ) ) {
        //cout << credencial->dameSocket()->getLastErrorMessage() << endl;
        terminado = true;
        retorno = false;
    }

    SDL_StartTextInput();
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
    while ( !terminado ) {
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
            case SDL_QUIT:
                sdl_quit = terminado = true;
                retorno = false;
            case SDL_KEYDOWN:
                if( event.key.keysym.sym == SDLK_BACKSPACE ) {
                    if( !textoIngresado.empty() ) {
                        textoIngresado.pop_back();
                    }
                } else if( event.key.keysym.sym == SDLK_RETURN ) {
                    if( !conectado ) {
                        if( textoIngresado.empty() ) {
                            error = true;
                            msjErrorLen = 32;
                            texturaError = texturaTexto( "No se ingreso el nombre del host", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                        } else {
                            if( credencial->dameSocket()->connect( textoIngresado.c_str(), "17999", &hints )
                                    && credencial->dameSocket()->setNonBlock() ) {
                                conectado = true;
                                error = false;
                                textoIngresado = "Jugador 00";
                            } else {
                                error = true;
                                msjErrorLen = 22;
                                texturaError = texturaTexto(  "Servidor no disponible", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                            }
                        }
                    } else {
                        if( textoIngresado.empty() ) {
                            error = true;
                            msjErrorLen = 32;
                            texturaError = texturaTexto(  "Debe elegir un nombre de jugador", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                        } else if( textoIngresado.size() > TAMANO_NOMBRE ) {
                            error = true;
                            msjErrorLen = 46;
                            texturaError = texturaTexto(  "Maximo 10 caracteres para el nombre de jugador", SDL_Color { 0, 0, 0, 255 }, 24,
                                                          renderer );
                        } else {
                            comando  = NOMBRE_REG;
                            memset( buffer, 0, sizeof( buffer ) );
                            memcpy( &buffer, reinterpret_cast<const char*>( &comando ), 1 );
                            memcpy( &buffer[1], textoIngresado.c_str(), 10 );
                            credencial->dameSocket()->send( buffer, 11, 0 );
                        }
                    }
                }
                break;
            case SDL_TEXTINPUT:
            case SDL_TEXTEDITING:
                textoIngresado.append( event.edit.text );
                break;
            }
            texturaTextoIngresado = texturaTexto( textoIngresado.c_str(), SDL_Color { 0, 0, 0, 255 }, 48, renderer );
        }

        SDL_RenderClear( renderer );
        fondoPantalla( texturasFondos, renderer );
        SDL_RenderFillRect( renderer, &cuadro );
        if( !conectado ) {
            texturaEnCuadro( texturaServidor, renderer, 0, 310, 800, 50 );
        } else {
            texturaEnCuadro( textoJugador, renderer, 0, 310, 800, 50 );
            /*  Solo leemos el numero de bytes que esperamos (1), leer mas puede leer cosas
                que serian procesadas en otro lado (struct Marcador)
            */
            totalBytes = credencial->dameSocket()->recv( buffer, sizeof( Comandos ), 0 );
            if ( totalBytes > 0 ) {
                buffer[totalBytes] = 0;
                memcpy( &comando, &buffer[0], 1 );
                if( comando == NOMBRE_OK ) {
                    terminado = true;
                    retorno = true;
                    credencial->fijaNombre( textoIngresado );
                } else if( comando == NOMBRE_OCUPADO ) {
                    error = true;
                    msjErrorLen = 40;
                    texturaError = texturaTexto(  "El nombre del jugador no esta disponible", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                }
            } else if ( totalBytes < 0 ) {
                if ( !credencial->dameSocket()->nonBlockNoError() ) {
                    //cout << credencial->dameSocket()->getLastErrorMessage() << endl;
                    terminado = true;
                    retorno = false;
                }
            } else {
                //cout << "Conexion cerrada" << endl;
                terminado = true;
                retorno = false;
            }
        }
        if( error ) {
            texturaEnCuadro( texturaError, renderer, 0, 650, msjErrorLen * 10, 24 );
        }

        texturaEnCuadro( texturaTextoIngresado, renderer, ( 400 - textoIngresado.size() * 7 ), 360,
                         textoIngresado.size() * 14, 40 );

        SDL_RenderPresent( renderer );
        SDL_Delay( 25 );
    }
    SDL_StopTextInput();
    SDL_DestroyTexture( texturaTextoIngresado );
    SDL_DestroyTexture( texturaServidor );
    SDL_DestroyTexture( textoJugador );
    for( SDL_Texture* textura : texturasFondos ) {
        SDL_DestroyTexture( textura );
    }
    return retorno;
}

/**
    Pantalla de espera de jugadores antes del inicio de cada partida.
    Cuando se completan 4 jugadores el servidor los da a conocer y se inicia la partida.
        - La pantalla se muestra cuando es juego nuevo.
        - Algun jugador abandono la partida.
**/
bool pantallaSalaPrevia( SDL_Renderer *renderer, Credencial *credencial, Tablero &tablero ) {
    SDL_Texture* texturaEsperando = SDL_CreateTextureFromSurface( renderer, imgtxtEsperando.imagenSurface() );
    SDL_Rect cuadro = { 0, 300, 800, 100 };
    SDL_Event event;
    Comandos comando;
    char bytesJugadores[SIZE_OF_STRUCT_TABLERO], bytesCartas[TOTAL_CARTAS], buffer[TAMANO_BUFFER];
    bool listo, retorno;
    int totalBytes;
    vector<SDL_Texture*> texturasFondos =  crearFondosPantalla( renderer );

    listo = false;
    retorno = true;
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
    while( !listo ) {
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
            case SDL_QUIT:
                retorno = false;
                sdl_quit = listo = true;
                break;
            }
        }
        totalBytes = credencial->dameSocket()->recv( buffer, SIZE_OF_STRUCT_TABLERO + TOTAL_CARTAS + 1, 0 );
        if ( totalBytes > 0 ) {
            buffer[totalBytes] = 0;
            memcpy( &comando, &buffer[0], 1 );
            if( comando == NUEVA_PARTIDA ) {
                memcpy( &bytesJugadores, &buffer[1], SIZE_OF_STRUCT_TABLERO );
                memcpy( &bytesCartas, &buffer[49], TOTAL_CARTAS );
                retorno = listo = true;
            }
        } else if ( totalBytes < 0 ) {
            if ( !credencial->dameSocket()->nonBlockNoError() ) {
                // cout << credencial->dameSocket()->getLastErrorMessage() << endl;
                retorno = false;
                listo = true;
            }
        } else {
            //cout << "Conexion cerrada" << endl;
            retorno = false;
            listo = true;
        }
        SDL_RenderClear( renderer );
        fondoPantalla( texturasFondos, renderer );
        SDL_RenderFillRect( renderer, &cuadro );
        texturaEnCuadro( texturaEsperando, renderer, 0, 300, 800, 100 );
        SDL_RenderPresent( renderer );
        SDL_Delay( 25 );
    }
    SDL_RenderClear( renderer );
    crearMarcador( bytesJugadores, credencial->dameNombre(), renderer );
    tablero.reiniciar( bytesCartas );

    for( SDL_Texture* textura : texturasFondos ) {
        SDL_DestroyTexture( textura );
    }
    SDL_DestroyTexture( texturaEsperando );
    return retorno;
}

void pantallaJuego( SDL_Renderer * renderer, Credencial * credencial ) {
    SDL_Texture *texturaTxtLanzada, *texturaCartaLanzada, *texturaGanador, *texturaLoteria;
    SDL_Rect rectCartaLanzada = { 515, 70, 228, 350 };
    SDL_Event event;
    char buffer[TAMANO_BUFFER], ganador[TAMANO_NOMBRE + 1];
    bool terminado, salir, habilitaBoton;
    unsigned char carta;
    int totalBytes;
    Comandos comando;
    vector<Imagen*> cartas;
    Boton btnLoteria( 528, 440, 200, 50, "img/botonLoteria.png", renderer );
    Tablero tablero( renderer );

    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE  );
    SDL_RenderClear( renderer );
    cartas = generarCartas();
    texturaLoteria = SDL_CreateTextureFromSurface( renderer, imgtxtLoteria.imagenSurface() );
    texturaTxtLanzada = SDL_CreateTextureFromSurface( renderer, imgtxtLanzada.imagenSurface() );
    salir = false;
    do {
        if( !pantallaSalaPrevia( renderer, credencial,  tablero ) ) {
            break;
        }

        btnLoteria.render( renderer );
        SDL_SetRenderDrawColor( renderer, 224, 224, 224, SDL_ALPHA_OPAQUE );
        texturaEnCuadro( texturaTxtLanzada, renderer, 528, 20, 200, 40 );
        SDL_RenderFillRect( renderer, &rectCartaLanzada );
        terminado = habilitaBoton = false;

        do {
            SDL_RenderPresent( renderer );
            while ( SDL_PollEvent( &event ) ) {
                switch ( event.type ) {
                case SDL_QUIT:
                    sdl_quit = salir = true;
                    terminado = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if( tablero.verficaClic( event.motion.x, event.motion.y ) ) {
                        if( tablero.restantes() == 0 ) {
                            habilitaBoton = true;
                        }
                    } else if( habilitaBoton && btnLoteria.validaClic( event.motion.x, event.motion.y ) ) {
                        comando = LOTERIA;
                        memcpy( &buffer, reinterpret_cast<const char*>( &comando ), 1 );
                        credencial->dameSocket()->send( buffer, 1, 0 );
                    }
                    break;
                }
            }

            totalBytes = credencial->dameSocket()->recv( buffer, TAMANO_BUFFER, 0 );
            if ( totalBytes > 0 ) {
                buffer[totalBytes] = 0;
                memcpy( &comando, &buffer[0], 1 );
                if( comando == LANZAMIENTO ) {
                    memcpy( &carta, &buffer[1], 1 );
                    texturaCartaLanzada = SDL_CreateTextureFromSurface( renderer, cartas[carta - 1]->imagenSurface() );
                    tablero.agregarCartaLanzada( carta );
                    SDL_RenderCopy( renderer, texturaCartaLanzada, nullptr, &rectCartaLanzada );
                    SDL_DestroyTexture( texturaCartaLanzada );
                } else if( comando == GANADOR ) {
                    terminado = true;
                    memcpy( &ganador, &buffer[1], TAMANO_NOMBRE );
                }
            } else if ( totalBytes < 0 ) {
                if ( !credencial->dameSocket()->nonBlockNoError() ) {
                    //cout << credencial->dameSocket()->getLastErrorMessage() << endl;
                    terminado = salir = true;
                }
            } else {
                //cout << "Conexion cerrada" << endl;
                terminado = salir = true;
            }
            if( terminado && !salir ) {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
                SDL_Rect cuadro = { 0, 0, 800, 680 };
                SDL_RenderFillRect( renderer, &cuadro );
                texturaEnCuadro( texturaLoteria, renderer, 0, 200, 800, 180 );
                texturaGanador  = texturaTexto( ganador, SDL_Color { 0, 0, 0, 255 }, 110, renderer );
                texturaEnCuadro( texturaGanador, renderer, 180, 360, 440, 110 );
                SDL_DestroyTexture( texturaGanador );
                SDL_RenderPresent( renderer );
                SDL_Delay( 5000 );
                terminado = true;
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
