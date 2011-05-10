;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
;                     ЬЬЬЬЬ  ЬЬЬ  Ь   Ь ЬЬЬЬЬ Ь     Ь
;                        ЬЯ Ы   Я Ы   Ы Ы     Ы     Ы
;                      ЬЯ    ЯЯЯЬ ЫЯЯЯЫ ЫЯЯ   Ы     Ы
;                     ЫЬЬЬЬ ЯЬЬЬЯ Ы   Ы ЫЬЬЬЬ ЫЬЬЬЬ ЫЬЬЬЬ
;                         Version 4.0 - November 1995
;                                Copyright 1995
;                          Dan Eble (eble.2@osu.edu)
;                      Magnus Hagander (e95_mha@e.kth.se)
;                    Rob Taylor (rtaylor@rtaylor.u-net.com)
;
;                     Extended header file (C) 20/12 1995
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД

;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; #DEFINEs for relative calls and jumps included in ZShell
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
#DEFINE ROM_CALL(index)         call $8C09 \ .db index
#DEFINE CALL_Z(addr)            call $8C0C \ .dw addr
#DEFINE CALL_(addr)             call $8C0F \ .dw addr
#DEFINE CALL_NZ(addr)           call $8C12 \ .dw addr
#DEFINE CALL_C(addr)            call $8C18 \ .dw addr
#DEFINE CALL_NC(addr)           call $8C1E \ .dw addr
#DEFINE JUMP_Z(addr)            call $8C24 \ .dw addr
#DEFINE JUMP_(addr)             call $8C27 \ .dw addr
#DEFINE JUMP_NZ(addr)           call $8C2A \ .dw addr
#DEFINE JUMP_C(addr)            call $8C30 \ .dw addr
#DEFINE JUMP_NC(addr)           call $8C36 \ .dw addr


;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; #DEFINEs for conditional ROM_CALL:s. The functionality will be
; implemented in your own program, not in ZShell.
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
#DEFINE ROM_CALL_C(index)       .db $30,$04 \ call $8c09 \ .db index
#DEFINE ROM_CALL_NC(index)      .db $38,$04 \ call $8c09 \ .db index
#DEFINE ROM_CALL_Z(index)       .db $20,$04 \ call $8c09 \ .db index
#DEFINE ROM_CALL_NZ(index)      .db $28,$04 \ call $8c09 \ .db index


;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; ZShell Specific Data areas
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
PROGRAM_ADDR	=$8C3C	; program address
ROM_VERS	=$8C3E	; ROM version byte
ZSHELL_VER	=$8C3F	; ZShell version number
ZS_BITS		=$8C40	; bit 0 set : checksum again
                        ; bit 1 set : get out without screen


;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; Call addresses
; These addresses are in the same location in all ROM versions, and can
; therefor be accessed through a direct call.
; There is no need to use the ROM_CALL function.
; Instead, use e.g. "call LD_HL_MHL", without using any macro.
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
LD_HL_MHL       =$0033  ; HL=(HL), A=L
CP_HL_DE        =$008E  ; CP HL,DE (modifies flags only)
UNPACK_HL       =$009A  ; unpacks one digit of HL into %A
STORE_KEY       =$01B1  ; store immediate keystroke from %A
GET_KEY         =$01BE  ; get immediate keystroke into %A


;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; Call Indexes
; These indexes are used in the ROM_CALL macro to call functios that
; are in the TI-85's ROM.
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
TX_CHARPUT       =$00   ; xlated display of %A on screen, normal text style
D_LT_STR         =$01   ; display length-byte normal text string
M_CHARPUT        =$02   ; display %A on screen, menu style
D_ZM_STR         =$03   ; display zero-terminated string,  menu style
D_LM_STR         =$04   ; display length-byte string, menu style
GET_T_CUR        =$05   ; HL = absolute address of text cursor
SCROLL_UP        =$06   ; scroll text screen up
TR_CHARPUT       =$07   ; raw display of %A on screen, normal text style
CLEARLCD         =$08   ; clear LCD, but not text or graphics memory
D_HL_DECI        =$09   ; disp. HL as 5-byte, right just., blank-pad. decimal
CLEARTEXT        =$0A   ; clear LCD and text memory (affected by 1,(IY+13))
D_ZT_STR         =$0B   ; display zero-terminated normal text string
BUSY_OFF         =$0C   ; turn off "busy" indicataor
BUSY_ON          =$0D   ; turn on "busy" indicator
FIND_PIXEL       =$80   ; Find location in RAM for a pixel on the GFX screen
                        ; WARNING! Page must be set to 4 before this function
                        ; is called! Do "ld a,4" and "out (5),a" do to that.

