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

;;
;; LCDOut
;;
;;  Safely output an LCD command to port 10h.
;;
;;  Input: A = LCD command
;;  Output: None
;;  Destroys: F

LCDOut:
	push bc
	 ld c,10h
LCDOut_loop:
	 in f,(c)
	 jp m,LCDOut_loop
	 pop bc
	out (10h),a
	ret


;;
;; LCDDataOut
;; 
;;  Safely output a data byte to port 11h.
;; 
;;  Input: A = data byte
;;  Output: None
;;  Destroys: F

LCDDataOut:
	push bc
	 ld c,10h
LCDDataOut_loop:
	 in f,(c)
	 jp m,LCDDataOut_loop
	 pop bc
	out (11h),a
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
	push af
	 push bc
	  ld a,80h
	  call LCDOut
	  ld a,20h
ClearLCD_Loop:
	  ld c,a
	  call LCDOut
	  ld b,64
	  xor a
ClearLCD_SubLoop:
	  call LCDDataOut
	  djnz ClearLCD_SubLoop
	  ld a,c
	  inc a
	  cp 2Ch
	  jr c,ClearLCD_Loop
	  pop bc
	 pop af
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
	cp 0D6h
	jr z,NewLine2
	push af
	 push bc
	  push de
	   push hl
	    ld l,a
	    ld h,0
	    ld a,i
	    push af
	     di
	     ld a,h
 	     call LCDOut	; set to 6-bit mode
	     ld a,(curRow)
	     add a,a
	     add a,a
	     add a,a
	     add a,80h
	     call LCDOut	; set row
	     add hl,hl
	     add hl,hl
	     add hl,hl
	     ld de,LargeFont+1
	     add hl,de
	     ld a,(curCol)
	     add a,20h
	     call LCDOut	; set column
	     ld b,7
PutC_Loop:
	     ld a,(hl)
	     rla
	     inc hl
	     call LCDDataOut
	     djnz PutC_Loop
	     ld a,(curCol)
	     inc a
	     ld (curCol),a
	     cp 10h
	     call nc,NewLine2
	     ld a,1
	     call LCDOut	; set back to 8-bit mode
	     pop af
	    jp po,PutC_DI
	    ei
PutC_DI:
	    pop hl
	   pop de
	  pop bc
	 pop af
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
