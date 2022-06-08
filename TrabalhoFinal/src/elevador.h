#ifndef _ELEVADOR_H
#define _ELEVADOR_H



#include "tx_api.h"
#include "defines.h"


enum Direcao{parado, subindo, descendo};

typedef struct Elevador{
  CHAR id;
  enum Direcao direcao;
  UINT ultimoAndar;
  UINT destinoAndar;
  TX_QUEUE *queue_in;
  TX_QUEUE *queue_out;
  TX_EVENT_FLAGS_GROUP *uart_flags;
  
  CHAR andaresPressionados[16];
  
} Elevador;

void elevador_inicializa(Elevador *elevador, CHAR id,TX_QUEUE *queue_in, TX_QUEUE *queue_out, TX_EVENT_FLAGS_GROUP *uart_flags);

void elevador_sobe(Elevador *elevador);
void elevador_desce(Elevador *elevador);
void elevador_para(Elevador *elevador);
void elevador_abre(Elevador *elevador);
void elevador_fecha(Elevador *elevador);

#endif
