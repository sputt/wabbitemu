;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; CRASH82.INC for CrASH v1.1
;  made by Hideaki Omuro and Jeff Mears
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; RAM Locations
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;Name         Address Len  Meaning
;ÄÄÄÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄ ÄÄÄÄ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
KEY_0        = $8000 ;Byte Scancode of last key, 0 if gotten
KEY_1        = $8001 ;Byte Scancode of key down now
KEY_2        = $8002 ;Byte Same as $8001, $FF if more than one key is down
KEY_STAT     = $8004 ;Byte Bit 2 set = key down now
LAST_KEY     = $8006 ;Byte Last key pressed
CONTRAST     = $8008 ;Byte Contrast ($00 to $1F)
CURSOR_POS   = $800C ;Word Text position for word operations
CURSOR_ROW   = $800C ;Byte Text cursor row
CURSOR_COL   = $800D ;Byte Text cursor column
BUSY_COUNTER = $8026 ;Byte Counter for busy indicator
BUSY_BITMAP  = $8027 ;Byte Bitmap for busy indicator
OP1          = $8028 ;  11 OP1
OP2          = $8033 ;  11 OP2
OP3          = $803E ;  11 OP3
OP4          = $8049 ;  11 OP4
OP5          = $8054 ;  11 OP5
OP6          = $805F ;  11 OP6
TEXT_MEM     = $808F ; 128 text memory
GRAF_CURS    = $8215 ;Word Cursor position for graphics
CURSOR_X     = $8215 ;Byte X value of graphics cursor
CURSOR_Y     = $8216 ;Byte Y value of graphics cursor
APD_BUF      = $8228 ; 768 APD screen buffer
_IY_TABLE    = $8528 ; N/A Where IY usually points
GRAPH_MEM    = $88B8 ; 768 Graphics Buffer
TEXT_MEM2    = $8BDF ; 128 Secondary text memory
WIN_START    = $8C8F ;Byte Number of first row in window
WIN_END      = $8C90 ;Byte Number of last row in window
CRASH_VER    = $8D73 ;Byte Version number of CrASH
VAT_START    = $FE6E ; N/A Start of VAT

; The following will be used in programs using the APD buffer.  If something
; important (ie. interrupt) is going on inside APD_BUF, INT_STATE will not
; be 00. (Look in CrASHprg.TXT for details)
APD_FREE     = $8D70 ;Word Public Free Mem of APD_BUF ($000=none, other=using)
INT_STATE    = $8D72 ;Byte Public Stat of Interrupts (00=none, other=using)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Variable Regions
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; You can allocate memory freely in FREE_A regions.
; You must be careful to not use FREE_B if your program calls CR_KHAND

;Free Memory Regions Len Comments
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ÄÄÄ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
FREE_A1 = GRAPH_MEM ;768 Cleared when starting a program
FREE_A2 = TEXT_MEM  ;128 Cleared when starting a program
FREE_A3 = OP1       ; 66 Can be used if only using normal ROM_CALLs
FREE_B  = TEXT_MEM2 ;128 Cleared after calling CR_KHAND
FREE_C  = APD_BUF   ;768 If INT_STATE is not 0, check for available APD_FREE

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; CrASH call Addresses (use CALL)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; This is a list of useful functions created by the authors of CrASH.  They
; are embedded into CrASH, and are very useful tools.

; Use CALL to call these functions.

EXIT_2_TIOS  = $8D88 ; Exits to TI-OS with a clear screen. (can use JP)
RAND         = $8D8B ; Returns a random number 0-127 in A.
CP_HL_BC     = $8D8E ; Compares HL and BC, like CP_HL_DE
CR_KHAND     = $8D91 ; Key_Hand; does not crash if turned off
CR_GRBCopy   = $8D94 ; Fast copy to LCD - Read below

