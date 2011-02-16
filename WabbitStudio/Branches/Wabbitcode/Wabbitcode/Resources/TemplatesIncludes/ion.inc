; The Ion Include File by Dan Englender
.nolist
;----> Safe Memory Areas
; saferam1 = 768 bytes (apdram)
; saferam2 = 531 bytes (statram)
; saferam3 = 128 bytes (textmem)
; saferam4 = 66 bytes (ops)
; saferam5 = 10 bytes (imathptrs)

#ifdef	TI83
#define bcall(xxxx)	call xxxx
#define	bcallz(xxxx)	call	z,xxxx
#define	bcallnz(xxxx)	call	nz,xxxx
#define	bcallc(xxxx)	call	c,xxxx
#define	bcallnc(xxxx)	call	nc,xxxx
#define bjump(xxxx)	jp xxxx
 .addinstr BCALL *       CD   3 NOP 1
_htimesl	=$4382
usermemoff	=$8565
progstart	=9327h
libstart	=$0002
saferam1	=8265h
saferam2	=858Fh
saferam3	=80C9h
saferam4	=8039h
_zeroop1	=428Eh
_getcsc		=$4014
_homeup		=$4775
_newline	=$473D
_darkline	=$4AE0
_indicatorOff	=$4795
_indicatorOn	=$4791
_cleargbuf	=515Bh
_copygbuf	=4B9Ch
_clrlcdf	=4755h
_cleartextshad	=4765h
_clrscr		=4759h
_clrscrf	=475Dh
_cline		=4A84h
_iline		=4AE4h
_ipoint		=4AE8h
_ldhlind	=4000h
_divhlby10	=4008h
_cphlde		=4004h
_disphl		=4709h
_divhlbya	=400Ch
_hltimes9	=4146h
_vputmap	=477Dh
_vputs		=4781h
_vputsn		=4785h
_putmap		=4701h
_puts		=470Dh
_putps		=4715h
_parseinp       =$4E8C
_putc		=4705h
_getkey		=4CFEh
_getk		=4A18h
_rcly		=4EE0h
_rclx		=4EE8h
_memfree	=441Ah
_chksysvar	=442Ah
_findsym	=442Eh
_creategdb	=4486h
_createprog	=448Ah
_createreal	=4452h
_createstring	=4472h
_delvar		=44AAh
_insertmem	=4432h
_deletemem	=44B2h
_lcdbusy	=4066h
ptemp		=9317h
op1		=8039h
op2		=8044h
op3		=804Fh
op4		=805Ah
op5		=8065h
op6		=8070h
imathptr1	=8094h
imathptr2	=8096h
imathptr3	=8098h
imathptr4	=809Ah
imathptr5	=809Ch
progptr		=9319h
cmdshad		=9157h
plotsscreen	=8E29h
symtable	=$FE6E
currow		=800Ch
curcol		=800Dh
pencol		=8252h
penrow		=8253h
curgy		=886Dh
curgx		=886Eh
_op1toop2	=$418E
_op1toop3	=$417E
_op1toop4	=$416E
_op1toop5	=$41BE
_op1toop6	=$41BA
_op2toop1	=$41C2
_op2toop3	=$41F6
_op2toop4	=$4172
_op2toop5	=$41B2
_op2toop6	=$41B6
_op3toop1	=$41A2
_op3toop2	=$417A
_op3toop4	=$416A
_op3toop5	=$41AE
_op4toop1	=$419A
_op4toop2	=$4176
_op4toop3	=$41FA
_op4toop5	=$41AA
_op4toop6	=$4202
_op5toop1	=$419E
_op5toop2	=$4182
_op5toop3	=$41FE
_op5toop4	=$418A
_op5toop6	=$4186
_op6toop1	=$4196
_op6toop2	=$4192
_op6toop5	=$41A6
_op1exop2	=$4306
_op1exop3	=$42FE
_op1exop4	=$4302
_op1exop5	=$42EE
_op1exop6	=$42F2
_op2exop4	=$42F6
_op2exop5	=$42FA
_op2exop6	=$42E6
_op5exop6	=$42EA
_PUSHREALO1        .EQU           4536h
_PUSHREALO4        .EQU           452Ah
_POPREALO2         .EQU           44FAh
_POPREALO5         .EQU           44EEh
_PUSHREALO2        .EQU           4532h
_PUSHREALO5        .EQU           4526h
_POPREALO3         .EQU           44F6h
_POPREALO6         .EQU           44EAh
_PUSHREALO3        .EQU           452Eh
_PUSHREALO6        .EQU           4522h
_POPREALO1         .EQU           44FEh
_POPREALO4         .EQU           44F2h
_FPMULT            .EQU           40AAh
_FPADD             .EQU           4092h

