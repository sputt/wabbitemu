; Gets page pageNum ($bootPage) on progName
; By Andree Chea

#ifdef COLOR
#include "ti84pcse.inc"
_ClrScrnFull .equ _maybe_ClrScrnFull
#else
#include "ti83plus.inc"
#endif

#define TI_83P_BOOTPAGE $1F
#define TI_83PSE_BOOTPAGE $7F
#define TI_84P_BOOTPAGE $3F
#define TI_84P_BOOTPAGE2 $2F
#define TI_84PSE_BOOTPAGE $7F
#define TI_84PSE_BOOTPAGE2 $6F
#define TI_84PCSE_BOOTPAGE $FF
#define TI_84PCSE_BOOTPAGE2 $FD

#define STRING_LEN 5
#define PAGE_SIZE $4000

.org usermem-2
#ifdef COLOR
.db $EF, $69
#else
.db $BB, $6D
#endif
	bcall(_ClrScrnFull)
	ld hl, $0000
	ld (curRow), hl
#ifdef COLOR
	ld a, TI_84PCSE_BOOTPAGE
	ld (BootPage1), a
	ld a, TI_84PCSE_BOOTPAGE2
	ld (BootPage2), a
	ld hl, String84PCSE
#else
	in a, ($02)
	rlca
	jr nc, Page83P
	and $40
	jr z, Page83PSE
_
	in a,($21)
	and $03
	jr z, Page84P
	jr nz, Page84PSE
Page83P:
	ld a, TI_83P_BOOTPAGE
	ld (BootPage1), a
	ld hl, String83P
	jr DonePage
Page83PSE:
	ld a, TI_83PSE_BOOTPAGE
	ld (BootPage1), a
	ld hl, String84PSE
	jr DonePage
Page84P:
	ld a, TI_84P_BOOTPAGE
	ld (BootPage1), a
	ld a, TI_84P_BOOTPAGE2
	ld (BootPage2), a
	ld hl, String84P
	jr DonePage
Page84PSE:
	ld a, TI_84PSE_BOOTPAGE
	ld (BootPage1), a
	ld a, TI_84PSE_BOOTPAGE2
	ld (BootPage2), a
	ld hl, String84PSE
#endif
DonePage:
	push hl
	ld de, StringSuccessReplace
	ld bc, STRING_LEN
	ldir
	pop hl
	push hl
	ld de, StringErrDefinedReplace
	ld bc, STRING_LEN
	ldir
	pop hl
	ld de, SaveVariableReplace
	ld bc, STRING_LEN
	ldir
	call CreateAppVar
	ld a,(BootPage1)
	ld hl, $4000
	ld bc, PAGE_SIZE
	bcall(_FlashToRam)
	ld hl, StringSuccess
	ld a, (BootPage2)
	or a
	jr z, DisplayAndExit
	call DisplayAndExit
	ld hl, SaveVariable
	rst $20
	bcall(_Arc_UnArc)
	ld hl, CurrentPage
	inc (hl)
	call CreateAppVar
	ld a, (BootPage2)
	ld hl, $4000
	ld bc, PAGE_SIZE
	bcall(_FlashToRam)
	ld hl, StringSuccess
DisplayAndExit:
	bcall(_PutS)
	bcall(_NewLine)
	ret

CreateAppVar:
	call UpdatePageNum
	ld hl, SaveVariable
	rst $20
	bcall(_ChkFindSym)
	ld hl, StringErrDefined
	jr nc, ErrorDefined
	ld hl, SaveVariable
	rst $20
	ld hl, $4000
	bcall(_EnoughMem)
	ld hl, StringErrMemory
	jr c, ErrorDefined
	ex de, hl
	bcall(_CreateAppVar)
	inc de
	inc de
	ret
ErrorDefined:
	pop de
	jr DisplayAndExit
	ret
UpdatePageNum:
	ld a, (CurrentPage)
	add a, '1' - 1
	ld (StringSuccessReplaceNum),a
	ld (StringErrDefinedReplaceNum),a
	ld (SaveVariableReplaceNum),a
	ret

CurrentPage:
	.db 1
BootPage1:
	.db 0
BootPage2:
	.db 0

String83P:
	.db "83PBE"
String83PSE:
	.db "83PSE"
String84P:
	.db "84PBE"
String84PSE:
	.db "84PSE"
String84PCSE:
	.db "84CSE"

StringSuccess:
	.db "AppVar D"
StringSuccessReplace:
	.db "     "
StringSuccessReplaceNum:
	.db " "
	.db " created.",$00

StringErrMemory:
	.db "You do not have "
	.db "16KBs of RAM    "
	.db "free.",$00

StringErrDefined:
	.db "AppVar "
StringErrDefinedReplace:
	.db "     "
StringErrDefinedReplaceNum:
	.db " "
	.db " already exists.",$00

SaveVariable:
	.db AppVarObj
	.db "D"
SaveVariableReplace:
	.db "     "
SaveVariableReplaceNum:
	.db " "
	.db $00

.end
