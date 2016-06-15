#ifndef TABLERO_H_INCLUDED
#define TABLERO_H_INCLUDED

#include <vector>

#include "Casilla.h"

class Tablero {
public:
    vector<vector<Casilla*>> tablero{4, vector<Casilla*>( 4 ) };

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
};

#endif // TABLERO_H_INCLUDED
