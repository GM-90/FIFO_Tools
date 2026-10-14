#ifndef _SERIAL_FIFO_
#define _SERIAL_FIFO_


#include <STR_FIFO.h>


template<int fifo_size>
class SERIAL_FIFO {
	
	public:
	SERIAL_FIFO(HardwareSerial & _serial) : 
	serial(_serial) {} ;
	
	void Read_Port();

    //Indica la precencia de mensajes en la FIFO esperando a ser retirados.
    bool Mensajes_Pendientes();

    //Indica la precencia de datos recibidos todavia a la espera de ser procesados.
    //bool Datos_Pendientes();

    void Retirar_Mensaje(char * new_msj, size_t new_msj_tam);
	
	
	private:
	HardwareSerial & serial;
	
	STRING_FIFO<fifo_size> Rx_Data;
    unsigned int TS_last_char=0;
    bool incoming_data=false;
};


template<int fifo_size>
void SERIAL_FIFO<fifo_size>::Read_Port( ){
	//recibir mensajes entrantes
    if (serial.available()) {
        do {
            char c = serial.read();
            incoming_data=true;
            Rx_Data.push_char(c);
            if (c=='\n') {
                Rx_Data.Close_string();
                incoming_data=false;
            }
        } while (serial.available());
        TS_last_char = millis() ;
    }
    if (incoming_data) {
        if ((millis()-TS_last_char)>=50){
            Rx_Data.Close_string();
            incoming_data=false;
        } 
    }
	
}

template<int fifo_size>
bool SERIAL_FIFO<fifo_size>::Mensajes_Pendientes(){
    return Rx_Data.available();
}

template<int fifo_size>
void SERIAL_FIFO<fifo_size>::Retirar_Mensaje(char * new_msj, size_t new_msj_tam){
    Rx_Data.FIFO_Read(new_msj,new_msj_tam);
}


#endif