_OP1SET0           .EQU           4286h

_SIN               .EQU           40F6h
_op1set1	=$4232
_op1set2	=$4242
_op1set3	=$423A
_op1set4	=$4236
_op2set0	=$4282
_op2set1	=$4252
_op2set2	=$4246
_op2set3	=$422E
_op2set4	=$422A
_op2set5	=$4222
_op2set8	=$421E
_op2set60	=$4EA8
_op2setA	=$4226
_op3set0	=$427E
_op3set1	=$421A
_op3set2	=$423E
_op4set0	=$427A
_op4set1	=$4216
_op5set0	=$4276
_pushop1	=$454A
_pushop3	=$4542
_popop1		=$44E6
_popop3		=$44E2
_setxxop1	=$4A74
_setxxop2	=$4A78
_setxxxxop2	=$4A7C
_dispop1a	=$51D4
_mov9toop1	=$4206
contrast	=$8008
#endif
#ifdef	TI83P
#define bcall(xxxx)	rst 28h \ .dw xxxx
#define	bcallz(xxxx)	jr nz,$+5 \ rst 28h \ .dw xxxx
#define	bcallnz(xxxx)	jr	z,$+5 \ rst 28h \ .dw xxxx
#define	bcallc(xxxx)	jr	nc,$+5 \ rst 28h \ .dw xxxx
#define	bcallnc(xxxx)	jr	c,$+5	\ rst 28h \ .dw xxxx
#define bjump(xxxx)	call 50h \ .dw xxxx
 .addinstr BCALL *       EF   3 NOP 1