; CR_GRBCopy is limited:
;  May not work correctly if LCD_Port is set to go auto right
;  May not work correctly if calc is turboed
;  Only copies from GRAPH_MEM
;  Still testing - please report bugs while using it.

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; ROM Addresses (use CALL)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; This is a list of useful functions inside the TI-82's ROM.

; Use CALL to call these functions.

;AddressName  Address Registers Description
;ÄÄÄÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
LD_HL_MHL    = $0033 ;HLA       HL=(HL), A=L
CP_HL_DE     = $0095 ;F         CP  HL, DE
UNPACK_HL    = $00A1 ;HLA       HL=int(HL/10), A=HL MOD 10
DIV_HL_A     = $00A3 ;HLA       HL=int(HL/A), A=HL MOD A
KEY_READ     = $01B8 ;AB        Does raw keyboard read with mask A; returns A
STORE_KEY    = $01C7 ;?         Store keystroke from A
GET_KEY      = $01D4 ;HL?       Get immediate keystroke into A
DISP_DELAY   = $07F3 ;          Delay for display controller      (60 clocks)
FIND_PIXEL   = $4166 ;?         Find pixel (rom page must be 4)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; ROM Offset Addresses (use ROM_CALL)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; To use functions not included here just make a definition like the ones
; below in your program (not recommended).  If you are using the addresses
; in rom found by another source for rom version 19, remember to subtract $1A
; from that address.  (The ones listed are roms version 16-18)

; Use ROM_CALL to call these functions.

#DEFINE ROM_CALL(addr) CALL $8D74 \ .DW addr

TR_CHARPUT   = $3738 ; Raw display of A on screen, normal text style
TX_CHARPUT   = $39D2 ; Xlated display of A on screen, normal text style
D_ZT_STR     = $38FA ; Display zero-terminated normal text string
D_LT_STR     = $373E ; Display length-byte normal text string
D_HL_DECI    = $387C ; Disp HL as 5-char, right just., blank-pad. integer
LAST_LINE    = $3774 ; Is it the last line?
NEXT_LINE    = $377A ; Goto next line (might scroll)
SCROLL_UP    = $3786 ; Scroll text screen up
UP_TEXT      = $37F2 ; Update text mem from command shadow

M_CHARPUT    = $37CE ; Display %A on screen, menu style
D_ZM_STR     = $37D4 ; Display zero-terminated string, menu style
D_LM_STR     = $37DA ; Display length indexed string, menu style

CLEARLCD     = $389A ; Clear LCD, but not text or graphics memory
CLEARTEXT_W  = $37B0 ; Clear LCD and text memory, but only current window (affected by 1,(IY+13))
CLEARTEXT_F  = $37A4 ; Clear LCD and text memory (affected by 1,(IY+13))
CLEAR_DISP   = $38CA ; Clear display
BACKUP_DISP  = $39C6 ; Copy display to APD_BUF
RESTOR_DISP  = $38DC ; Copy APD_BUF to display

; In OShell-82, two of these ROM functions are called by the TI-83 name which
; TI gave them.  To make easier for TI-83 programmers, the following two
; definitions are included, although NOT recommended for usage AT ALL.
;                                    ÄÄÄ                       ÄÄÄÄÄÄ
; Also, there are alternate CrASH calls that will do the same thing, but
; enhanced.  (Look above)

DISP_GRAPH   = $38AC ; Display contents of GRAPH_MEM on LCD
KEY_HAND     = $3924 ; Get key using key handler (2nd, alpha, contrast)

