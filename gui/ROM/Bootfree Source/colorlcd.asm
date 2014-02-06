;;; -*- TI-Asm -*-
;;; 
;;; BootFree -- A free boot loader and library for the TI-73, TI-83
;;; Plus, and TI-84 Plus graphing calculators
;;;
;;; Copyright (C) 2006-2007 Benjamin Moody
;;;
;;; All rights reserved.
;;;
;;; Permission is hereby granted, free of charge, to any person
;;; obtaining a copy of this software and associated documentation
;;; files (the "Software"), to deal in the Software without
;;; restriction, including without limitation the rights to use, copy,
;;; modify, merge, publish, distribute, and/or sell copies of the
;;; Software, and to permit persons to whom the Software is furnished
;;; to do so, provided that the above copyright notice(s) and this
;;; permission notice appear in all copies of the Software and that
;;; both the above copyright notice(s) and this permission notice
;;; appear in supporting documentation.
;;;
;;; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;;; EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
;;; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;;; NONINFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL THE
;;; COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR
;;; ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
;;; ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
;;; PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
;;; TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
;;; PERFORMANCE OF THIS SOFTWARE.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  LCD Routines
;;;

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_DEPTH 3

#define DISPLAY_CONTROL1_REG $07
#define POWER_CONTROL1_REG $10
#define LCD_Y_REG $20
#define LCD_X_REG $21
#define LCD_GRAM_REG $22
#define WINDOW_Y_START_REG $50
#define WINDOW_Y_END_REG $51
#define WINDOW_X_START_REG $52
#define WINDOW_X_END_REG $53

;;
;; LCDInit
;;
;;  Initialize LCD
;;
;;  Input: None
;;  Output: LCD is initialized
;;  Destroys: AF
LCDInit:
#if HW_VERSION >= 4
	ld a, DISPLAY_CONTROL1_REG
	ld hl, $0000
	call LCDWriteReg	; turn off the lcd
	ld a, POWER_CONTROL1_REG
	ld hl, $07F1
	call LCDWriteReg	; set power
#else
	ld a,18h		; reset test mode
	call LCDOut
	ld a,01h		; 8 bit mode
	call LCDOut
	ld a,05h		; increment down
	call LCDOut
	ld a,0f0h		; reasonable default contrast
	call LCDOut
#endif
	
#if HW_VERSION >= 4
	;;; LCD delay
	ld a,17h
	out (29h),a
	ld a,0Fh
	out (2Ah),a
	ld a,2Fh
	out (2Bh),a
	ld a,3Bh
	out (2Ch),a
	ld a,45h
	out (2Eh),a
	;;; LCD delay timer
	ld a,4Bh
	out (2Fh),a
#else
#ifdef SILVER
	;;; LCD delay
	ld a,14h
	out (29h),a
	ld a,27h
	out (2Ah),a
	ld a,2Fh
	out (2Bh),a
	ld a,3Bh
	out (2Ch),a

	;;; LCD delay timer
	ld a,4Ah
	out (2Fh),a
#endif
#endif
	ret

;;
;; LCDWriteReg
;;
;;  Safely output an LCD command to port 10h.
;;
;;  Input: A = Register
;;		   HL = Value
;;  Output: None
;;  Destroys: F

LCDWriteReg:
	push bc
	 ld c, $10
	 out (c),a
	 out (c),a
	 inc c
	 out (c),h
	 out (c),l
	pop bc
	ret

ResetCursor:
	ld a, LCD_X_REG
	ld hl, $0000
	call LCDWriteReg
	ld a, LCD_Y_REG
	ld hl, $0000
	call LCDWriteReg
	ret

ResetWindow:
	ld a, WINDOW_Y_START_REG
	ld hl, $0000
	call LCDWriteReg
	ld a, WINDOW_Y_END_REG
	ld hl, $00EF
	call LCDWriteReg
	ld a, WINDOW_X_START_REG
	ld hl, $0000
	call LCDWriteReg
	ld a, WINDOW_X_END_REG
	ld hl, $013F
	call LCDWriteReg
	ret

;;
;; ClearLCD
;;
;;  Clear the screen.
;;
;;  Input: None
;;  Output: None
;;  Destroys: None

ClearLCD:
	push hl
	 push de
	
 	  call ResetWindow
 	  call ResetCursor
	  xor a
	  out ($10),a
	  ld a, LCD_GRAM_REG
	  out ($10),a
	  ld de,19200
_:
	  ld a, $FF
 	  out ($11),a
 	  out ($11),a
 	  out ($11),a
 	  out ($11),a
 	  out ($11),a
 	  out ($11),a
 	  out ($11),a
 	  out ($11),a
 	  dec de
 	  ld a,d
 	  or e
 	  jp nz,-_
 	
 	 pop de
 	pop hl
 	ret

;;
;; PutC
;;
;;  Display a character.
;;
;;  Input: A = character
;;  Output: None
;;  Destroys: None

PutC:
	;; TODO
	ret

;;
;; NewLine2
;;
;;  Move to the next line mod 8.
;;
;;  Input: None
;;  Output: None
;;  Destroys: None

NewLine2:
	push af
	 xor a
	 ld (curCol),a
	 ld a,(curRow)
	 inc a
	 and 7
	 ld (curRow),a
	 pop af
	ret


;;
;; PutS
;;
;;  Display a zero-terminated string.
;;
;;  Input: HL -> string
;;  Output: HL -> byte following string
;;  Destroys: None

PutS:
	push af
PutS_Loop:
	 ld a,(hl)
	 inc hl
	 or a
	 jr z,PutS_Done
	 call PutC
	 jr PutS_Loop
PutS_Done:
	 pop af
	ret