#include <iostream>
#include <sstream>
#include <cstdio>

#include "SocketPortable.h"
#include "Poll.h"

using namespace std;

#define TAMANO_BUFFER 128
#define TOTAL_JUGADORES 4

#define ERROR_SOCKET -1
#define ERROR_POLL -2

int main() {
    char buffer[TAMANO_BUFFER + 1], ip[100];
    SocketPortable sp, *sockNuevo;
    struct sockaddr_storage origen;
    struct sockaddr_in servidor;
    socklen_t origen_len;
    int conexion, leido, numJugadores;
    Poll poll( 5 );

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
        numJugadores = 0;
        origen_len = sizeof ( origen );
        do {
            conexion = poll.poll( 50 );
            if ( conexion > 0 ) {
                if ( poll.checkRevents( 0, POLLIN ) ) {
                    memset( &origen, 0, sizeof ( sockaddr ) );
                    sockNuevo = sp.accept( ( struct sockaddr * )&origen, &origen_len );
                    if( numJugadores < TOTAL_JUGADORES ) {
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
                        numJugadores++;
                    } else {
                        delete sockNuevo;
                    }
                }
                for ( size_t i = 1; i < poll.size(); i++ ) {
                    if ( poll.checkRevents( i,  POLLIN ) ) {
                        leido = poll.getSocketPortable( i )->recv( buffer, TAMANO_BUFFER, 0 );
                        if ( leido > 0 ) {
                            buffer[leido] = 0;
                            /***
                                PROCESAR TODA LA LOGICA DEL JUEGO AQUI
                            ***/
                            cout << " MENSAJE: " << endl << buffer << endl;
                        } else {
                            cout << "El cliente (fd: " << poll.getSocketPortable( i )->getFD() << ") ha abandonado" << endl;
                            delete poll.getSocketPortable( i );
                            poll.remove( i-- );
                        }
                    }
                }
            } else if ( conexion < 0 ) {
                throw ERROR_POLL;
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
