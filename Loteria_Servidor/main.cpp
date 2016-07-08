#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>

#include "SocketPortable.h"
#include "Poll.h"

using namespace std;

#define TAMANO_BUFFER 128
#define TOTAL_JUGADORES 2

#define ERROR_SOCKET -1
#define ERROR_POLL -2

enum Comandos : unsigned char { NOMBRE_REG, NOMBRE_OK, NOMBRE_OCUPADO, TABLERO_REG, CARTAS_REG, LANZAMIENTO, LOTERIA, GANADOR};

vector<unsigned char> cartas;
vector<unsigned char> arraylanzar;

struct Jugador {
    char nombre[10];
    uint16_t ganados;
    char cartas[16];
    SocketPortable* socekt;
};

void removerJugador( int i, Poll &poll, struct Jugador* jugadores, int &cantidadJugadores );
void init( Poll &poll, struct Jugador *jugadores, int &cantidadJugadores );

int main() {
    char buffer[TAMANO_BUFFER + 1], ip[100];
    SocketPortable sp, *sockNuevo;
    struct sockaddr_storage origen;
    struct sockaddr_in servidor;
    socklen_t origen_len;
    int conexion, leido, cantidadJugadores;
    Poll poll( TOTAL_JUGADORES + 1 );
    Comandos cmd;
    bool registrado, jugadoresListos = false;
    struct Jugador jugadores[TOTAL_JUGADORES];
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
                        jugadores[cantidadJugadores].socekt = sockNuevo;
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
                                registrado = false;
                                for( int k = 0; !registrado && k < cantidadJugadores; k++ ) {
                                    registrado = memcmp( &jugadores[k].nombre, &buffer[1], 10 ) == 0;
                                }
                                if( !registrado ) {
                                    memcpy( &jugadores[i - 1].nombre, &buffer[1], 10 ); // leido-1
                                    cmd = NOMBRE_OK;
                                    jugadoresListos = ( cantidadJugadores == TOTAL_JUGADORES );
                                    cout << "Jugador: ";
                                    for( int p = 1; p < leido; p++ )
                                        cout << buffer[p];
                                    cout << " registrado" << endl;
                                } else {
                                    cmd = NOMBRE_OCUPADO;
                                }
                                memset( buffer, 0, sizeof( buffer ) );
                                memcpy( &buffer, reinterpret_cast<const char*>( &cmd ), 1 );
                                poll.getSocketPortable( i )->send( buffer, 1, 0 );
                                break;
                            case LOTERIA:
                                // Revisar si ya ha ganado
                                break;
                            case NOMBRE_OK:
                            case NOMBRE_OCUPADO:
                            case TABLERO_REG:
                            case CARTAS_REG:
                            case LANZAMIENTO:
                            case GANADOR:
                                cout << "ERROR!!! El servidor no deberia recibir este comando" << endl;
                                break;
                            }
                        } else {
                            removerJugador( i, poll, jugadores, cantidadJugadores );
                        }
                    }
                }
            } else if ( conexion < 0 ) {
                throw ERROR_POLL;
            }
            if( jugadoresListos ) {
                init( poll, jugadores, cantidadJugadores );
                jugadoresListos = false;
            }
            /****************************

            LANZAMIENTOS DE CARTAS

            ****************************/
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
    cout << "El cliente (fd: " << poll.getSocketPortable( i )->getFD() << ") ha abandonado" << endl;
    jugadores[i] = jugadores[cantidadJugadores - 1];
    jugadores[cantidadJugadores - 1] = { {0}, 0, {0}, nullptr };
    cantidadJugadores--;
    delete poll.getSocketPortable( i );
    poll.remove( i-- );
}

void init( Poll &poll, struct Jugador *jugadores, int &cantidadJugadores ) {
    Comandos cmd;
    char buffer_j[4][65];
    char buffer[49];

    cmd = TABLERO_REG;
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

    memcpy( &buffer_j[0], &buffer, 49 );
    memcpy( &buffer_j[1], &buffer, 49 );
    memcpy( &buffer_j[2], &buffer, 49 );
    memcpy( &buffer_j[3], &buffer, 49 );

    for( int k = 0, q = 0; k < 4; k++, q = 0 ) {
        mt19937 g( static_cast<uint32_t>( time( nullptr ) ) );
        shuffle( cartas.begin(), cartas.end(), g );
        cout << "Generando cartas del jugador: " << jugadores[k].nombre << "\r\n\t";
        for( int i = 49; i < 65; i++, q++ ) {
            cout << ( unsigned int ) cartas[q] << " ";
            memcpy( &buffer_j[k][i], &cartas[q], 1 );
        }
        cout << endl;
    }
    cout << "Anunciando jugadores. . . " << endl;
    if( poll.getSocketPortable( 1 )->send( buffer_j[0], 65, 0 ) < 0 )
        removerJugador( 1, poll, jugadores, cantidadJugadores );
    if( poll.getSocketPortable( 2 )->send( buffer_j[1], 65, 0 ) < 0 )
        removerJugador( 2, poll, jugadores, cantidadJugadores );
    /*if( poll.getSocketPortable( 3 )->send( buffer_j[2], 65, 0 ) < 0 )
        removerJugador( 3, poll, jugadores, cantidadJugadores );
    if( poll.getSocketPortable( 4 )->send( buffer_j[3], 65, 0 ) < 0 )
        removerJugador( 4, poll, jugadores, cantidadJugadores );
    */
}
