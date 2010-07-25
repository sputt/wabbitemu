.nolist
#ifdef DCS7
;+----------------------------------------+
;|      Doors CS v7.0 Include File        |
;|         TI-83+/TI-84+ Version          |
;|          Revised July 6, 2010          |
;|           By Kerm Martian              |
;|                                        |
;| Please see http://dcs.cemetech.net for |
;| information and updates including a    |
;| full Developers' SDK.                  |
;+----------------------------------------+

;----> Safe Memory Areas
; saferam1 = 768 bytes (APDRAM)
; saferam2 = 531 bytes (statRAM)
; saferam3 = 128 bytes (textMem)
; saferam4 = 66 bytes (OpXs)
; saferam5 = 10 bytes (iMathPtrs)
SEram		=$86EC
saferam1	=9872h
saferam2	=8A3Ah
saferam3	=8508h
saferam4	=8478h
saferamp	=9872h
saferamp2	=8251h

#ifndef TASM
#define PushGUIStack(xx,yyyy,zzzz) ld a,xx \ ld hl,yyyy \ ld de,zzzz \ call PushGUIStack
#define PopGUIStack() call PopGUIStack
#define PopGUIStacks(xx) ld b,xx \ call PopGUIStackS
#define OpenGUIStack() call OpenGUIStack
#define CloseGUIStack() call CloseGUIStack
#define GUIMouse(xxxx) ld hl,xxxx \ call GUIMouse
#define GUIRender() call RenderGUI
#define RenderGUI() call RenderGUI
#define FileSaveAs(location,filetypepointer,size) ld hl,location \ ld de,filetypepointer \ ld bc,size \ call FileSaveAs
#define FileSave(location,filetypepointer,size) ld hl,location \ ld de,file \ ld bc,size \ call FileSave
#define FileOpen() call FileOpen
#define MouseRecover() call ResetAppPage
#define Pause() call Pause
#define GUIFindFirst() call GUIFindFirst
#define GUIFindNext(xxxx,yyyy) ld hl,xxxx \ ld de,yyyy \ call GUIFindNext
#define GUIFindNext() call GUIFindNext
#define PushGUIStacks(location) ld hl,location \ call PushGUIStacks
#define GUIFindThis(offset) ld b,offset \ call GUIFindThis
#endif

iversion              = $4083
irandom               = $4086
isprite               = $4089
ilsprite              = $408C
igetpix               = $408F
ifastcopy             = $4092
idetect               = $4095
idecomp               = $4098

ionversion              = $4083
ionrandom               = $4086
ionputsprite            = $4089
ionlargesprite        = $408C
iongetpixel           = $408F
ionfastcopy           = $4092
iondetect             = $4095
iondecompress         = $4098

iputsprite            = $4089
ilargesprite        = $408C
igetpixel           = $408F
idecompress         = $4098

;=================================;
;====== MirageOS Subroutines =====;
;=================================;

