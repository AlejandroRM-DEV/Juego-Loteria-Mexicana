#ifndef SERVERPRUEBAINTERFAZ_H_INCLUDED
#define SERVERPRUEBAINTERFAZ_H_INCLUDED

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <sys/time.h>
#include <cstdlib>

class MiniServidor {
    vector<int> cartas;
     long long milisegundosActuales;

public:
    MiniServidor() {
        for( int i = 0; i < 54; i++ ) {
            cartas.push_back(i);
        }
    }

    int* pruebaSeleccionados() {
        int *seleccionados = new int[16] {0};

        auto engine = default_random_engine{};
        shuffle(begin(cartas), end(cartas), engine);
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
        if( ( milisegundos() - milisegundosActuales ) >= 5000  ) {
            milisegundosActuales = milisegundos();
            return true;
        }
        return false;
    }

    int lanzar() {
        return rand() % 54 + 1;
    }
};

#endif // SERVERPRUEBAINTERFAZ_H_INCLUDED
