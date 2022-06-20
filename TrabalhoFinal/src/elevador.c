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
  elevador->statusPorta = porta_aberta;
  
  memset(elevador->andaresPressionados,nao_pressionado,NUM_ANDARES); 
}


void elevador_reset(Elevador *elevador){
   elevador->destinoAndar = 0;
  elevador->ultimoAndar = 0;
  elevador->direcao = parado;
   memset(elevador->andaresPressionados,nao_pressionado,NUM_ANDARES); 
}





void elevador_processa_msg(Elevador *elevador, CHAR *msg){
  //Botao externo
  if(msg[0] == 'E'){
    CHAR andarChr[3] = {msg[1],msg[2],'\0'};      
    UINT andar = atoi(andarChr);
    
    
    if(msg[3]  == 's'){
      elevador->andaresPressionados[andar] = externo_sobe;
    }
    else{
      elevador->andaresPressionados[andar] = externo_desce;
    }     
  }
  //Botao interno
  else if(msg[0] == 'I'){
    UINT andar = msg[1]-97; //Converte ascii numerico para inteiro
    
    elevador->andaresPressionados[andar] = interno;
  }
  
  //Status do elevador
  else{
    //Porta aberta ou fechada
    if(msg[0] == 'A'){
      elevador->statusPorta = porta_aberta;
    }
    else if(msg[0] == 'F'){
      elevador->statusPorta = porta_fechada;
    }
    else{
      elevador->ultimoAndar= atoi(msg);
    }
  }
}

void elevador_update(Elevador *elevador){
  if(elevador->direcao == parado){
    for (UINT i=0;i<NUM_ANDARES;i++){
      if(elevador->andaresPressionados[i] !=  0){
        elevador->destinoAndar = i;
      }
    }
    
    if(elevador->destinoAndar > elevador->ultimoAndar){
      elevador->direcao = subindo;
      elevador_fecha(elevador);
      tx_thread_sleep(50);
      elevador_sobe(elevador);
    }else if(elevador->destinoAndar < elevador->ultimoAndar){
      elevador->direcao = descendo;
      elevador_fecha(elevador);
      tx_thread_sleep(50);
      elevador_desce(elevador);
    }
  }
  else if(elevador->direcao == subindo){
    if(elevador->destinoAndar == elevador->ultimoAndar){
      elevador_para(elevador);
      elevador_abre(elevador);
      elevador->andaresPressionados[elevador->destinoAndar] =  0;
      tx_thread_sleep(TEMPO_PARADO);
      
      
      //Checa se existe algum andar acima deste pressionado
      UINT proximo_andar = NUM_ANDARES;
      for (UINT i=0;i<NUM_ANDARES;i++){
        
        
        if((elevador->andaresPressionados[i] == externo_sobe || elevador->andaresPressionados[i] == interno) && i > elevador->destinoAndar){
          if(i < proximo_andar){
            proximo_andar = i;
          }
        }
      }
      
      if(proximo_andar < NUM_ANDARES){
        elevador->destinoAndar = proximo_andar;
        elevador_fecha(elevador);
        tx_thread_sleep(50);
        elevador_sobe(elevador);
      }
      
      if(elevador->destinoAndar == elevador->ultimoAndar){
        elevador->direcao = parado;
      }    
    }
    
  }
  else if(elevador->direcao == descendo){
    if(elevador->destinoAndar == elevador->ultimoAndar){
      elevador_para(elevador);
      elevador_abre(elevador);
      elevador->andaresPressionados[elevador->destinoAndar] =  0;
      tx_thread_sleep(TEMPO_PARADO);
      
      
      
      //Checa se existe algum andar abaixo deste pressionado
      INT proximo_andar = -1;
      for (UINT i=0;i<NUM_ANDARES;i++){
        
        
        if((elevador->andaresPressionados[i] == externo_desce || elevador->andaresPressionados[i] == interno) && i < elevador->destinoAndar){
          if(i > proximo_andar){
            proximo_andar = i;
          }
        }
      }
      
      if(proximo_andar >= 0){
        elevador->destinoAndar = proximo_andar;
        elevador_fecha(elevador);
        tx_thread_sleep(50);
        elevador_desce(elevador);
      }
      
      if(elevador->destinoAndar == elevador->ultimoAndar){
        elevador->direcao = parado;
      }    
    }
  }
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
