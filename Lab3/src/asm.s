; arm-asm
    PUBLIC EightBitHistogram

    SECTION .text : CODE (2)
    THUMB

EightBitHistogram
    PUSH {R4-R6, LR}
    MUL R4, R0, R1
    
    ;verifies if size is valid
    CMP R4, #65536
    IT GT
    BGT InvalidSize
    MOV R0, #1
    
    MOV R5, #0
Loop
    LDRB R6, [R2, R5]
    PUSH {R3}
    LSL R6, #1
    ADD R3, R6
    LDRB R6, [R3]
    ADD R6, #1
    STRB R6, [R3]
    POP {R3}
    
    ADD R5, #1
    CMP R5, R4
    IT NE
    BNE Loop
    
    POP {R4-R6, PC}
    
; in case size is not valid (too large), returns 0
InvalidSize
    MOV R0, #0
    POP {R4-R6, PC}
    
    END