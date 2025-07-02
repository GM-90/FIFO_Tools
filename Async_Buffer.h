#ifndef _ASYNC_BUFFER_
#define _ASYNC_BUFFER_

#include "STR_FIFO.h"
#include <Arduino.h>

//Tamanio del Buffer donde se acumulan los datos recibidos
// del puerto Serie a la espera de ser extraidos los mensajes enteros
// y colocados en la FIFO.
#define READ_BUFFER_SIZE    200

template<int fifo_size>
class ASYNC_BUFFER{

    public:
    ASYNC_BUFFER()
    {   
        *BUFFER_Rx='\0';
    }

    void Update_Buffer(const char * new_msg);

    //Indica la precencia de mensajes en la FIFO esperando a ser retirados.
    bool Mensajes_Pendientes();

    //Indica la precencia de datos recibidos todavia a la espera de ser procesados.
    bool Datos_Pendientes();

    void Retirar_Mensaje(char * new_msj, size_t new_msj_tam);

    private:
    char BUFFER_Rx[READ_BUFFER_SIZE];
    STRING_FIFO<fifo_size> FI;
    unsigned int millis_BUFFER_RX_last_update=0;
    bool datos_pendientes=false;

};


template<int fifo_size>
void ASYNC_BUFFER<fifo_size>::Update_Buffer(const char * new_msg){
    size_t new_msg_len = strlen(new_msg);
    if( new_msg_len>0 ){
        if( new_msg_len>=(sizeof(BUFFER_Rx)-strlen(BUFFER_Rx)-1) ){
            //Los datos nuevos no entran en el buffer.
            // Se reinicia el buffer para adquirir nuevos datos.
            // Los datos antiguos sin procesar se pierden.
            *BUFFER_Rx='\0';
        }
        strncat(BUFFER_Rx,new_msg,new_msg_len);
        millis_BUFFER_RX_last_update=millis();
        datos_pendientes=true;
    }

    if( strlen(BUFFER_Rx)!=0 ){
        if( (millis()-millis_BUFFER_RX_last_update)>50/*ms*/ ){
            //pasaron 50ms y los datos acumulados en BUFFER_Rx no se retiran
            // por falta de la trama '\r\n'.
            // Si hay espacio se agregan caracteres de terminacion, sino se elimina.
            if( sizeof(BUFFER_Rx) > (strlen(BUFFER_Rx)+2) ){
                strcat(BUFFER_Rx,"\r\n");
            }
            else{
                *BUFFER_Rx='\0';
            }
        }
        
        //Recursivamente, busco la trama '\r\n' para detectar fin de un mensaje entero
        char * fin_msj;
        fin_msj = strstr(BUFFER_Rx,"\r\n");
        while(fin_msj!=NULL){
            millis_BUFFER_RX_last_update=millis();
            //extraigo al mensaje del buffer de entrada.
            char nuevo_mensaje_entero[100]={'\0'};
            int  nuevo_mensaje_entero_tam = (fin_msj+2-BUFFER_Rx);
            //limito el tamanio del nuevo mensaje al tamanio del array
            if( nuevo_mensaje_entero_tam >= sizeof(nuevo_mensaje_entero) ){
                nuevo_mensaje_entero_tam = sizeof(nuevo_mensaje_entero) - 1 ;    //se resta '1' por el caracter nulo.
            }
            strncat(nuevo_mensaje_entero,BUFFER_Rx,nuevo_mensaje_entero_tam);

            FI.FIFO_Write(nuevo_mensaje_entero);

            //Desplazo el mensaje restante al origen del array
            if( BUFFER_Rx[nuevo_mensaje_entero_tam]=='\0' ){
                //El mensaje analizado corresponde al ultimo del BUFFER_Rx.
                *BUFFER_Rx='\0';
                break;      //salgo del lazo, no es necesario buscar otra trama "\r\n" porque no hay.
            }
            for(int i=0;   BUFFER_Rx[nuevo_mensaje_entero_tam + i]!=0   ;i++){
                BUFFER_Rx[i] = BUFFER_Rx[nuevo_mensaje_entero_tam + i] ;
                BUFFER_Rx[i+1] = '\0' ;
            }
            //busco nuevamente la trama "\r\n"
            fin_msj = strstr(BUFFER_Rx,"\r\n");
        }
        
    } else {
        //no hay mas datos en BUFFER_RX
        datos_pendientes=false;
    }
}

template<int fifo_size>
bool ASYNC_BUFFER<fifo_size>::Mensajes_Pendientes(){
    return FI.available();
}

template<int fifo_size>
bool ASYNC_BUFFER<fifo_size>::Datos_Pendientes(){
    return datos_pendientes;
}

template<int fifo_size>
void ASYNC_BUFFER<fifo_size>::Retirar_Mensaje(char * new_msj, size_t new_msj_tam){
    FI.FIFO_Read(new_msj,new_msj_tam);
}


#endif