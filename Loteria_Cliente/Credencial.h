#ifndef CREDENCIAL_H_INCLUDED
#define CREDENCIAL_H_INCLUDED

#include <iostream>

#include "SocketPortable.h"

using namespace std;

class Credencial {
private:
    SocketPortable* sp;
    string nombre;
public:
    Credencial(){
        sp = new SocketPortable();
    }
    ~Credencial(){
        delete sp;
    }

    SocketPortable* dameSocket() {
        return sp;
    }
    void fijaNombre( string nombre ) {
        this->nombre = nombre;
    }
    string dameNombre() {
        return nombre;
    }
};

#endif // CREDENCIAL_H_INCLUDED
