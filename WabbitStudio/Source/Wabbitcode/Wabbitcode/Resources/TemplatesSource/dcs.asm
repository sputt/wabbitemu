#include "ti83plus.inc"
#include "ion.inc"

.org progstart-2
.db t2ByteTok, tAsmCmp
Init:
	xor d
	ret
	jr Start
	
	.dw Description			;or .dw $0000 if you don't have a description
	.db $05,$00			;always this string
	.dw Icon			;or .dw $0000 if you don't have an icon
	.dw ALE				;usually .dw $0000 if you don't have or know what an ALE is
Start:
	;Your code goes here


Description:
	.db "Description",0	;can be omitted if .dw Description is .dw 0000 above
#ifdef USE_ICON
Icon:				;a 16x16 icon (can be omitted if .dw Icon is .dw 0000 above)
.option BM_MIN_W = 16
#include "icon.bmp"
#else
#define Icon 0
#endif
#ifdef USE_ALE
ALE:					;must be omitted if .dw ALE is .dw 0000 above
	.db "ZALE",0,0,0,0 ;always eight bytes, use
	.db "ZLALE",0,0,0 ;zeros for extra bytes
	.db $FF ;put after last ALE
#else
#define ALE 0
#endif