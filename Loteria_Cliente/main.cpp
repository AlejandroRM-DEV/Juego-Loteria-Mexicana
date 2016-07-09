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

enum Comandos : unsigned char { NOMBRE_REG, NOMBRE_OK, NOMBRE_OCUPADO, NUEVA_PARTIDA, LANZAMIENTO, LOTERIA, GANADOR};

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

#define SIZE_OF_STRUCT_JUGADORES 48
#define TAMANO_BUFFER 128

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
SDL_Texture* renderTexto( const string &texto, SDL_Color color, int tamano, SDL_Renderer *renderer );
bool pantallaInicio( Credencial* credencial, SDL_Renderer *renderer );
void crearPuntajes( char* bytesJugadores, string jugadorLocal, SDL_Renderer *renderer );
void pantallaJuego( SDL_Renderer *renderer, Credencial* credencial );
void fondoPantalla( vector<SDL_Texture*>& texturas, SDL_Renderer *renderer );
bool salaPrevia( SDL_Renderer *renderer, Credencial* credencial, Tablero &tablero );
void renderTexturaEnRect( SDL_Texture *texto, SDL_Renderer *renderer, int x, int y, int ancho, int alto );

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

    Credencial* credencial = new Credencial();
    if( pantallaInicio( credencial, renderer ) ) {
        pantallaJuego( renderer, credencial );
    }
    delete credencial;

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

void renderTexturaEnRect( SDL_Texture *texto, SDL_Renderer *renderer, int x, int y, int ancho, int alto ) {
    SDL_Rect dst = {x, y, ancho, alto};
    SDL_RenderCopy( renderer, texto, nullptr, &dst );
}