appBackupScreen	=$9872	; 768 bytes
_ldhlind	=$4009
_htimesl	=$4276
_convop1	=$4AEF
contrast	=8447h
_darkline	=$47DD
_darkpoint	=$47F2
_archUnarch	=$4FD8
_enoughRam	=$42FD
usermemoff	=$89EC
libstart	=$0004
progstart	=9D95h
;saferam1	=86ECh
saferam1	=9872h
saferam2	=858fh
saferam3	=8508h
saferam4	=8478h
saferamp	=9872h
saferamp2	=8251h
_homeup		=$4558
_newline	=$452E
_cleargbuf	=4BD0h
_indicatorOn	=456Dh
_indicatorOff	=4570h
_copygbuf	=486Ah
_clrlcdf	=4540h
_cleartextshad	=454Ch
_clrscr		=4543h
_clrscrf	=4546h
_cline		=4798h
_iline		=47E0h
_ipoint		=47E3h
_ParseInp	=4A9Bh
_divhlby10	=400Fh
_cphlde		=400Ch
_disphl		=4507h
_divhlbya	=4012h
_hltimes9	=40F9h
_vputmap	=455Eh
_vputs		=4561h
_vputsn		=4564h
_putmap		=4501h
_puts		=450Ah
_putps		=4510h
_putc		=4504h
_getkey		=4972h
_getk		=4744h
_getcsc		=4018h
;_RCLY		=4EE0h
;_RCLX		=4EE8h
_memfree	=$42E5
_chksysvar	=42F1h
_findsym	=42F4h
;_CREATEGDB	=4486h
;_CREATEREAL	=4452h
;_CREATESTRNG	=4472h
_createprog	=$4339
_createprotprog	=$4E6D
_delvar		=4351h
_insertmem	=42F7h
_deletemem	=4357h
_lcdbusy	=4051h
op1		=8478h
op2		=8483h
op3		=8499h
op4		=8499h
op5		=84A4h
op6		=84AFh
imathptr1	=84D3h
imathptr2	=84D5h
imathptr3	=84D7h
imathptr4	=84D9h
imathptr5	=84DBh
progptr		=9830h
cmdshad		=966Eh
plotsscreen	=9340h
symtable	=$FE6E
currow		=844Bh
curcol		=844Ch
pencol		=86D7h
penrow		=86D8h
curgy		=8D18h
curgx		=8D19h
_PushRealO1	.equ 43BAh
_PushRealO2	.equ 43B7h
_PushRealO3	.equ 43B4h
_PushRealO4	.equ 43B1h
_PushRealO5	.equ 43AEh
_PushRealO6	.equ 43ABh
_FPAdd	.equ 4072h
_FPDiv	.equ 4099h
_FPMult	.equ 4084h
_OP1ExOP4	.equ 421Ch
_OP1Set0	.equ 41BFh
_OP3SET2	.equ 41A4h
_PopRealO1	=4390h
_PopRealO2	=438Dh
_PopRealO3	=438Ah
_PopRealO4	=4387h
_PopRealO5	=4384h
_PopRealO6	=4381h
_op5exop6	=420Ah
_op5set0	=41B3h
_cpop1op2	=$4111
_op3toop4	=$4114
_op1toop4	=$4117
_op2toop4	=$411A
_op4toop2	=$411D
_op3toop2	=$4120
_op1toop3	=$4123
_op5toop2	=$4126
_op5toop6	=$4129
_op5toop4	=$412C
_op1toop2	=$412F
_op6toop2	=$4132
_op6toop1	=$4135
_op4toop1	=$4138
_op5toop1	=$413B
_op3toop1	=$413E
_op6toop5	=$4141
_op4toop5	=$4144
_op3toop5	=$4147
_op2toop5	=$414A
_op2toop6	=$414D
_op1toop6	=$4150
_op1toop5	=$4153
_op2toop1	=$4156
_op2toop3	=$416E
_op4toop3	=$4171
_op5toop3	=$4174
_op4toop6	=$4177
_OP1Set1	.equ 419Bh
_OP1Set2	.equ 41A7h
_OP1Set3	.equ 41A1h
_OP1Set4	.equ 419Eh
_pushop1	=$43C9
_popop1		=$437E
_arc_unarc	.equ 4FD8h
_sin		=40BDh
_zeroop1	=$41C5
_zeroop2	=$41C8
_zeroop3	=$41CB
_setxxop1	=$478C
_setxxop2	=$478F
_setxxxxop2	=$4792
_dispop1a	=4BF7h
_mov9toop1	=$417A
_delvararc	=$4fC6
ptemp		=982Eh
#endif
grbuf		=plotsscreen
graph_mem	=plotsscreen
gbuf		=plotsscreen
sram		=saferam1
apdram		=saferam1
statram		=saferam2
statvars	=saferam2
vat		=progptr
saferam5	=imathptr1
_grbufcpy	=_copygbuf
_grbufclr	=_cleargbuf
_clrlcdfull     =_clrlcdf
_clrscrnfull	=_clrscrf
_chkfindsym     =_chksysvar
textshadow	=saferam3
savesscreen     =saferam1
;-----> Ion Functions
lVectors	=cmdshad+80
ionVersion	=lVectors
ionRandom	=lVectors+3
ionPutSprite	=lVectors+6
ionLargeSprite	=lVectors+9
ionGetPixel	=lVectors+12
ionFastCopy	=lVectors+15
ionDetect	=lVectors+18
ionDecompress	=lVectors+21