directin              = $409B
sendbytetios          = $409E
getbytetios           = $40A1
version               = $40A4
setvputs              = $40A7
setpixel              = $40AA
fastcopys             = $40AD
delayb                = $40B0
multhe                = $40B3
multhl                = $40B6
quittoshell           = $40B9
fastline              = $40BC
pixelonhl             = $40BF
pixeloff              = $40C2
pixelxor              = $40C5
pixeltest             = $40C8
pixeloffhl            = $40CB
pixelxorhl            = $40DE
pixeltesthl           = $40D1
fastlineb             = $40D4
fastlinew             = $40D7
fastlinex             = $40DA
pointonc              = $40DD
pointoffc             = $40E0
pointxorc             = $40E3
centertext            = $40E6
cphlbc                = $40E9
putsprite8            = $40EC
fastcopyb             = $40EF
vputsc                = $40F2
scrolld7              = $40F5
vnewline              = $40F8
rand127               = $40FB
disprle               = $40FE
cphlde                = $4101
screentopic           = $4104
fastlined             = $4107
disprlel              = $410A
getnextgoodprog       = $410D
getprevgoodprog       = $4110
getnext               = $4113
getprev               = $4116
compstrs              = $4119
nextstr               = $411C
getinfo               = $411F
fastrectangle         = $4122
gettext               = $4125
gettextv              = $4128
FastRectangle_Save    = $412B
vputa                 = $412E
runprog               = $4131
isgoodprog            = $4134
existfold             = $4137
delprog               = $413A
filledrectangle       = $413D
nextfolder            = $4140
delfolder             = $4143
moveall               = $4149
curfoldname           = $4149
curfoldnamea          = $414C
createfolder          = $414F
compstrsn             = $4152
folder_menu_start     = $4155
options_screen        = $4158
put_folder_name_top   = $415B
general_key_routine   = $415E
find_num_good_progs   = $4161
put_scrollbar         = $4164
invert_lines          = $4167
invert_1_line         = $416A
right_align_value     = $416D
put_mirageos_header   = $4170
put_size_graphic      = $4173
sendprog              = $4176
hideprog              = $4179
arcprog               = $417C
filledrectangle_save  = $417F
getbytetiosw          = $4182
vatswap               = $4185
renameprog            = $4188
renamefolder          = $418B
sysmain               = $418E
setupint              = $4191
move_gui_prog         = $4194
largespritehl         = $4197
Update_Scrollbar      = $419A
Initial_Scrollbar     = $419D
sortallfolds          = $41A0
dofoldsort            = $41A3
getfoldsort           = $41A6
setfoldsort           = $41A9
Increase_Cur_Element  = $41AC
Decrease_Cur_Element  = $41AF
Increase_Max_Elements = $41B2
Decrease_Max_Elements = $41B5
Add_A_To_Cur_Element  = $41B8
Sub_A_From_Cur_Element = $41BB
Add_A_To_Max_Elements = $41BE
Sub_A_From_Max_Elements = $41C1
Skip_Forward_B_From_Top = $41C4
Get_Curgoodprog_Ptr   = $41C7
getchecksum           = $41CA
freearc               = $41CD
swapram               = $41D0
hideall               = $41D3
Small_Window 	      = $41D6
ClrDialogFull 	      = $41D9
LargeWindow 	      = $41DC
ClrWinFull 	      = $41DF
PlaySound 	      = $41E2
VDispHL 	      = $41E5
Pause 	              = $41E8
hDetect 	      = $41EB
OpenGUIStack 	      = $41EE
CloseGUIStack	      = $41F1
PushGUIStack 	      = $41F4
PopGUIStack 	      = $41F7
RenderGUI 	      = $41FA
PopGUIStacks	      = $41FD
GUIMouse 	      = $4200
GUIFindFirst 	      = $4203
GUIFindNext 	      = $4206
ResetAppPage 	      = $4000
Cn2_Setup	      = $4209
Cn2_Clear_SendBuf     = $420C
Cn2_Clear_RecBuf      = $420F
Cn2_Setdown	      = $4212
FileOpen 	      = $4215
FileSave 	      = $4218
FileSaveAs 	      = $421B
DispLongInt	      = $421E
Cn2_GetK		  = $4221
DPutMap			  = $4224
APGui_gui7ToTop   = $4227
PushGUIStacks	  = $422A
GUIFindThis		  = $422D

