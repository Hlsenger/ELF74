#include <stdio.h>
#include <stdint.h>

using namespace std;

extern "C" int eightBitHistogram(uint16_t, uint16_t,uint8_t *,uint16_t *);
#define WIDTH0 4
#define HEIGTH0 3 


const uint8_t image0[HEIGTH0][WIDTH0] = {
  { 20, 16, 16, 18}, 
  {255, 255, 0, 0}, 
  { 32, 32, 32, 32}
};


uint16_t histogramC(uint16_t width, uint16_t height, uint8_t *p_image, uint16_t *p_histogram){
  uint16_t pixelCount = width*height;
  
  for(uint16_t i=0;i<pixelCount;i++){
    p_histogram[*(p_image+i)] +=1;
  }
  
  return pixelCount;
}




void main(void){
  
  uint16_t histogram[256] = {0};
  
  
  uint16_t t;
  //t = histogramC(WIDTH0,HEIGTH0,(uint8_t*) image0, histogram);
  t = eightBitHistogram(WIDTH0,HEIGTH0,(uint8_t*) image0,histogram);
  for(uint16_t i=0;i<256;i++){
    printf("%d",histogram[i]); 
    if(i < 255){
      printf(",");
    }
  }
  printf("\n");
  
  while(1){
  }
}