_GRBUFCPY_V  = DISP_GRAPH ; Way too slow
_GETKEY      = KEY_HAND   ; Crashes when you turn off calc. (not a joke)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Ports
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
P_LINK       = $00   ; Link port                                     (TI-85)
P_KEYBOARD   = $01   ; Port used to access keys                      (TI-85)
P_ROM        = $02   ; Rom page switching and other things
P_STATUS     = $03   ; Used to get/set status of varius hardware
P_INT        = $04   ; Interrupt speed (don't change!!! the display needs it)
P_LCD_CON    = $10   ; Control port for display controller           (TI-83)
P_LCD_DATA   = $11   ; Data port for display controller              (TI-83)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Misc
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
#DEFINE TEXT_START RES 1,(IY+0Dh) \ RES 2,(IY+0Dh) \ ROM_CALL(CLEAR_DISP) \ LD HL,0000h \ LD (CURSOR_POS),HL
#DEFINE TEXT_END SET 2,(IY+0Dh) \ SET 1,(IY+0Dh)

G_NONE   = $00
G_DOWN   = $01
G_LEFT   = $02
G_RIGHT  = $03
G_RIGTH  = $03 ; Included because author of ASH couldn't spell :)
G_UP     = $04
G_ENTER  = $09
G_PLUS   = $0A
G_MINUS  = $0B
G_TIMES  = $0C
G_DIV    = $0D
G_CARROT = $0E
G_CLEAR  = $0F
G_NEG    = $11
G_3      = $12
G_6      = $13
G_9      = $14
G_PARENR = $15
G_TAN    = $16
G_VARS   = $17
G_PERIOD = $19
G_2      = $1A
G_5      = $1B
G_8      = $1C
G_PARENL = $1D
G_COS    = $1E
G_PRGM   = $1F
G_STAT   = $20
G_0      = $21
G_1      = $22
G_4      = $23
G_7      = $24
G_COMMA  = $25
G_SIN    = $26
G_MATRIX = $27
G_XTO    = $28
G_ON     = $29
G_STO    = $2A
G_LN     = $2B
G_LOG    = $2C
G_SQR    = $2D
G_INVE   = $2E
G_MATH   = $2F
G_ALPHA  = $30
G_GRAPH  = $31
G_TRACE  = $32
G_ZOOM   = $33
G_WINDOW = $34
G_YEDIT  = $35
G_2ND    = $36
G_MODE   = $37
G_DEL    = $38

K_RIGTH   =   1 ; RIGHT     RIGHT
K_RIGHT   =   1 ; RIGHT     RIGHT
K_LEFT    =   2 ; LEFT      LEFT
K_UP      =   3 ; UP        UP
K_DOWN    =   4 ; DOWN      DOWN
K_ENTER   =   5 ; ENTER     ENTER
K_CLEAR   =   6 ; CLEAR     CLEAR
K_DEL     =   7 ; DEL       DEL
K_INS     =   8 ; DEL       INS
K_RCL     =   9 ; STO>      RCL
K_ENTRY   =  10 ; ENTER     ENTRY
K_BOL     =  11 ; LEFT      BOL
K_EOL     =  12 ; RIGHT     EOL
K_PRGM    =  45 ; PRGM      PRGM
K_ZOOM    =  46 ; ZOOM      ZOOM
K_DRAW    =  47 ; PRGM      DRAW
K_PLOT    =  48 ; Y=        STAT PLOT
K_MATH    =  49 ; MATH      MATH
K_TEST    =  50 ; MATH      TEST
K_VARS    =  52 ; VARS      VARS
K_MEM     =  53 ; +         MEM
K_MATRIX  =  54 ; MATRIX    MATRIX
K_STAT    =  55 ; STAT      STAT
K_Y-VARS  =  56 ; VARS      Y-VARS
K_ANGLE   =  57 ; MATRIX    ANGLE
K_LIST    =  58 ; STAT      LIST
K_CALC    =  59 ; TRACE     CALC
K_QUIT    =  64 ; MODE      QUIT
K_LINK    =  65 ; X,T,theta LINK
K_GRAPH   =  69 ; GRAPH     GRAPH
K_MODE    =  70 ; MODE      MODE
K_WINDOW  =  73 ; WINDOW    WINDOW
K_YEDIT   =  74 ; Y=        Y=
K_TABLE   =  75 ; GRAPH     TABLE
K_TBLSET  =  76 ; WINDOW    TblSet
K_TRACE   =  93 ; TRACE     TRACE
K_PLUS    = 129 ; +         +
K_MINUS   = 130 ; -         -
K_TIMES   = 131 ; *         *
K_DIV     = 132 ; /         /
K_POWER   = 133 ; ^         ^
K_LPAREN  = 134 ; (         (
K_RPAREN  = 135 ; )         )
K_LBRACK  = 136 ; *         [
K_RBRACK  = 137 ; -         ]
K_STO     = 139 ; STO>      STO>
K_COMMA   = 140 ; ,         ,
K_NEG     = 141 ; (-)       (-)
K_PERIOD  = 142 ; .         .
K_0       = 143 ; 0         0
K_1       = 144 ; 1         1
K_2       = 145 ; 2         2
K_3       = 146 ; 3         3
K_4       = 147 ; 4         4
K_5       = 148 ; 5         5
K_6       = 149 ; 6         6
K_7       = 150 ; 7         7
K_8       = 151 ; 8         8
K_9       = 152 ; 9         9
K_EE      = 153 ; ,         EE
K_SEMI    = 154 ; 0         ;
K_A       = 155 ; MATH      A
K_B       = 156 ; MATRIX    B
K_C       = 157 ; PRGM      C
K_D       = 158 ; X^-1      D
K_E       = 159 ; SIN       E
K_F       = 160 ; COS       F
K_G       = 161 ; TAN       G
K_H       = 162 ; ^         H
K_I       = 163 ; X^2       I
K_J       = 164 ; ,         J
K_K       = 165 ; (         K
K_L       = 166 ; )         L
K_M       = 167 ; /         M
K_N       = 168 ; LOG       N
K_O       = 169 ; 7         O
K_P       = 170 ; 8         P
K_Q       = 171 ; 9         Q
K_R       = 172 ; *         R
K_S       = 173 ; LN        S
K_T       = 174 ; 4         T
K_U       = 175 ; 5         U
K_V       = 176 ; 6         V
K_W       = 177 ; -         W
K_X       = 178 ; STO>      X
K_Y       = 179 ; 1         Y
K_Z       = 180 ; 2         Z
K_XTO     = 181 ; X,T,theta X,T,theta
K_PI      = 182 ; ^         Pi
K_INVERSE = 183 ; X^-1      X^-1
K_SIN     = 184 ; SIN       SIN
K_ASIN    = 185 ; SIN       SIN^-1
K_COS     = 186 ; COS       COS
K_ACOS    = 187 ; COS       COS^-1
K_TAN     = 188 ; TAN       TAN
K_ATAN    = 189 ; TAN       TAN^-1
K_SQR     = 190 ; X^2       X^2
K_SQRT    = 191 ; X^2       SQRT
K_LN      = 192 ; LN        LN
K_EX      = 193 ; LN        e^X
K_LOG     = 194 ; LOG       LOG
K_TENP    = 195 ; LOG       10^X
K_ANS     = 198 ; (-)       ANS
K_COLON   = 199 ; .         :
K_QUEST   = 203 ; (-)       ?
K_QUOTE   = 204 ; +         "
K_THETA   = 205 ; 3         THETA
K_LBRACE  = 237 ; (         {
K_RBRACE  = 238 ; )         }
K_ABS     = 244 ; X^-1      ABS
K_L1      = 245 ; 1         L1
K_L2      = 246 ; 2         L2
K_L3      = 247 ; 3         L3
K_L4      = 248 ; 4         L4
K_L5      = 249 ; 5         L5
K_L6      = 250 ; 6         L6
K_n       = 251 ; 9         n
K_Vn1     = 252 ; 8         Vn-1
K_Un1     = 253 ; 7         Un-1

START_ADDR = $9104
.ORG START_ADDR       ; All CrASH Programs will start at address $9104.

; The Mystery of the .END directive...
.END ; End is needed in every single program, and TASM will have a fit over
     ; it, and this works, so here it is.
