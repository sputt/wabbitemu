vRandom		.equ   $FE72
vFastCopy	.equ   $FE75

.org   $9329
.db    $E7,"9_[V?",0  ; $E7, $39, $5F, $5B, $56, $3F, $00
	jr    nc,start
.db    "description",0
#ifdef USE_ICON
.option BM_MIN_W = 16
#include "icon.bmp"
#endif
	;your code goes here
	


;use this routine to quit
Quit:
	res   4,(iy+9)				;reset the on key flag in case the user pressed on at some point
	call  _clrScrnFull			;clear the done message
	jp    _homeUp				;exit
