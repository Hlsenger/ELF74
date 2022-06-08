#include "charbuffer.h"

void charBufferAdd(CharBuffer *buffer,CHAR data){
  buffer->data[buffer->head] = data;
  buffer->head = (buffer->head + 1) % CHAR_BUFFER_SIZE;
}


bool charBufferGet(CharBuffer *buffer, CHAR *data){
  if(buffer->tail == buffer->head){
    return false;
  }
  *data = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % CHAR_BUFFER_SIZE;
  return true;
}

bool charBufferIsEmpty(CharBuffer *buffer){
  if(buffer->tail == buffer->head){
    return true;
  }
  return false;
}
