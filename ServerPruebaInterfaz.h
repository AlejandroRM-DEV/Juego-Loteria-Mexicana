#ifndef SERVERPRUEBAINTERFAZ_H_INCLUDED
#define SERVERPRUEBAINTERFAZ_H_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

#include <sys/time.h>
#include <cstdlib>

class MiniServidor {
    vector<int> cartas;
    long long milisegundosActuales;

public:
    MiniServidor() {
        srand ( unsigned ( time( nullptr ) ) );
        for( int i = 1; i <= 54; i++ ) {
            cartas.push_back( i );
        }
    }

    int* pruebaSeleccionados() {
        int *seleccionados = new int[16] {0};
        mt19937 g( static_cast<uint32_t>( time( nullptr ) ) );
        shuffle( cartas.begin(), cartas.end(), g );
        for( int i = 0; i < 16; i++ ) {
            seleccionados[i] = cartas[i];
        }

        return seleccionados;
    }

    void iniciarReloj() {
        milisegundosActuales = milisegundos();
    }

    long long milisegundos() {
        struct timeval te;
        gettimeofday( &te, nullptr );
        long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
        return milliseconds;
    }

    bool hayQueLeer() {
        if( ( milisegundos() - milisegundosActuales ) >= 1000  ) {
            milisegundosActuales = milisegundos();
            return true;
        }
        return false;
    }

    int lanzar() {
        return rand() % 54 + 1;
    }

    char* datosJugadores() {
        char* datosJugadores = new char[sizeof( struct Jugadores  )];
        struct Jugadores datos;

        memset( datosJugadores, 0, sizeof( struct Jugadores  ) );
        strcpy( datos.nombre1, "AlejandroR" );
        datos.ganados1 = 0;
        strcpy( datos.nombre2, "Jose" );
        datos.ganados2 = 1;
        strcpy( datos.nombre3, "Roberto" );
        datos.ganados3 = 2;
        strcpy( datos.nombre4, "Karla" );
        datos.ganados4 = 3;

        memcpy( datosJugadores, &datos, sizeof( struct Jugadores  ) );
        return datosJugadores;
    }
};

#endif // SERVERPRUEBAINTERFAZ_H_INCLUDED
