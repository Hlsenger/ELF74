#ifndef _CHAR_BUFFER_H_
#define _CHAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#include "tx_api.h"

#include "defines.h"

//Buffer circular de chars usado no UART
typedef struct CharBuffer{
  uint32_t head;
  uint32_t tail;
  uint8_t data[CHAR_BUFFER_SIZE];
} CharBuffer; 


void charBufferAdd(CharBuffer *buffer,CHAR data);
bool charBufferGet(CharBuffer *buffer, CHAR *data);
bool charBufferIsEmpty(CharBuffer *buffer);

void charBufferSkip(CharBuffer *buffer);


#endif