#include "mirage.inc"
#include "ti83plus.inc"
#include "var.inc"
#include "z80ext.inc"

.org progstart-2
.db t2ByteTok, tAsmCmp
	ret				;So TIOS wont run the program
.db 1					;Identifier as MirageOS program
.option BM_MIN_W = 16
#include "icon.bmp"			;15x15 button
.db	"Description",0			;Zero terminated description
Start:
;Program code starts here