SDL_Texture* renderTexto( const string &texto, SDL_Color color, int tamano, SDL_Renderer *renderer ) {
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

void crearPuntajes( char* bytesJugadores, string jugadorLocal, SDL_Renderer *renderer ) {
    struct Jugadores jugadores;
    vector<SDL_Texture*> texturasJugadores;
    SDL_Color color;

    memcpy( &jugadores.nombre1, &bytesJugadores[0], 10 );
    memcpy( &jugadores.ganados1, &bytesJugadores[10], 2 );
    memcpy( &jugadores.nombre2, &bytesJugadores[12], 10 );
    memcpy( &jugadores.ganados2, &bytesJugadores[22], 2 );
    memcpy( &jugadores.nombre3, &bytesJugadores[24], 10 );
    memcpy( &jugadores.ganados3, &bytesJugadores[34],  2 );
    memcpy( &jugadores.nombre4, &bytesJugadores[36], 10 );
    memcpy( &jugadores.ganados4, &bytesJugadores[46], 2 );

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

bool pantallaInicio( Credencial* credencial, SDL_Renderer *renderer ) {
    struct addrinfo hints;
    Comandos cmd;
    char buffer[TAMANO_BUFFER];
    int leido;

    SDL_Texture *texturaServidor, *texturaTextoIngresado, *textoJugador, *texturaError;
    vector<SDL_Texture*> texturasFondos =  fondos( renderer );
    SDL_Event event;
    std::chrono::steady_clock::time_point tInicio;
    string textoIngresado;
    int msjErrorLen;
    bool error, terminado, conectado, retorno;

    texturaServidor = SDL_CreateTextureFromSurface( renderer, imgtxtServidor.imagenSurface() );
    textoJugador = SDL_CreateTextureFromSurface( renderer, imgtxtNombre.imagenSurface() );
    SDL_Rect hint = { 0, 300, 800, 100 };
    error = terminado = conectado = false;
    textoIngresado = "127.0.0.1";
    tInicio = std::chrono::steady_clock::now();

    memset( &hints, 0, sizeof ( addrinfo ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    /*
        Si no se puede crear el socket cerramos la aplicacion, no podriamos continuar
    */
    if( !credencial->dameSocket()->socket( AF_INET, SOCK_STREAM, 0 ) ) {
        cout << credencial->dameSocket()->getLastErrorMessage() << endl;
        terminado = true;
        retorno = false;
    }

    SDL_StartTextInput();
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
    while ( !terminado ) {
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
            case SDL_QUIT:
                terminado = true;
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
                            texturaError = renderTexto( "No se ingreso el nombre del host", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                        } else {
                            if( credencial->dameSocket()->connect( textoIngresado.c_str(), "17999", &hints )
                                    && credencial->dameSocket()->setNonBlock() ) {
                                conectado = true;
                                error = false;
                                textoIngresado = "Jugador 00";
                            } else {
                                error = true;
                                msjErrorLen = 22;
                                texturaError = renderTexto(  "Servidor no disponible", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                            }
                        }
                    } else {
                        if( textoIngresado.empty() ) {
                            error = true;
                            msjErrorLen = 32;
                            texturaError = renderTexto(  "Debe elegir un nombre de jugador", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                        } else if( textoIngresado.size() > 10 ) {
                            error = true;
                            msjErrorLen = 46;
                            texturaError = renderTexto(  "Maximo 10 caracteres para el nombre de jugador", SDL_Color { 0, 0, 0, 255 }, 24,
                                                         renderer );
                        } else {
                            cmd  = NOMBRE_REG;
                            memset( buffer, 0, sizeof( buffer ) );
                            memcpy( &buffer, reinterpret_cast<const char*>( &cmd ), 1 );
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
            texturaTextoIngresado = renderTexto( textoIngresado.c_str(), SDL_Color { 0, 0, 0, 255 }, 48, renderer );
        }

        SDL_RenderClear( renderer );
        fondoPantalla( texturasFondos, renderer );
        SDL_RenderFillRect( renderer, &hint );
        if( !conectado ) {
            renderTexturaEnRect( texturaServidor, renderer, 0, 310, 800, 50 );
        } else {
            renderTexturaEnRect( textoJugador, renderer, 0, 310, 800, 50 );
            /*  Solo leemos el numero de bytes que esperamos (1), leer mas puede leer cosas
                que serian procesadas en otro lado (struct Jugadores)
            */
            leido = credencial->dameSocket()->recv( buffer, sizeof( Comandos ), 0 );
            if ( leido > 0 ) {
                buffer[leido] = 0;
                memcpy( &cmd, &buffer[0], 1 );
                if( cmd == NOMBRE_OK ) {
                    terminado = true;
                    retorno = true;
                    credencial->fijaNombre( textoIngresado );
                } else if( cmd == NOMBRE_OCUPADO ) {
                    error = true;
                    msjErrorLen = 40;
                    texturaError = renderTexto(  "El nombre del jugador no esta disponible", SDL_Color { 0, 0, 0, 255 }, 24, renderer );
                }
            } else if ( leido < 0 ) {
                if ( !credencial->dameSocket()->nonBlockNoError() ) {
                    cout << credencial->dameSocket()->getLastErrorMessage() << endl;
                    terminado = true;
                    retorno = false;
                }
            } else {
                cout << "Conexion cerrada" << endl;
                terminado = true;
                retorno = false;
            }
        }
        if( error ) {
            renderTexturaEnRect( texturaError, renderer, 0, 650, msjErrorLen * 10, 24 );
        }

        renderTexturaEnRect( texturaTextoIngresado, renderer, ( 400 - textoIngresado.size() * 7 ), 360,
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
bool salaPrevia( SDL_Renderer *renderer, Credencial *credencial, Tablero &tablero ) {
    char bytesJugadores[SIZE_OF_STRUCT_JUGADORES];
    char bytesCartas[16];
    char buffer[TAMANO_BUFFER];
    int leido;
    Comandos cmd;
    SDL_Event event;
    SDL_Rect hint = { 0, 300, 800, 100 };
    SDL_Texture* texturaEsperando = SDL_CreateTextureFromSurface( renderer, imgtxtEsperando.imagenSurface() );
    vector<SDL_Texture*> texturasFondos =  fondos( renderer );
    bool listo = false, retorno = true;
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
    while( !listo ) {
        while ( SDL_PollEvent( &event ) ) {
            switch ( event.type ) {
            case SDL_QUIT:
                retorno = false;
                listo = true;
                break;
            }
        }
        leido = credencial->dameSocket()->recv( buffer, SIZE_OF_STRUCT_JUGADORES + 17, 0 );
        if ( leido > 0 ) {
            buffer[leido] = 0;
            memcpy( &cmd, &buffer[0], 1 );
            if( cmd == NUEVA_PARTIDA ) {
                memcpy( &bytesJugadores, &buffer[1], SIZE_OF_STRUCT_JUGADORES );
                memcpy( &bytesCartas, &buffer[49], 16 );
                retorno = listo = true;
            }
        } else if ( leido < 0 ) {
            if ( !credencial->dameSocket()->nonBlockNoError() ) {
                cout << credencial->dameSocket()->getLastErrorMessage() << endl;
                retorno = false;
                listo = true;
            }
        } else {
            cout << "Conexion cerrada" << endl;
            retorno = false;
            listo = true;
        }
        SDL_RenderClear( renderer );
        fondoPantalla( texturasFondos, renderer );
        SDL_RenderFillRect( renderer, &hint );
        renderTexturaEnRect( texturaEsperando, renderer, 0, 300, 800, 100 );
        SDL_RenderPresent( renderer );
        SDL_Delay( 25 );
    }
    SDL_RenderClear( renderer );
    crearPuntajes( bytesJugadores, credencial->dameNombre(), renderer );
    tablero.reiniciar( bytesCartas );

    for( SDL_Texture* textura : texturasFondos ) {
        SDL_DestroyTexture( textura );
    }
    SDL_DestroyTexture( texturaEsperando );
    return retorno;
}

void pantallaJuego( SDL_Renderer * renderer, Credencial * credencial ) {
    Boton btnLoteria( 528, 440, 200, 50, "img/botonLoteria.PNG", renderer );
    Tablero tablero( renderer );
    bool terminado, salir, habilitaBoton;
    char buffer[TAMANO_BUFFER], ganador[10];
    unsigned char carta;
    int leido;
    Comandos cmd;
    vector<Imagen*> cartas;
    SDL_Texture *texturaTxtLanzada, *texturaCartaLanzada, *texturaGanador, *texturaLoteria;
    SDL_Event event;

    SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE  );
    SDL_RenderClear( renderer );
    cartas = generarCartas();
    texturaLoteria = SDL_CreateTextureFromSurface( renderer, imgtxtLoteria.imagenSurface() );
    SDL_Rect rectCartaLanzada = { 515, 70, 228, 350 };
    texturaTxtLanzada = SDL_CreateTextureFromSurface( renderer, imgtxtLanzada.imagenSurface() );
    salir = false;
    do {
        if( !salaPrevia( renderer, credencial,  tablero ) ) {
            break;
        }

        btnLoteria.render( renderer );
        SDL_SetRenderDrawColor( renderer, 224, 224, 224, SDL_ALPHA_OPAQUE );
        renderTexturaEnRect( texturaTxtLanzada, renderer, 528, 20, 200, 40 );
        SDL_RenderFillRect( renderer, &rectCartaLanzada );
        terminado = habilitaBoton = false;

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
                        cmd = LOTERIA;
                        memcpy( &buffer, reinterpret_cast<const char*>( &cmd ), 1 );
                        credencial->dameSocket()->send( buffer, 1, 0 );
                    }
                    break;
                }
            }

            leido = credencial->dameSocket()->recv( buffer, TAMANO_BUFFER, 0 );
            if ( leido > 0 ) {
                buffer[leido] = 0;
                memcpy( &cmd, &buffer[0], 1 );
                if( cmd == LANZAMIENTO ) {
                    memcpy( &carta, &buffer[1], 1 );
                    texturaCartaLanzada = SDL_CreateTextureFromSurface( renderer, cartas[carta - 1]->imagenSurface() );
                    tablero.agregarCartaLanzada( carta );
                    SDL_RenderCopy( renderer, texturaCartaLanzada, nullptr, &rectCartaLanzada );
                    SDL_DestroyTexture( texturaCartaLanzada );
                } else if( cmd == GANADOR ) {
                    terminado = true;
                    memcpy( &ganador, &buffer[1], 10 );
                }
            } else if ( leido < 0 ) {
                if ( !credencial->dameSocket()->nonBlockNoError() ) {
                    cout << credencial->dameSocket()->getLastErrorMessage() << endl;
                    terminado = salir = true;
                }
            } else {
                cout << "Conexion cerrada" << endl;
                terminado = salir = true;
            }
            if( terminado && !salir ) {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
                SDL_Rect hint = { 0, 0, 800, 680 };
                SDL_RenderFillRect( renderer, &hint );
                renderTexturaEnRect( texturaLoteria, renderer, 0, 200, 800, 180 );
                texturaGanador  = renderTexto( ganador, SDL_Color { 0, 0, 0, 255 },110, renderer );
                renderTexturaEnRect( texturaGanador, renderer, 180, 360, 440, 110 );
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
