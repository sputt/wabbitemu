;----------------------------------------------------------------------------
; TI82.H Only for Ash version 3.1
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; RAM locations
;----------------------------------------------------------------------------
.nolist
KEY_0		=$8000	; translated scancode of last key, but 0 if gotten
KEY_1		=$8001	; translated scancode of key down now
KEY_2		=$8002	; same as 8001, but $FF if more than one key is down
KEY_STAT	=$8004	; bit 2 set = key down now
LAST_KEY	=$8006	; last key pressed
CONTRAST	=$8008	; contrast (0..1Fh)
CURSOR_POS      =$800C  ; text position for word ops
CURSOR_ROW	=$800C	; text cursor row
CURSOR_COL	=$800D	; text cursor column
BUSY_COUNTER	=$8026	; counter for busy indicator
BUSY_BITMAP	=$8027	; bitmap for busy indicator
OP1             =$8028  ; OP1
OP2             =$8033  ; OP2
OP3             =$803E  ; OP3
OP4             =$8049  ; OP4
OP5             =$8054  ; OP5
OP6             =$805F  ; OP6
TEXT_MEM	=$808F	; text memory
CURSOR_X	=$8215	; x value of cursor
CURSOR_Y	=$8216	; y value of cursor
APD_BUF         =$8228  ; buffer used for APD
_IY_TABLE	=$8528	; where IY usually points
GRAPH_MEM       =$88B8  ; $8581  ; Graph mem
TEXT_MEM2	=$8BDF	; secondary text memory
WIN_START       =$8C8F  ; number of first row in window
WIN_END         =$8C90  ; number of last row in window
VAT_START	=$FE6E	; start of VAT

;----------------------------------------------------------------------------
; ROM routines
;----------------------------------------------------------------------------

; The following contains a list of useful functions in the TI82 rom, some have
; to be called using ROM_CALL's others just normals calls. To make it easier
; to find out how to call a function they have been split into two parts.

; Call these the sing call
LD_HL_MHL       =$0033  ; HL=(HL), A=L
CP_HL_DE        =$0095  ; CP HL,DE (modifies flags only)
UNPACK_HL       =$00A1  ; unpacks one digit of HL into %A
STORE_KEY       =$01C7  ; store immediate keystroke from %A
GET_KEY         =$01D4  ; get immediate keystroke into %A
DISP_DELAY      =$07F3  ; delay for display controller

; The function below is used for boss key like features. Calling this function
; will make you goto the homescreen. Since this does not clear the dislay or
; update from the secondary text mem a macro is included which does all this.

BOSS_FUNC = $213
#DEFINE BOOS_OUT ROM_CALL(UP_TEXT) \ LD HL,$0000 \ LD (CURSOR_POS),HL \ ROM_CALL(CLEARLCD) \ JP BOSS_FUNC

FIND_PIXEL      =$4166  ; Find pixel (rom page most be 4 otherwise crash)

; Call these using ROM_CALL's. To use functions not included here just make
; a definition like the ones below in your program. If you are using the adr
; in rom version 19 (like in 82-ROM.TXT), remeber to subtract 1Ah from the
; adr (otherwise dont).

KEY_HAND        =$393E-$1A  ; get key using key.handler (using 2nd,aplha +contrast)
TX_CHARPUT      =$39EC-$1A  ; xlated display of %A on screen, normal text style
D_LT_STR        =$3758-$1A  ; display length-byte normal text string
M_CHARPUT       =$37E8-$1A  ; display %A on screen, menu style
D_ZM_STR        =$37EE-$1A  ; display zero-terminated string,  menu style
D_LM_STR        =$37F4-$1A  ; Display length indexed string, menu style
GET_T_CUR       =$37E2-$1A  ; HL = absolute address of text cursor
LAST_LINE       =$378E-$1A  ; Is it the last line ?
NEXT_LINE       =$3794-$1A  ; goto next line (might scroll)
SCROLL_UP       =$37A0-$1A  ; scroll text screen up
TR_CHARPUT      =$3752-$1A  ; raw display of %A on screen, normal text style
CLEARLCD        =$38B4-$1A  ; clear LCD, but not text or graphics memory
D_HL_DECI       =$3896-$1A  ; disp. HL as 5-byte, right just., blank-pad. decimal
CLEARTEXT_W     =$37CA-$1A  ; clear LCD and text memory,but only current window (affected by 1,(IY+13))
CLEARTEXT_F     =$37BE-$1A  ; clear LCD and text memory (affected by 1,(IY+13))
CLEAR_DISP      =$38E4-$1A  ; clear display
D_ZT_STR        =$3914-$1A  ; display zero-terminated normal text string
BUSY_OFF        =$3932-$1A  ; turn off "busy" indicataor
BUSY_ON         =$394A-$1A  ; turn on "busy" indicator
DONE            =$3884-$1A  ; Print DONE at end of current line
UP_TEXT         =$380C-$1A  ; Update text mem from command shadow
BACKUP_DISP     =$39E0-$1A  ; Copy display to APD_BUF
RESTOR_DISP     =$38F6-$1A  ; Copy APD_BUF to display
DISP_GRAPH      =$38C6-$1A  ; Display contence of graph mem

; In OShell some of the above functions are called by the name TI gave them
; on the TI83, to make it is as easy as possible for programmers the follwing
; definitions are included. I find them hard to remember, but if you want to
; use them do so.

_GRBUFCPY = DISP_GRAPH
_GET_KEY  = KEY_HAND
;----------------------------------------------------------------------------
; Ports
;----------------------------------------------------------------------------

P_LINK       =   0 ; Link port
P_KEYBOARD   =   1 ; Port used to acces keyboard
P_ROM        =   2 ; Rom page switching and other things
P_STATUS     =   3 ; Used to get/set status of varius hardware
P_INT        =   4 ; Controls interrupt speed (do not change the display needs it)
P_LCD_CON    = $10 ; Control port for display controller
P_LCD_DATA   = $11 ; Data port for display controller

;----------------------------------------------------------------------------
; Misc
;----------------------------------------------------------------------------

#DEFINE TEXT_START RES 1,(IY+0Dh) \ RES 2,(IY+0Dh) \ ROM_CALL(CLEAR_DISP) \ LD HL,0000h \ LD (CURSOR_POS),HL
#DEFINE TEXT_END SET 2,(IY+0Dh) \ SET 1,(IY+0Dh)

#DEFINE ROM_CALL(addr) CALL $8D74 \ .DW addr

; All Ash 3.1 programs should start with .ORG START_ADDR. START_ADDR is the
; adresse which you program is moved to before it is started.

START_ADDR = $9104
.list