#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <type_traits>

#include "SocketPortable.h"
#include "Poll.h"

#define TAMANO_BUFFER 128
#define TOTAL_JUGADORES 4

#define ERROR_SOCKET -1
#define ERROR_POLL -2

using namespace std;

enum Comandos : unsigned char { NOMBRE_REG, NOMBRE_OK, NOMBRE_OCUPADO, NUEVA_PARTIDA, LANZAMIENTO, LOTERIA, GANADOR};

using reloj = std::conditional <
              std::chrono::high_resolution_clock::is_steady,
              std::chrono::high_resolution_clock,
              std::chrono::steady_clock
              >::type;

vector<unsigned char> cartas;
vector<unsigned char> arraylanzar;

struct Jugador {
    char nombre[10];
    uint16_t ganados;
    char cartas[16];
    SocketPortable* socket;
};

void removerJugador( int i, Poll &poll, struct Jugador* jugadores, int &cantidadJugadores );
void anunciarPartida( Poll &poll, struct Jugador *jugadores, int &cantidadJugadores );

int main() {
    struct Jugador jugadores[TOTAL_JUGADORES];
    struct sockaddr_storage origen;
    struct sockaddr_in servidor;
    bool nombreRegistrado, partidaIniciada, jugadoresListos, ganador;
    char buffer[TAMANO_BUFFER + 1], ip[100];
    int conexion, leido, cantidadJugadores;
    reloj::time_point tiempo_inicio;
    Poll poll( TOTAL_JUGADORES + 1 );
    SocketPortable sp, *sockNuevo;
    socklen_t origen_len;
    Comandos cmd;

    for( int i = 0; i < TOTAL_JUGADORES; i++ ) {
        jugadores[i] = {{0}, 0, {0}, nullptr};
    }
    for( unsigned char i = 1; i <= 54; i++ ) {
        cartas.push_back( i );
    }

    memset( &servidor, 0, sizeof ( servidor ) );
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = htonl( INADDR_ANY );
    servidor.sin_port = htons( 17999 );
    partidaIniciada = jugadoresListos = false;
    try {
        if( !sp.socket( AF_INET, SOCK_STREAM, 0 ) ) throw ERROR_SOCKET;

        cout << "Socket de servidor creado" << endl;

        if( !sp.bind( ( struct sockaddr * ) &servidor, sizeof( servidor ) ) )throw ERROR_SOCKET;
        if( !sp.listen( 4 ) )throw ERROR_SOCKET;

        cout << "Escuchando en el puerto 17999 \r\nEsperando clientes. . ." << endl;

        poll.add( &sp, POLLIN, 0 );
        cantidadJugadores = 0;
        origen_len = sizeof ( origen );
        do {
            conexion = poll.poll( 50 );
            if ( conexion > 0 ) {
                if ( poll.checkRevents( 0, POLLIN ) ) {
                    memset( &origen, 0, sizeof ( sockaddr ) );
                    sockNuevo = sp.accept( ( struct sockaddr * )&origen, &origen_len );
                    if( cantidadJugadores < TOTAL_JUGADORES ) {
                        cout << "Nuevo cliente IP: ";
                        switch ( origen.ss_family ) {
                        case AF_INET:
                            cout << inet_ntop( AF_INET,  &( ( ( struct sockaddr_in * )&origen )->sin_addr ),  ip, INET_ADDRSTRLEN ) << endl;
                            break;
                        case AF_INET6:
                            cout << inet_ntop( AF_INET6,  &( ( ( struct sockaddr_in6 * )&origen )->sin6_addr ), ip, INET6_ADDRSTRLEN ) << endl;
                            break;
                        default:
                            break;
                        }
                        poll.add( sockNuevo, POLLIN, 0 );
                        jugadores[cantidadJugadores].socket = sockNuevo;
                        cantidadJugadores++;
                    } else {
                        delete sockNuevo;
                    }
                }
                for ( size_t i = 1; i < poll.size(); i++ ) {
                    if ( poll.checkRevents( i,  POLLIN ) ) {
                        leido = poll.getSocketPortable( i )->recv( buffer, TAMANO_BUFFER, 0 );
                        if ( leido > 0 ) {
                            buffer[leido] = 0;
                            memcpy( &cmd, &buffer[0], 1 );
                            switch( cmd ) {
                            case NOMBRE_REG:
                                nombreRegistrado = false;
                                for( int k = 0; !nombreRegistrado && k < cantidadJugadores; k++ ) {
                                    nombreRegistrado = memcmp( &jugadores[k].nombre, &buffer[1], 10 ) == 0;
                                }
                                if( !nombreRegistrado ) {
                                    memcpy( &jugadores[i - 1].nombre, &buffer[1], 10 ); // leido-1
                                    cmd = NOMBRE_OK;
                                    cout << "Jugador: ";
                                    for( int p = 1; p < leido; p++ )
                                        cout << buffer[p];
                                    cout << " registrado" << endl;
                                    jugadoresListos = ( cantidadJugadores == TOTAL_JUGADORES );
                                    tiempo_inicio = reloj::now();
                                } else {
                                    cmd = NOMBRE_OCUPADO;
                                }
                                memset( buffer, 0, sizeof( buffer ) );
                                memcpy( &buffer, reinterpret_cast<const char*>( &cmd ), 1 );
                                poll.getSocketPortable( i )->send( buffer, 1, 0 );
                                break;
                            case LOTERIA:
                                ganador = true;
                                for( size_t k = 0; k < 16; k++ ) {
                                    // Verificamos que todas las cartas se hayan lanzado
                                    if( find( arraylanzar.begin(), arraylanzar.end(), jugadores[i - 1].cartas[k] ) != arraylanzar.end() ) {
                                        ganador = false;
                                        break;
                                    }
                                }
                                if( ganador ) {
                                    cmd = GANADOR;
                                    memcpy( &buffer, reinterpret_cast<const char*>( &cmd ), 1 );
                                    memcpy( &buffer[1], &jugadores[i - 1].nombre, 10 );
                                    for( int k = 0; k < cantidadJugadores; k++ ) {
                                        poll.getSocketPortable( k + 1 )->send( buffer, 11, 0 );
                                    }
                                    cout << "Loteria de " << jugadores[i - 1].nombre << endl;
                                    jugadores[i - 1].ganados++;
                                    partidaIniciada = false;
                                    jugadoresListos = ( cantidadJugadores == TOTAL_JUGADORES );
                                    tiempo_inicio = reloj::now();
                                }
                                break;
                            case NOMBRE_OK:
                            case NOMBRE_OCUPADO:
                            case NUEVA_PARTIDA:
                            case LANZAMIENTO:
                            case GANADOR:
                                cout << "ERROR!!! El servidor no deberia recibir este comando" << endl;
                                break;
                            }
                        } else {
                            removerJugador( i - 1, poll, jugadores, cantidadJugadores );
                            jugadoresListos = false;
                        }
                    }
                }
            } else if ( conexion < 0 ) {
                throw ERROR_POLL;
            }
            if( partidaIniciada && cantidadJugadores == 0 ) {
                cout << "Juego terminado por falta de jugadores" << endl;
                partidaIniciada = jugadoresListos = false;
            }
            if( !partidaIniciada && jugadoresListos ) {
                if( std::chrono::duration_cast<std::chrono::seconds>( reloj::now() - tiempo_inicio ).count() > 10 ) {
                    anunciarPartida( poll, jugadores, cantidadJugadores );
                    partidaIniciada = true;
                    mt19937 g( static_cast<uint32_t>( time( nullptr ) ) );
                    shuffle( cartas.begin(), cartas.end(), g );
                    for( unsigned char i : cartas ) {
                        arraylanzar.push_back( i );
                    }
                    tiempo_inicio = reloj::now();
                }
            } else if( partidaIniciada ) {
                if( std::chrono::duration_cast<std::chrono::seconds>( reloj::now() - tiempo_inicio ).count() > 0.5 ) {
                    if( !arraylanzar.empty() ) {
                        cmd = LANZAMIENTO;
                        memcpy( &buffer, reinterpret_cast<const char*>( &cmd ), 1 );
                        memcpy( &buffer[1], &arraylanzar.back(), 1 );

                        for( int k = 0; k < cantidadJugadores; k++ ) {
                            if( poll.getSocketPortable( k + 1 )->send( buffer, 2, 0 ) < 0 ) {
                                removerJugador( k, poll, jugadores, cantidadJugadores );
                                k--;
                            }
                        }

                        cout << "Carta #" << ( unsigned int ) arraylanzar.back() << " lanzada " << endl;
                        arraylanzar.pop_back();
                        tiempo_inicio = reloj::now();
                    } else {
                        cout << "No hay mas lanzamientos, esperando loteria de alguien" << endl;
                        partidaIniciada = false;
                    }
                }
            }
        } while ( true );
    } catch( int e ) {
        if( e == ERROR_SOCKET ) {
            cout << sp.getLastErrorMessage() << endl;
        } else if( e == ERROR_POLL ) {
            perror( "Error en POLL" );
        }
        return e;
    }
    sp.close();
    return 0;
}


