#include "ion.inc"
#include "ti83plus.inc"
#include "var.inc"
#include "z80ext.inc"

.org progstart-2
.db t2ByteTok, tAsmCmp
	ret
	jr nc,Start
.db "program name",0		;This is the name Ion displays

Start:
;Your code goes here