.ORIG x3000
  AND R0, R0, #0    ; Clear R0
  LD R0, NUM        ; load our number into R0
  LD R2, ASCII      ; load the ascii offset into R2
  ADD R0, R0, R2    
  OUT
HALT                ; Trap x25

NUM   .fill  x02    ; Our Number to print
ASCII .fill  x30    ; Our ASCII offset
.END