null				=$00
LargeWin				=$01
SmallWin				=$02
FullScreenImg			=$03
Text				=$04
WinButtons				=$05
WrappedText				=$06
ButtonText				=$07
ButtonImg				=$08
TextLineIn				=$09
Radio				=$0A
Checkbox				=$0B
ByteInt				=$0C
WordInt				=$0D
Hotspot				=$0E
TextMultiline			=$0F
Sprite				=$10
LargeSprite				=$11
PassIn				=$12
ScrollVert				=$13
ScrollHoriz				=$14
Border				=$15
Rect				=$16
Custom				=$17
MouseCursor				=$18
GUIRnull					=$00			;COMPLETE	XY	[none]
GUIRLargeWin				=$01			;COMPLETE	--	[5 byte icon, header text]
GUIRSmallWin				=$02			;COMPLETE	XY	[x,y, 5 byte icon, header text]
GUIRFullScreenImg			=$03			;COMPLETE	XY	[768 bytes]
GUIRText					=$04			;COMPLETE	XY	[x,y,font,z.t. text]
GUIRWinButtons				=$05			;COMPLETE	--	[which {-,[],X,00000}, onclick[-] {word}, onclick[] {word}, onclick[X] {word}]
GUIRWrappedText				=$06			;COMPLETE	XY	[x,y,width,font,z.t. text]
GUIRButtonText				=$07			;COMPLETE	XY	[x,y,onclick {word},zt text]
GUIRButtonImg				=$08			;COMPLETE	XY	[x,y,onclick {word},img width [BYTES],button width [PX], img data (5 rows high)]
GUIRTextLineIn				=$09			;COMPLETE	XY	[x,y,width,maxchar [word],curdatapos (0 to datalength) [word],data (z.t.)]
GUIRRadio					=$0A			;COMPLETE	XY	[x,y,group,state {bool},zttext]  1 per group
GUIRCheckbox				=$0B			;COMPLETE	XY	[x,y,group,state {bool},zttext] ++ per group
GUIRByteInt					=$0C			;COMPLETE	XY	[x,y,value,min,max]
GUIRWordInt					=$0D			;COMPLETE	XY	[x,y,value,min,max]
GUIRHotspot					=$0E			;COMPLETE	XY	[x,y,width,height,ptr]
GUIRTextMultiline			=$0F			;COMPLETE	XY	[x,y,rows,width,curdatapos (0 to datalength) [word],data]
GUIRSprite					=$10			;COMPLETE	XY	[x,y,b,data]
GUIRLargeSprite				=$11			;COMPLETE	XY	[x,y,a,b,data]
GUIRPassIn					=$12			;COMPLETE	XY	[x,y,width,maxchar [word],curdatapos (0 to datalength) [word],data (z.t.)]
GUIRScrollVert				=$13			;COMPLETE	XY	[x,y,height,ID,per,min [word],max [word],cur [word],onScrollUp,onScrollDown]
GUIRScrollHoriz				=$14			;COMPLETE	XY	[x,y,width,ID,per,min [word],max [word],cur [word],onScrollUp,onScrollDown]
GUIRBorder					=$15			;COMPLETE	XY	[x,y,width,height,color]
GUIRRect					=$16			;COMPLETE	XY	[x,y,width,height,fill]
GUIRCustom					=$17			;COMPLETE	--	[ptr_to_routine]
GUIRMouseCursor				=$18			;COMPLETE	--  [xy,xy,8-byte mask,8-byte sprite]
GUIRMouseClick				=$19		

;other compatibility equates
gbuf 			.equ	plotsscreen

;End of file

;end
#else
;+----------------------------------------+
;|      Doors CS v6.0 Include File        |
;|         TI-83+/TI-84+ Version          |
;|         Revised March 17, 2007         |
;|           By Kerm Martian              |
;|                                        |
;| Please see http://dcs.cemetech.net for |
;| information and updates including a    |
;| full Developers' SDK.                  |
;+----------------------------------------+

;----> Safe Memory Areas
; saferam1 = 768 bytes (APDRAM)
; saferam2 = 531 bytes (statRAM)
; saferam3 = 128 bytes (textMem)
; saferam4 = 66 bytes (OpXs)
; saferam5 = 10 bytes (iMathPtrs)

#DEFINE equ .equ
#DEFINE EQU .equ
#DEFINE end .end
#DEFINE END .end

