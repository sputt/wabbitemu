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
;;;  Execution Protection
;;;

;;
;; SetAppLimit
;;
;;  Set the app execution limit.
;;
;;  Input: A = first disallowed app page
;;  Output: None
;;  Destroys: AF, B, HL
;;  Protection: Must be disabled by caller	
	
#IFDEF TI83PBE
SetAppLimit:
	di
	sub 7
	ret c
	ld hl,0
	jr z,SetAppLimit_Page7
	ld b,a
SetAppLimit_Loop:
	add hl,hl
	inc l
	djnz SetAppLimit_Loop
SetAppLimit_Page7:
	ld a,1
	out (5),a
	ld a,h
	nop
	nop
	im 1
	di
	out (16h),a

	xor a
	out (5),a
	ld a,l
	nop
	nop
	im 1
	di
	out (16h),a
	ret
#ENDIF

#IFDEF SILVER
SetAppLimit:
	di
	nop
	nop
	im 1
	di
	out (23h),a
	ret
#ENDIF