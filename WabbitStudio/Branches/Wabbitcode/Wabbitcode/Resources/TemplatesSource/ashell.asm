#include "ti83asm.inc"
	nop			;these 2 lines identify the program
	jr      Start		;as AShell-compatable
	.dw     $0000		;Version of table
	.dw     Description	;Points to the program description
	.dw     Icon		;Points to an 8x8 icon
	.dw     $0000		;For future use ( *Libraries*? :)
Start:


Icon:
.option BM_MIN_W = 16
#include "icon.bmp"