#define bcall(xxxx)	rst 28h 	\ .dw xxxx
#define	bcallz(xxxx)	jr nz,$+5 	\ rst 28h \ .dw xxxx
#define	bcallnz(xxxx)	jr z,$+5 	\ rst 28h \ .dw xxxx
#define	bcallc(xxxx)	jr nc,$+5 	\ rst 28h \ .dw xxxx
#define	bcallnc(xxxx)	jr c,$+5	\ rst 28h \ .dw xxxx
#define bjump(xxxx)	call 50h 	\ .dw xxxx
#define b_call(xxxx)	rst 28h 	\ .dw xxxx
#define	b_callz(xxxx)	jr nz,$+5 	\ rst 28h \ .dw xxxx
#define	b_callnz(xxxx)	jr z,$+5 	\ rst 28h \ .dw xxxx
#define	b_callc(xxxx)	jr nc,$+5 	\ rst 28h \ .dw xxxx
#define	b_callnc(xxxx)	jr c,$+5 	\ rst 28h \ .dw xxxx
#define b_jump(xxxx)	call 50h 	\ .dw xxxx
 .addinstr BCALL *       EF   3 NOP 1
appBackupScreen	=$9872	; 768 bytes
pendfile	.equ	$8A3A
SEram		=$86EC
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
progstart	=9D93h
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

#define PushGUIStack(xx,yyyy,zzzz) ld a,xx \ ld hl,yyyy \ ld de,zzzz \ call PushGUIStack
#define PopGUIStack() call PopGUIStack
#define PopGUIStacks(xx) ld b,xx \ call PopGUIStackS
#define OpenGUIStack() call OpenGUIStack
#define CloseGUIStack() call CloseGUIStack
#define GUIMouse(xxxx) ld hl,xxxx \ call GUIMouse
#define GUIMouse() ld hl,0 \ call GUIMouse
#define GUIRender() call RenderGUI
#define RenderGUI() call RenderGUI
#define FileSaveAs(location,filetypepointer,size) ld hl,location \ ld de,filetypepointer \ ld bc,size \ call FileSaveAs
#define FileSave(location,filetypepointer,size) ld hl,location \ ld de,file \ ld bc,size \ call FileSave
#define FileOpen() call FileOpen
#define MouseRecover() call ResetAppPage
#define Pause() call Pause
#define GUIFindFirst() call GUIFindFirst
#define GUIFindNext(xxxx,yyyy) ld hl,xxxx \ ld de,yyyy \ call GUIFindNext
#define GUIFindNext() call GUIFindNext

iversion              = $4083
irandom               = $4086
isprite               = $4089
ilsprite              = $408C
igetpix               = $408F
ifastcopy             = $4092
idetect               = $4095
idecomp               = $4098

ionversion              = $4083
ionrandom               = $4086
ionputsprite            = $4089
ionlargesprite        = $408C
iongetpixel           = $408F
ionfastcopy           = $4092
iondetect             = $4095
iondecompress         = $4098

iputsprite            = $4089
ilargesprite        = $408C
igetpixel           = $408F
idecompress         = $4098

;=================================;
;====== MirageOS Subroutines =====;
;=================================;

