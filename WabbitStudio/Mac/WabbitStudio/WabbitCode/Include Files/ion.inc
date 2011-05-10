; The Ion Include File by Dan Englender
; Altered James Montelongo to be used with ti83plus.inc
; ti's inc is more complete

;----> Safe Memory Areas
; saferam1 = 768 bytes (apdram)
; saferam2 = 531 bytes (statram)
; saferam3 = 128 bytes (textmem)
; saferam4 = 66 bytes (ops)
; saferam5 = 10 bytes (imathptrs)

#define bcall(xxxx)	rst 28h \ .dw xxxx

#define	bcallz(xxxx)	jr nz,$+5 \ rst 28h \ .dw xxxx
#define	bcallnz(xxxx)	jr	z,$+5 \ rst 28h \ .dw xxxx
#define	bcallc(xxxx)	jr	nc,$+5 \ rst 28h \ .dw xxxx
#define	bcallnc(xxxx)	jr	c,$+5	\ rst 28h \ .dw xxxx
#define bjump(xxxx)	call 50h \ .dw xxxx
 .addinstr B_CALL *       EF   3 NOP 1

;Not defined in ti83plus.inc
libstart		=$0004
cmdshad		=$966E
progstart		=$9D95
saferam1		=$86EC	;saveSScreen=768
saferam2		=$8A3A	;statVars=531
saferam3		=$8508	;textShadow=128
saferam4		=$8478	;OPs=66
saferam5		=$84D3	;iMathPtrs=10
saferamp		=$9872	;appBackUpScreen=768
saferamp2		=$8251	;bootTemp=52
gbuf			=$9340
grbuf			=$9340
graph_mem		=$9340
_copygbuf		=$486A
_cleargbuf		=$4BD0
_clrlcdf		=$4540
_clrscrf		=$4546
_chksysvar		=$42F1
sram			=saferam1
apd_buf		=saferam1
apdbuf		=saferam1
apdram		=saferam1
statram		=saferam2
statvar		=saferam2
text_mem		=saferam3
textmem		=saferam3
;-----> Ion Functions
lVectors		=cmdshad+80
ionVersion		=lVectors
ionRandom		=lVectors+3
ionPutSprite	=lVectors+6
ionLargeSprite	=lVectors+9
ionGetPixel		=lVectors+12
ionFastCopy		=lVectors+15
ionDetect		=lVectors+18
ionDecompress	=lVectors+21

fastcopy =ionFastCopy