;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; Data Locations
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
KEY_0		=$8000	; translated scancode of last key, but 0 if gotten
KEY_1		=$8001	; translated scancode of key down now
KEY_2		=$8002	; same as 8001, but $FF if more than one key is down
KEY_STAT	=$8004	; bit 2 set = key down now
LAST_KEY	=$8006	; last key pressed
CONTRAST	=$8007	; contrast
CURSOR_ROW	=$800C	; text cursor row
CURSOR_COL	=$800D	; text cursor column
CURSOR_LET	=$800E	; letter under text cursor
BUSY_COUNTER	=$8080	; counter for busy indicator
BUSY_BITMAP	=$8081	; bitmap for busy indicator
CURR_INPUT	=$80C6	; -> current home-screen input
BYTES_USED	=$80CC	; # of used user memory (Add to 80C8 to find first 
			; byte of free memory)
TEXT_MEM	=$80DF	; text memory
CURSOR_ROW2	=$800C	; text cursor row
CURSOR_COL2	=$800D	; text cursor column
CHECKSUM	=$81BE	; memory checksum from 8BF7 to FA6F
CURSOR_X	=$8333	; x value of cursor
CURSOR_Y	=$8334	; y value of cursor
_IY_TABLE	=$8346	; where IY usually points
GRAPH_MEM	=$8641	; graphics memory
TEXT_MEM2	=$8A6B	; secondary text memory
USER_MEM	=$8B1B	; -> start of user memory
FIXED_POINT	=$8B3A	; fixed-point decimal place (FFh for floating point)
VAT_END		=$8BEB	; -> one byte before end of VAT (backwards)
VAT_START	=$FA6F	; start of VAT
VIDEO_MEM	=$FC00	; video memory


;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
; Key Scancode definitions
;ДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДДД
;                     Function                Character
K_NOKEY       =$00    ;No key
K_DOWN        =$01    ;Down
K_LEFT        =$02    ;Left
K_RIGHT       =$03    ;Right
K_UP          =$04    ;Up
K_ENTER       =$09    ;Enter
K_PLUS        =$0A    ;+                      X
K_MINUS       =$0B    ;-                      T
K_STAR        =$0C    ;*                      O
K_SLASH       =$0D    ;/                      J
K_RAISE       =$0E    ;^                      E
K_CLEAR       =$0F    ;Clear
K_SIGN        =$11    ;(-)                    Space
K_3           =$12    ;3                      W
K_6           =$13    ;6                      S
K_9           =$14    ;9                      N
K_RIGHTPAR    =$15    ;)                      I
K_TAN         =$16    ;Tan                    D
K_CUSTOM      =$17    ;Custom
K_DOT         =$19    ;.                      Z
K_2           =$1A    ;2                      V
K_5           =$1B    ;5                      R
K_8           =$1C    ;8                      M
K_LEFTPAR     =$1D    ;(                      H
K_COS         =$1E    ;Cos                    C
K_PRGM        =$1F    ;Prgm
K_DEL         =$20    ;Del
K_0           =$21    ;0                      Y
K_1           =$22    ;1                      U
K_4           =$23    ;4                      Q
K_7           =$24    ;7                      L
K_EE          =$25    ;EE                     G
K_SIN         =$26    ;Sin                    B
K_STAT        =$27    ;Stat
K_XVAR        =$28    ;x-Var                  x
K_ON          =$29    ;On
K_STO         =$2A    ;Sto                    =
K_COMMA       =$2B    ;,                      P
K_SQUARE      =$2C    ;x^2                    K
K_LN          =$2D    ;Ln                     F
K_LOG         =$2E    ;Log                    A
K_GRAPH       =$2F    ;Graph
K_ALPHA       =$30    ;Alpha
K_F5          =$31    ;F5
K_F4          =$32    ;F4
K_F3          =$33    ;F3
K_F2          =$34    ;F2
K_F1          =$35    ;F1
K_SECOND      =$36    ;2nd
K_EXIT        =$37    ;EXIT
K_MORE        =$38    ;MORE
