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
;;;  Integer Math
;;;

;;
;; ATimesE
;;
;;  Multiply A by E.
;;
;;  Also known as Mult8By8.
;;
;;  Input: A, E unsigned integers
;;  Output: HL = product
;;  Destroys: AF, DE

ATimesE:
	ld d,0
	;; fall through

;;
;; ATimesDE
;;
;;  Multiply A by DE.
;;
;;  Also known as Mult16By8.
;;
;;  Input: A, DE unsigned integers
;;  Output: HL = product
;;  Destroys: AF, DE

ATimesDE:
	ld hl,0
	or a
	ret z
ATimesDE_loop:
	rra
	jr nc,ATimesDE_0
	add hl,de
ATimesDE_0:
	sla e
	rl d
	or a
	jr nz,ATimesDE_loop
	ret


;;
;; DivHLByE
;;
;;  Divide HL by E.
;;
;;  Also known as Div16By8.
;;
;;  Input: HL, E unsigned integers
;;  Output: DE = quotient, HL = remainder
;;  Destroys: AF, B, [IX, bootTemp (3)]

DivHLByE:
	ld d,0
	;; fall through

;;
;; DivHLByDE
;;
;;  Divide HL by DE.
;;
;;  Also known as Divide16By16.
;;
;;  Input: HL, DE unsigned integers
;;  Output: DE = quotient, HL = remainder
;;  Destroys: AF, B, [IX, bootTemp (3)]

DivHLByDE:
	push bc
	 ld a,h
	 ld c,l
	 ld hl,0
	 ld b,16
DivHLByDE_Loop:
	 sll c
	 rla
	 adc hl,hl
	 jr c,DivHLByDE_C
	 sbc hl,de
	 jr nc,DivHLByDE_1
	 add hl,de
	 dec c
DivHLByDE_1:
	 djnz DivHLByDE_Loop
DivHLByDE_Done:
	 ld d,a
	 ld e,c
	 pop bc
	ret
DivHLByDE_C:
	 or a
	 sbc hl,de
	 djnz DivHLByDE_Loop
	 jr DivHLByDE_Done


;;
;; Div32ByDE
;;
;;  Divide the 32-bit (big endian!) number in OP1 by DE.
;;
;;  Also known as Div32By16.
;;
;;  Input: OP1, DE unsigned integers
;;  Output: OP1 = quotient, OP2 = remainder
;;  Destroys: AF, BC, HL, IX, [bootTemp (4)]

Div32ByDE:
	ld bc,(OP1+2)
	ld ixh,c
	ld ixl,b
	ld hl,(OP1)
	ld a,l
	ld c,h			; ACIX = dividend
	ld b,32
	ld hl,0			; HL = remainder
Div32ByDE_Loop:
	add ix,ix
	rl c
	rla			; shift dividend left into remainder
	adc hl,hl
	jr c,Div32ByDE_C	; carry: must subtract
	sbc hl,de
	jr nc,Div32ByDE_1
	add hl,de
	djnz Div32ByDE_Loop
	jr Div32ByDE_Done
Div32ByDE_C:
	or a
	sbc hl,de
Div32ByDE_1:
	inc ixl
	djnz Div32ByDE_Loop
Div32ByDE_Done:
	ld b,h
	ld h,l
	ld l,b
	ld (OP2+2),hl
	ld hl,0
	ld (OP2),hl
	ld l,a
	ld h,c
	ld (OP1),hl
	ld c,ixh
	ld b,ixl
	ld (OP1+2),bc
	ret
