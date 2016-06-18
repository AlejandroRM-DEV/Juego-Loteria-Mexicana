#ifndef TABLERO_H_INCLUDED
#define TABLERO_H_INCLUDED

#include <iostream>
#include <vector>
#include <set>

#include "Casilla.h"

using namespace std;

class Tablero {
private:
    vector<vector<Casilla*>> tablero{4, vector<Casilla*>( 4 ) };
    vector<Imagen*> cartas;
    set<char> lanzadas;
    SDL_Renderer * renderer;

public:
    Tablero( vector<Imagen*>& cartas, SDL_Renderer * renderer ) {
        this->renderer = renderer;
        this->cartas = cartas;
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < 4; j++ ) {
                tablero[i][j] = new Casilla( 20 + ( i * CASILLA_ANCHO ), 20 + ( j * CASILLA_ALTO ),
                                             renderer );
            }
        }
    }
    ~Tablero() {
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < 4; j++ ) {
                delete tablero[i][j];
            }
        }
    }
    void reiniciar( int* seleccionadas ) {
        int indiceSel = 0;
        lanzadas.clear();
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < 4; j++ ) {
                tablero[i][j]->reiniciar( cartas[seleccionadas[indiceSel++] - 1] );
            }
        }
    }
    void agregarCartaLanzada( char id ) {
        lanzadas.insert( id );
    }

    bool verficaClic( int x, int y ) {
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < 4; j++ ) {
                if( !tablero[i][j]->estaMarcada() && lanzadas.find( tablero[i][j]->idCarta() ) != lanzadas.end() &&
                        tablero[i][j]->validaClic( x, y ) ) {
                    tablero[i][j]->marcar();
                    return true;
                }
            }
        }
        return false;
    }
};

#endif // TABLERO_H_INCLUDED
