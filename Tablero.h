#ifndef TABLERO_H_INCLUDED
#define TABLERO_H_INCLUDED

#include <vector>
#include <set>

#include "Casilla.h"

class Tablero {
public:
    vector<vector<Casilla*>> tablero{4, vector<Casilla*>( 4 ) };
    set<char> lanzadas;

    Tablero( vector<Imagen*>& cartas, char* seleccionadas, SDL_Renderer * renderer ) {
        int indiceSel = 0;
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < 4; j++ ) {
                tablero[i][j] = new Casilla( 20 + ( i * CASILLA_ANCHO ),
                                             20 + ( j * CASILLA_ALTO ),
                                             cartas[seleccionadas[indiceSel++]],
                                             renderer );
            }
        }
    }

    void agregarCartaLanzada( char id ) {
        lanzadas.insert( id );
    }

    ~Tablero() {
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < 4; j++ ) {
                delete tablero[i][j];
            }
        }
    }
    void verficaClic( int x, int y, SDL_Renderer * renderer ) {
        bool seguir = true;
        for( int i = 0; seguir && i < 4; i++ ) {
            for( int j = 0; seguir && j < 4; j++ ) {
                if( !tablero[i][j]->estaMarcada() && lanzadas.find( tablero[i][j]->idCarta() ) != lanzadas.end() &&
                        tablero[i][j]->validaClic( x, y ) ) {
                    tablero[i][j]->marcar( renderer );
                    seguir = false;
                }
            }
        }
    }
};

#endif // TABLERO_H_INCLUDED
