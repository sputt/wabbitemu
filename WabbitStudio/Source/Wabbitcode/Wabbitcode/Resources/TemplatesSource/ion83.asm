#include "ti83asm.inc"
#include "ion.inc"

.org progstart
	ret
	jr nc,Start
.db "program name",0		;This is the name Ion displays

Start:
;Your code goes here