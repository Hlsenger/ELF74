#ifndef _ELEVADOR_DEFINES_H
#define _ELEVADOR_DEFINES_H


#define RECIEVE_FLAG 0x1
#define SEND_FLAG  0x2



//Defines relacionados ao Threadx
#define THREAD_STACK_SIZE         512
#define BYTE_POOL_SIZE     2*8192
#define QUEUE_SIZE      128





#define CHAR_BUFFER_SIZE 128


//Parametros do elevador(todos em ticks)


#define TEMPO_PARADO 200 //Tempo que o elvador para em cada andar
#define NUM_ANDARES 16


#endif