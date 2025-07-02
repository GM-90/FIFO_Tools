#ifndef _ASYNC_SERIAL_
#define _ASYNC_SERIAL_

#include "STR_FIFO.h"
#include "Async_Buffer.h"
#include <Arduino.h>

//Tamanio del Buffer donde se acumulan los datos recibidos
// del puerto Serie a la espera de ser extraidos los mensajes enteros
// y colocados en la FIFO.
#define READ_BUFFER_SIZE    200

template<int fifo_size>
class ASYNC_SERIAL : public ASYNC_BUFFER <fifo_size> {
    public:
    ASYNC_SERIAL(HardwareSerial & _serial) :
        serial(_serial),
        ASYNC_BUFFER<fifo_size>()
        { }

    void set_baudrate(unsigned long baudrate);

    void Read_Port();

    private:
    HardwareSerial & serial;

};


template<int fifo_size>
void ASYNC_SERIAL<fifo_size>::set_baudrate(unsigned long baudrate){
    serial.begin(baudrate);
}


template<int fifo_size>
void ASYNC_SERIAL<fifo_size>::Read_Port( ){
    int byte_to_read = serial.available();
    char buffer[32];
    int i=0;
    while( byte_to_read>0 && (i<(sizeof(buffer)-1)) ){
        uint8_t c = serial.read();
        byte_to_read--;
        if( isAscii(c) ){
            buffer[i] = c ;
            i++ ;
        }
    }
    buffer[i] = '\0' ;
    ASYNC_BUFFER<fifo_size>::Update_Buffer(buffer);
}


#endif