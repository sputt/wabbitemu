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
#define PushGUIStack(xx,yyyy,zzzz) ld a,xx \ ld hl,yyyy \ ld de,zzzz \ call _PushGUIStack
#define PopGUIStack() call _PopGUIStack
#define PopGUIStacks(xx) ld b,xx \ call _PopGUIStackS
#define OpenGUIStack() call _OpenGUIStack
#define CloseGUIStack() call _CloseGUIStack
#define GUIMouse(xxxx) ld hl,xxxx \ call _GUIMouse
#define GUIRender() call _RenderGUI
#define RenderGUI() call _RenderGUI
#define FileSaveAs(location,filetypepointer,size) ld hl,location \ ld de,filetypepointer \ ld bc,size \ call _FileSaveAs
#define FileSave(location,filetypepointer,size) ld hl,location \ ld de,file \ ld bc,size \ call _FileSave
#define FileOpen() call _FileOpen
#define MouseRecover() call ResetAppPage
#define Pause() call _Pause
#define GUIFindFirst() call _GUIFindFirst
#define GUIFindNext(xxxx,yyyy) ld hl,xxxx \ ld de,yyyy \ call _GUIFindNext
#define GUIFindNext() call _GUIFindNext
#define PushGUIStacks(location) ld hl,location \ call _PushGUIStacks
#define GUIFindThis(offset) ld b,offset \ call _GUIFindThis
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
_RenderGUI 	      = $41FA
PopGUIStacks	      = $41FD
_GUIMouse 	      = $4200
_GUIFindFirst 	      = $4203
_GUIFindNext 	      = $4206
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
_PushGUIStacks	  = $422A
_GUIFindThis		  = $422D

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
#endif
.list