#include "elevador.h"

#include "stdio.h"

void elevador_inicializa(Elevador *elevador, CHAR id,TX_QUEUE *queue_in, TX_QUEUE *queue_out,TX_EVENT_FLAGS_GROUP *uart_flags){
  elevador->id = id;
  elevador->queue_in = queue_in;
  elevador->queue_out = queue_out;
  elevador->uart_flags = uart_flags;
  
  elevador->destinoAndar = 0;
  elevador->ultimoAndar = 0;
  elevador->direcao = parado;
  memset(elevador->andaresPressionados,0,16);
  
  
}



void elevador_sobe(Elevador *elevador){
  CHAR out[3];
  out[0] = elevador->id;
  out[1] = 's';
  out[2] = '\x0D';
  

  
  tx_queue_send(elevador->queue_out, out, TX_WAIT_FOREVER);
  tx_event_flags_set(elevador->uart_flags, SEND_FLAG, TX_OR);
}

void elevador_desce(Elevador *elevador){
  CHAR out[3];
  out[0] = elevador->id;
  out[1] = 'd';
  out[2] = '\x0D';
  
  
  
  tx_queue_send(elevador->queue_out, out, TX_WAIT_FOREVER);
  tx_event_flags_set(elevador->uart_flags, SEND_FLAG, TX_OR);
}

void elevador_para(Elevador *elevador){
  CHAR out[3];
  out[0] = elevador->id;
  out[1] = 'p';
  out[2] = '\x0D';
  
  
  tx_queue_send(elevador->queue_out, out, TX_WAIT_FOREVER);
  tx_event_flags_set(elevador->uart_flags, SEND_FLAG, TX_OR);
}


void elevador_abre(Elevador *elevador){
  CHAR out[3];
  out[0] = elevador->id;
  out[1] = 'a';
  out[2] = '\x0D';
  
  tx_queue_send(elevador->queue_out, out, TX_WAIT_FOREVER);
  tx_event_flags_set(elevador->uart_flags, SEND_FLAG, TX_OR);
}

void elevador_fecha(Elevador *elevador){
  CHAR out[3];
  out[0] = elevador->id;
  out[1] = 'f';
  out[2] = '\x0D';
  
  tx_queue_send(elevador->queue_out, out, TX_WAIT_FOREVER);
  tx_event_flags_set(elevador->uart_flags, SEND_FLAG, TX_OR);
}
