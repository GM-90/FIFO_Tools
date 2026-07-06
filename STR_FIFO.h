#ifndef _STR_FIFO_
#define _STR_FIFO_

#include "SimpleFIFO.h"
#include <string.h>


template <int fifo_size>
class STRING_FIFO {
	public:

    //Carga el mensaje en la FIFO, finalizado por el caracter nulo.
	void FIFO_Write(const char * nuevo_mensaje);

    bool push_char(char new_char);
    void Close_string();

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

    unsigned int num_msj=0;

};


template <int fifo_size>
void STRING_FIFO<fifo_size>::FIFO_Write(const char * nuevo_mensaje){
    size_t tam_mensaje = strlen(nuevo_mensaje) ;
    for (int i=0 ; i<tam_mensaje; i++ ){
        push_char(nuevo_mensaje[i]);
    }
    //al final (por la fuerza), siempre agrego el caracter nulo.
    Close_string();
}

template <int fifo_size>
bool STRING_FIFO<fifo_size>::push_char(char new_char){
    if ( !SF.enqueue(new_char) ) {
        //Por seguridad, si la cola FIFO se llena se vacia el contenido.
        SF.flush();
        num_msj=0;
        return false;
    }
    return true;
}

template <int fifo_size>
void STRING_FIFO<fifo_size>::Close_string(){
    while (!SF.enqueue('\0'))  SF.flush();
    num_msj++;
}

template <int fifo_size>
bool STRING_FIFO<fifo_size>::FIFO_Read(char * new_msj, size_t new_msj_tam){
    if( SF.count() > 0 ){
        int i ;
        for( i=0 ; SF.count()>0 ; ){
            char c = SF.dequeue();
            if( i < (new_msj_tam-1) ){
                new_msj[i] = c ;
                i++ ;       //Solo aqui aumento el indice
            }
            if( c =='\0' )    break;
        }

        new_msj[i] = '\0' ;     //esto solo es necesario en caso de que no salga por el BREAK
        
        if(num_msj>0)  num_msj--;

        return true;
    }
    return false;
}

template <int fifo_size>
bool STRING_FIFO<fifo_size>::available(){
    return (num_msj>0) ;
}



#endif