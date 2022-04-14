  PUBLIC eightBitHistogram

  SECTION .text:CODE(2)
  THUMB
          
;Param1(R0): Width - Image Width
;Param2(R1): Height - Image Heioght
;Param3(R2): *p_image - Pointer to image array 
;Param4(R3) : *p_histogram - Pointer to the histogram array

;Return(R0): numPixels - Number of pixels processed
eightBitHistogram:
  push {r4-r5}

  mul r0, r1 ; r0 = width*height;
  mov r1, r0
ebh_loop_1:
  subs r1,#1
  bmi ebh_loop1_end
  ldrb r4,[r2,r1] ;Reads image value in position image[r1]
  ldrh r5,[r3,r4,lsl #1]; Reads histogram value in position histogram[r2]
  add r5,#1
  strh r5,[r3,r4, lsl #1]
  b ebh_loop_1
ebh_loop1_end:  
  pop {r4-r5}
  bx lr
  

  END