trigflags       .EQU       0           ; IY OFFSET VALUE
trigdeg         .EQU       2
;
plotflags       .EQU       2           ; IY OFFSET VALUE
plotloc         .EQU       1
plotdisp        .EQU       2
;
grfmodeflags    .EQU       2           ; IY OFFSET VALUE
grffuncm        .EQU       4
grfpolarm       .EQU       5
grfparamm       .EQU       6
grfrecurm       .EQU       7
;
graphflags      .EQU       3           ; IY OFFSET VALUE
graphdraw       .EQU       0
graphcursor     .EQU       2
;
grfdbflags      .EQU       4           ; IY OFFSET VALUE
grfdot          .EQU       0
grfsimul        .EQU       1
grfgrid         .EQU       2
grfpolar        .EQU       3
grfnocoord      .EQU       4
grfnoaxis       .EQU       5
grflabel        .EQU       6
;
textflags       .EQU       5           ; IY OFFSET VALUE
textEraseBelow  .EQU       1
textScrolled    .EQU       2
textInverse     .EQU       3
;
onflags         .EQU       9           ; IY OFFSET VALUE
onRunning       .EQU       3
onInterrupt     .EQU       4
;
statflags       .EQU       9           ; IY OFFSET VALUE
statsvalid      .EQU       6
;
fmtflags        .EQU       10          ; IY OFFSET VALUE
fmtExponent     .EQU       0
fmtEng          .EQU       1
;
nummode         .EQU       10          ; IY OFFSET VALUE
FMTREAL         .EQU       5
FMTRECT         .EQU       6
FMTPOLAR        .EQU       7
;
curflags        .EQU       12          ; IY OFFSET VALUE
curAble         .EQU       2
curOn           .EQU       3
curLock         .EQU       4
;
appflags        .EQU       13          ; IY OFFSET VALUE
appTextSave     .EQU       1
appAutoScroll   .EQU       2
;
PLOTFLAG2       .EQU       17          ; IY OFFSET VALUE
EXPR_PARAM      .EQU       3
EXPR_WRITING    .EQU       4
;
indicflags      .EQU       18          ; IY OFFSET VALUE
indicRun        .EQU       0
indicOnly       .EQU       2
;
shiftflags      .EQU       18          ; IY OFFSET VALUE
shift2nd        .EQU       3
shiftAlpha      .EQU       4
shiftALock      .EQU       6
;
tblflags        .EQU       19          ; IY OFFSET VALUE
AutoFill        .EQU       4
AutoCalc        .EQU       5
;
sgrflags        .EQU       20          ; IY OFFSET VALUE
grfSplit        .EQU       0
VertSplit       .EQU       1
WRITE_ON_GRAPH  .EQU       4
textwrite       .EQU       7
;
asm_flag1       .EQU       33          ; IY OFFSET VALUE
asm_flag1_0     .EQU       0
asm_flag1_1     .EQU       1
asm_flag1_2     .EQU       2
asm_flag1_3     .EQU       3
asm_flag1_4     .EQU       4
asm_flag1_5     .EQU       5
asm_flag1_6     .EQU       6
asm_flag1_7     .EQU       7
;
asm_flag2       .EQU       34          ; IY OFFSET VALUE
asm_flag2_0     .EQU       0
asm_flag2_1     .EQU       1
asm_flag2_2     .EQU       2
asm_flag2_3     .EQU       3
asm_flag2_4     .EQU       4
asm_flag2_5     .EQU       5
asm_flag2_6     .EQU       6
asm_flag2_7     .EQU       7
;
asm_flag3       .EQU       35          ; IY OFFSET VALUE
asm_flag3_0     .EQU       0
asm_flag3_1     .EQU       1
asm_flag3_2     .EQU       2
asm_flag3_3     .EQU       3
asm_flag3_4     .EQU       4
asm_flag3_5     .EQU       5
asm_flag3_6     .EQU       6
asm_flag3_7     .EQU       7
.list


