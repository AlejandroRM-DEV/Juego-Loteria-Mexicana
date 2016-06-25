#ifndef CREDENCIAL_H_INCLUDED
#define CREDENCIAL_H_INCLUDED

#include <iostream>

using namespace std;

class Credencial {
private:
    int servidorfd;
    string jugador;
public:
    Credencial( int servidorfd, string jugador ) {
        this->servidorfd = servidorfd;
        this->jugador = jugador;
    }
    int dameServidorfd() {
        return servidorfd;
    }
    string dameJugador() {
        return jugador;
    }
};

#endif // CREDENCIAL_H_INCLUDED
