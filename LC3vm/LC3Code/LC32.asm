.ORIG x3000
  AND R0, R0, #0    ; Clear R0
  AND R1, R1, #0    ; Clear R1
  AND R3, R3, #0    ; Clear R3
  LEA R0, NUM       ; pointer [mem]NUM
  ADD R1, R1, R0    ; Store the pointer address of R0 into R1
  LD R2, ASCII      ; load the ascii offset into R2

FOR_LOOP
  LDR R4, R1, #0    ; load the contents of mem address of R1 into R4
  BRz END_LOOP
  ADD R4, R4, R2    ; Add our number to the ASCII offset
  STR R4, R1, #0    ; Store the new value in R4 into [mem] address R1
  ADD R1, R1, #1    ; move our memory pointer down one
  BRnzp FOR_LOOP    ; loop again until we get an x00 char
END_LOOP

  PUTs              ; print our string starting from [mem]address in R0
HALT                ; Trap x25

ASCII .fill  x30    ; Our ASCII offset
NUM   .fill  x01    ; Our Number to print
      .fill  x02     
      .fill  x03
      .fill  x04
.END