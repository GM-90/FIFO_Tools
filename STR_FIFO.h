#ifndef _STR_FIFO_
#define _STR_FIFO_

#include "SimpleFIFO.h"
#include <string.h>


template <int fifo_size>
class STRING_FIFO {
	public:

    //Carga el mensaje en la FIFO, finalizado por el caracter nulo.
	void FIFO_Write(const char * nuevo_mensaje);

	//Devuelve true si se retira un mensaje con exito
	bool FIFO_Read(char * new_msj, size_t new_msj_tam);

	//devuelve true si hay mensajes para retirar
	bool available();

    //vaciar la FIFO
    void flush(){
        SF.flush();
    }

	protected:
	SimpleFIFO<char,fifo_size>SF;

};


template <int fifo_size>
void STRING_FIFO<fifo_size>::FIFO_Write(const char * nuevo_mensaje){
    size_t tam_mensaje = strlen(nuevo_mensaje) ;
    for(int i=0 ; i<tam_mensaje; i++ ){
        if( !SF.enqueue(nuevo_mensaje[i]) )
            //Por seguridad, si la cola FIFO se llena se vacia el contenido.
            SF.flush();
    }
    //al final (por la fuerza), siempre agrego el caracter nulo.
    while(!SF.enqueue('\0'))  SF.flush();
}


template <int fifo_size>
bool STRING_FIFO<fifo_size>::FIFO_Read(char * new_msj, size_t new_msj_tam){
    if( SF.count() > 0 ){
        for( int i=0 ; SF.count()>0 ; i++ ){
            char c = SF.dequeue();
            if( i < (new_msj_tam-1) ){
                new_msj[i] = c ;
            }
            if( c =='\0' )    break;
        }
        return true;
    }
    return false;
}

template <int fifo_size>
bool STRING_FIFO<fifo_size>::available(){
    return (SF.count() > 0) ;
}



#endif