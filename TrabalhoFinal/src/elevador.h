#ifndef _ELEVADOR_H
#define _ELEVADOR_H



#include "tx_api.h"
#include "defines.h"


enum StatusBotao{nao_pressionado,externo_sobe,externo_desce,interno};

enum Direcao{parado, subindo, descendo};

enum porta{porta_aberta, porta_fechada};

typedef struct Elevador{
  CHAR id;
  enum Direcao direcao;
  UINT ultimoAndar;
  UINT destinoAndar;
  CHAR statusPorta;
  
  TX_QUEUE *queue_in;
  TX_QUEUE *queue_out;
  TX_EVENT_FLAGS_GROUP *uart_flags;
  
  CHAR andaresPressionados[NUM_ANDARES];
  
} Elevador;

void elevador_inicializa(Elevador *elevador, CHAR id,TX_QUEUE *queue_in, TX_QUEUE *queue_out, TX_EVENT_FLAGS_GROUP *uart_flags);
void elevador_reset(Elevador *elevador);

void elevador_processa_msg(Elevador *elevador, CHAR *msg);
void elevador_update(Elevador *elevador);



void elevador_sobe(Elevador *elevador);
void elevador_desce(Elevador *elevador);
void elevador_para(Elevador *elevador);
void elevador_abre(Elevador *elevador);
void elevador_fecha(Elevador *elevador);


#endif