directin              = $409B
sendbytetios          = $409E
getbytetios           = $40A1
version               = $40A4
setvputs              = $40A7
setpixel              = $40AA
fastcopys             = $40AD
delayb                = $40B0
multhe                = $40B3
multhl                = $40B6
quittoshell           = $40B9
fastline              = $40BC
pixelonhl             = $40BF
pixeloff              = $40C2
pixelxor              = $40C5
pixeltest             = $40C8
pixeloffhl            = $40CB
pixelxorhl            = $40DE
pixeltesthl           = $40D1
fastlineb             = $40D4
fastlinew             = $40D7
fastlinex             = $40DA
pointonc              = $40DD
pointoffc             = $40E0
pointxorc             = $40E3
centertext            = $40E6
cphlbc                = $40E9
putsprite8            = $40EC
fastcopyb             = $40EF
vputsc                = $40F2
scrolld7              = $40F5
vnewline              = $40F8
rand127               = $40FB
disprle               = $40FE
cphlde                = $4101
screentopic           = $4104
fastlined             = $4107
disprlel              = $410A
getnextgoodprog       = $410D
getprevgoodprog       = $4110
getnext               = $4113
getprev               = $4116
compstrs              = $4119
nextstr               = $411C
getinfo               = $411F
fastrectangle         = $4122
gettext               = $4125
gettextv              = $4128
FastRectangle_Save    = $412B
vputa                 = $412E
runprog               = $4131
isgoodprog            = $4134
existfold             = $4137
delprog               = $413A
filledrectangle       = $413D
nextfolder            = $4140
delfolder             = $4143
moveall               = $4149
curfoldname           = $4149
curfoldnamea          = $414C
createfolder          = $414F
compstrsn             = $4152
folder_menu_start     = $4155
options_screen        = $4158
put_folder_name_top   = $415B
general_key_routine   = $415E
find_num_good_progs   = $4161
put_scrollbar         = $4164
invert_lines          = $4167
invert_1_line         = $416A
right_align_value     = $416D
put_mirageos_header   = $4170
put_size_graphic      = $4173
sendprog              = $4176
hideprog              = $4179
arcprog               = $417C
filledrectangle_save  = $417F
getbytetiosw          = $4182
vatswap               = $4185
renameprog            = $4188
renamefolder          = $418B
sysmain               = $418E
setupint              = $4191
move_gui_prog         = $4194
largespritehl         = $4197
Update_Scrollbar      = $419A
Initial_Scrollbar     = $419D
sortallfolds          = $41A0
dofoldsort            = $41A3
getfoldsort           = $41A6
setfoldsort           = $41A9
Increase_Cur_Element  = $41AC
Decrease_Cur_Element  = $41AF
Increase_Max_Elements = $41B2
Decrease_Max_Elements = $41B5
Add_A_To_Cur_Element  = $41B8
Sub_A_From_Cur_Element = $41BB
Add_A_To_Max_Elements = $41BE
Sub_A_From_Max_Elements = $41C1
Skip_Forward_B_From_Top = $41C4
Get_Curgoodprog_Ptr   = $41C7
getchecksum           = $41CA
freearc               = $41CD
swapram               = $41D0
hideall               = $41D3
Small_Window 	      = $41D6
ClrDialogFull 	      = $41D9
LargeWindow 	      = $41DC
ClrWinFull 	      = $41DF
PlaySound 	      = $41E2
VDispHL 	      = $41E5
Pause 	              = $41E8
hDetect 	      = $41EB
OpenGUIStack 	      = $41EE
CloseGUIStack	      = $41F1
PushGUIStack 	      = $41F4
PopGUIStack 	      = $41F7
RenderGUI 	      = $41FA
PopGUIStacks	      = $41FD
GUIMouse 	      = $4200
GUIFindFirst 	      = $4203
GUIFindNext 	      = $4206
ResetAppPage 	      = $4000
Cn2_Setup	      = $4209
Cn2_Clear_SendBuf     = $420C
Cn2_Clear_RecBuf      = $420F
Cn2_Setdown	      = $4212
FileOpen 	      = $4215
FileSave 	      = $4218
FileSaveAs 	      = $421B
DispLongInt	      = $421E

GUIRnull				=$00
GUIRLargeWin				=$01
GUIRSmallWin				=$02
GUIRFullScreenImg			=$03
GUIRText				=$04
GUIRWinButtons				=$05
GUIRWrappedText				=$06
GUIRButtonText				=$07
GUIRButtonImg				=$08
GUIRTextLineIn				=$09
GUIRRadio				=$0A
GUIRCheckbox				=$0B
GUIRByteInt				=$0C
GUIRWordInt				=$0D
GUIRHotspot				=$0E
GUIRTextMultiline			=$0F
GUIRSprite				=$10
GUIRLargeSprite				=$11
GUIRPassIn				=$12
GUIRScrollVert				=$13
GUIRScrollHoriz				=$14
GUIRBorder				=$15
GUIRRect				=$16
GUIRCustom				=$17
GUIRMouseCursor				=$18
null = 0


;End of file

;end
#endif
.list