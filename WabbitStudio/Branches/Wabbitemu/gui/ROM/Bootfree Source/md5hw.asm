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
;;;  MD5 Support Routines (Hardware Version)
;;;

;;
;; MD5_FourRounds
;;
;;  Perform the four rounds of the main MD5 calculation.
;;
;;  Input: MD5_Hash
;;  Output: MD5_Hash
;;  Destroys: AF, B, DE, HL, IX

MD5_FourRounds:
	xor a
	out (1Fh),a
	ld hl,MD5_Table
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop1:
	call MD5_Operation
	djnz MD5_FourRounds_Loop1
	ld a,1
	out (1Fh),a
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop2:
	call MD5_Operation
	djnz MD5_FourRounds_Loop2
	ld a,2
	out (1Fh),a
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop3:
	call MD5_Operation
	djnz MD5_FourRounds_Loop3
	ld a,3
	out (1Fh),a
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop4:
	call MD5_Operation
	djnz MD5_FourRounds_Loop4
	ret


;;
;; MD5_Operation
;;
;;  Perform an MD5 operation.
;;
;;  Input: HL -> MD5_Table entry
;;         IX -> MD5_RegsTable entry
;;  Output: HL -> next MD5_Table entry
;;          IX -> next MD5_RegsTable entry
;;  Destroys: AF, DE, MD5_Hash

MD5_Operation:
	push bc
	 push hl
	  ld l,(ix+0)
	  ld h,high(MD5_Hash)
	  ld c,18h		; port 18h: initial a
	  outi
	  outi
	  outi
	  outi

	  ld l,(ix+1)
	  inc c			; port 19h: b
	  outi
	  outi
	  outi
	  outi

	  ld l,(ix+2)
	  inc c			; port 1ah: c
	  outi
	  outi
	  outi
	  outi

	  ld l,(ix+3)
	  inc c			; port 1bh: d
	  outi
	  outi
	  outi
	  outi

	  pop hl
	 push hl
	  ld l,(hl)
	  ld h,high(MD5_Buffer)
	  inc c			; port 1ch: X (or T)
	  outi
	  outi
	  outi
	  outi

	  pop hl
	 inc hl
	 inc c			; port 1dh: T (or X)
	 outi
	 outi
	 outi
	 outi

	 inc c			; port 1eh: shift
	 outi
	 push hl

	;; Store the result back into a, and advance to the next entry
	;; in the MD5_RegsTable.

	  ld l,(ix+0)
	  ld h,high(MD5_Hash)
	  ld c,1Ch
	  ini
	  inc c
	  ini
	  inc c
	  ini
	  inc c
	  ini

	  dec ix
	  pop hl
	 pop bc
	ret
