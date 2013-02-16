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
;;;  Big Integer Math
;;;

;;
;; BigNumCompare
;;
;;  Compare the big integers at HL and DE.
;;
;;  Also known as CompareStrings.
;;
;;  Input: HL -> first big integer
;;         DE -> second big integer
;;  Output: Z if numbers are equal
;;  Destroys: AF, B, DE, HL

BigNumCompare:
	ld a,(de)
	cp (hl)
	ret nz
	or a
	ret z
	ld b,a
BigNumCompare_Loop:
	inc hl
	inc de
	ld a,(de)
	cp (hl)
	ret nz
	djnz BigNumCompare_Loop
	ret


;;
;; BigNumMult [UNTESTED]
;;
;;  Multiply big integers at HL and DE.  The output buffer is assumed
;;  to be large enough.
;;
;;  Input: HL -> first big integer
;;         DE -> second big integer
;;         IX -> output buffer (may not overlap inputs)
;;  Output: None
;;  Destroys: AF, BC, DE, HL, IX

BigNumMult:
	ld a,(de)
	add a,(hl)
	ld b,0
	ld c,a
	dec c
	push hl
	 push de
	  ld d,ixh
	  ld e,ixl
	  ld (de),a
	  inc hl
	  ld h,d
	  ld l,e
	  ld (hl),0
	  inc de
	  ldir
	  pop de
	 pop hl
	ld a,(de)
	ld b,a
	inc de
	ld c,(hl)
	inc hl
BigNumMult_Loop:
	inc ix
	push ix
	 push de
	  push bc
BigNumMult_SubLoop:
	   ld a,(de)
	   push de
	    ld e,(hl)
	    push hl
	     call ATimesE
	     ld e,(ix)
	     ld d,(ix+1)
	     add hl,de
	     ld (ix),l
	     ld (ix+1),h
	     jr nc,BigNumMult_NC
	     push ix
BigNumMult_CarryLoop:
	      inc ix
	      inc (ix+1)
	      jr z,BigNumMult_CarryLoop
	      pop ix
BigNumMult_NC:
	     pop hl
	    pop de
	   inc de
	   inc ix
	   djnz BigNumMult_SubLoop
	   inc hl
	   pop bc
	  pop de
	 pop ix
	dec c
	jr nz,BigNumMult_Loop
	ret
	

;;
;; BigNumMod [UNTESTED]
;;
;;  Reduce the big integer at HL mod bigN; store the result back at HL.
;;
;;  Input: HL -> big integer
;;  Output: None
;;  Destroys: AF, BC, DE, HL

BigNumMod:
	ld a,(bigN)
	dec a
BigNumMod_Again:
	cp (hl)			; if this comparison doesn't carry,
	ret nc			; then (HL) is definitely less than N
	ld b,a
	ld e,(hl)
	ld d,0
	ex de,hl
	add hl,de
	ex de,hl
	ld a,(de)
	or a
	jr nz,BigNumMod_SizeOK
	dec (hl)
	ld a,b
	jr BigNumMod_Again
BigNumMod_SizeOK:
	;; A = MSB of HL
	push hl
	 ld hl,bigN
	 ld c,(hl)
	 ld b,0
	 add hl,bc
	 ld l,(hl)
	 ld h,b
BigNumMod_BitLoop:
	;; Find how far left we can shift H.L before it becomes
	;; >= A
	 cp h
	 jr c,BigNumMod_BitLoopDone
	 jr z,BigNumMod_BitLoopDone
	 add hl,hl
	 jr c,BigNumMod_BitLoopDone
	 inc b
	 jr BigNumMod_BitLoop
BigNumMod_BitLoopDone:
	 dec b
	;; We want to subtract out N << (B + 8*(l(HL) - l(N) - 1))
	 pop hl
	ld a,(bigN)
	bit 3,b			; if B = 8, fold this into the byte shift
	jr z,BigNumMod_NoExtraShift
	dec a
	res 3,b
BigNumMod_NoExtraShift:
	cpl			; A = -l(N) or -l(N)-1
	add a,(hl)
	jr nc,BigNumMod_NoShiftSub	; if this doesn't carry, then
					; the number at HL isn't large
					; enough to perform the
					; subtraction we're talking
					; about...
	ld c,a
	push ix
	 ld ix,bigN
	 push hl
	  call BigNumShiftSub
	  pop hl
	 pop ix
	jr BigNumMod

BigNumMod_NoShiftSub:
	;; If we get to this point, then the lengths of the two
	;; numbers are equal.  Furthermore, B must have been less than
	;; 8, so unshifted N is already comparable to (HL).  Thus, at
	;; most one subtraction is needed now.
	push hl
	 ld c,(hl)
	 ld b,0
	 add hl,bc
	 ex de,hl
	 ld hl,bigN
	 add hl,bc
	 ld b,c
BigNumMod_CompareLoop:
	 ld a,(de)
	 cp (hl)
	 jr c,BigNumMod_Done
	 jr nz,BigNumMod_SubtractFinal
	 dec hl
	 dec de
	 djnz BigNumMod_CompareLoop
BigNumMod_Done:
	 pop hl
	ret
BigNumMod_SubtractFinal:
	 pop hl
	push ix
	 ld ix,bigN
	 call BigNumShiftSub0
	 pop ix
	ret


;;
;; BigNumShiftSub [UNTESTED]
;;
;;  Shift the number at IX left by C bytes and B bits, and subtract it
;;  from the number at HL.  Store the result at HL.
;;
;;  Input: HL -> big integer
;;         IX -> big integer
;;         C = byte count
;;         B = bit count
;;  Output: None
;;  Destroys: AF, BC, DE, HL

BigNumShiftSub:
	;; Shift number at IX left by C bytes and B bits, and
	;; subtract it from the number at HL (which is assumed to be
	;; larger.)
	ld a,b
	or a
	jr z,BigNumShiftSub_Aligned
	push bc
	 inc hl
	 ld b,0
	 add hl,bc
	 ex de,hl
	 pop bc
	ld c,(ix)		; C = bytes remaining
	inc ix
	ld l,0			; L = bits left over from previous byte
BigNumShiftSub_Loop:
	push bc
	 push af
	  ld a,(ix)
	  ld h,a
BigNumShiftSub_GetBits:
	  add hl,hl
	  djnz BigNumShiftSub_GetBits
	  ld l,a
	  pop af
	 ld a,(de)
	 sbc a,h
	 ld (de),a
	 inc de
	 inc ix
	 pop bc
	dec c
	jr nz,BigNumShiftSub_Loop
	;; last byte
	push af
	 ld h,0
BigNumShiftSub_GetBits2:
	 add hl,hl
	 djnz BigNumShiftSub_GetBits2
	 pop af
	ld a,(de)
	sbc a,h
	ld (de),a
	ret

BigNumShiftSub_Aligned:
	add hl,bc
BigNumShiftSub0:
	or a
	inc hl
	ld b,(ix)
BigNumShiftSub_AlignedLoop:
	ld a,(hl)
	sbc a,(ix+1)
	ld (hl),a
	inc hl
	inc ix
	djnz BigNumShiftSub_AlignedLoop
	ret
	