void removerJugador( int i, Poll &poll, struct Jugador* jugadores, int &cantidadJugadores ) {
    cout << "El jugador " << jugadores[i].nombre << " ha abandonado" << endl;
    jugadores[i] = jugadores[cantidadJugadores - 1];
    jugadores[cantidadJugadores - 1] = { {0}, 0, {0}, nullptr };
    cantidadJugadores--;
    delete poll.getSocketPortable( i + 1 );
    poll.remove( i + 1 );
}

void anunciarPartida( Poll &poll, struct Jugador *jugadores, int &cantidadJugadores ) {
    Comandos cmd;
    char buffers_jugadores[TOTAL_JUGADORES][65];
    char buffer[49];

    cmd = NUEVA_PARTIDA;
    memset( buffer, 0, sizeof( buffer ) );
    memcpy( &buffer, reinterpret_cast<const char*>( &cmd ), 1 );
    memcpy( &buffer[1], &jugadores[0].nombre, 10 );
    memcpy( &buffer[11], &jugadores[0].ganados, 2 );
    memcpy( &buffer[13], &jugadores[1].nombre, 10 );
    memcpy( &buffer[23], &jugadores[1].ganados, 2 );
    memcpy( &buffer[25], &jugadores[2].nombre, 10 );
    memcpy( &buffer[35], &jugadores[2].ganados, 2 );
    memcpy( &buffer[37], &jugadores[3].nombre, 10 );
    memcpy( &buffer[47], &jugadores[3].ganados, 2 );

    memcpy( &buffers_jugadores[0], &buffer, 49 );
    memcpy( &buffers_jugadores[1], &buffer, 49 );
    memcpy( &buffers_jugadores[2], &buffer, 49 );
    memcpy( &buffers_jugadores[3], &buffer, 49 );

    for( int k = 0, q = 0; k < cantidadJugadores; k++, q = 0 ) {
        mt19937 mt( static_cast<uint32_t>( time( nullptr ) ) );
        shuffle( cartas.begin(), cartas.end(), mt );
        cout << "Generando cartas del jugador: " << jugadores[k].nombre << "\r\n\t";
        for( int i = 49; i < 65; i++, q++ ) {
            cout << ( unsigned int ) cartas[q] << " ";
            memcpy( &buffers_jugadores[k][i], &cartas[q], 1 );
        }
        cout << endl;
    }
    cout << "Anunciando jugadores. . . " << endl;
    for( int k = 0; k < cantidadJugadores; k++ ) {
        if( poll.getSocketPortable( k + 1 )->send( buffers_jugadores[k], 65, 0 ) < 0 ) {
            removerJugador( k, poll, jugadores, cantidadJugadores );
            k--;
        }
    }
}
