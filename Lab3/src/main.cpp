#include <cstdint>
#include <iostream>
#include "images.h"

using namespace std;
extern "C" uint16_t EightBitHistogram(uint16_t width, uint16_t height, const uint8_t * p_image, uint16_t * p_histogram);

int main()
{
  uint16_t p_histogram[256] = {0};
  if(EightBitHistogram(width1, height1, p_start_image1, p_histogram) == 0){
    cout << "Tamanho incompativel" << endl;
  }
  else{
    cout << "X,Y" << endl;
    for(int i=0; i<256; i++){
      cout << i << "," <<p_histogram[i] << endl;
    }
  }
  return 0;
}
