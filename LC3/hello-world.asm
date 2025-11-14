.orig x3000

ADD R0, R0, #0  ; Clear R0
LEA R0, MSG
PUTS
HALT

MSG .stringz "hello world!"

.end
