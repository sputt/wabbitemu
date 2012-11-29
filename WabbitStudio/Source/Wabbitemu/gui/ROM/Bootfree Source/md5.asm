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
;;;  MD5 Calculation
;;;

;;; Note, the code below assumes that neither MD5_Hash (16 bytes) nor
;;; MD5_Buffer (64 bytes) spans a 256-byte boundary


;;
;; MD5Init
;;
;;  Reset the MD5 registers for a new computation.
;;
;;  Input: None
;;  Output: MD5_Length, MD5_Hash reset
;;  Destroys: None

MD5Init:
	push hl
	 push de
	  push bc
	   ld hl,MD5_Length
	   ld de,MD5_Length+1
	   ld bc,7
	   ld (hl),0
	   ldir
	   ld hl,MD5_InitialRegisters
	   ld de,MD5_Hash
	   ld bc,16
	   ldir
	   pop bc
	  pop de
	 pop hl
	ret

MD5_InitialRegisters:
	.DB 001h, 023h, 045h, 067h
	.DB 089h, 0ABh, 0CDh, 0EFh
	.DB 0FEh, 0DCh, 0BAh, 098h
	.DB 076h, 054h, 032h, 010h


;;
;; MD5Update
;;
;;  Add a chunk of data to the MD5 hash.
;;
;;  Input: MD5_Length, MD5_Hash, MD5_Buffer
;;         HL -> data to hash
;;         BC = number of bytes to hash
;;  Output: MD5_Length, MD5_Hash, MD5_Buffer
;;  Destroys: AF, BC, DE, HL, IX(!), MD5_AltRegs

MD5Update:
	ld a,b
	or c
	ret z
	ld de,(MD5_Length)

	;; we want de = (length/8 mod 64) + MD5_Buffer
	;; and a = length/2 mod 256
	srl d
	ld a,e
	rra
	ld d,a
	rrca
	rrca
	add a,low(MD5_Buffer)
	ld e,a
	ld a,d
	ld d,high(MD5_Buffer)
MD5Update_Loop:
	add a,4		; add one byte
	ldi
	jr c,MD5Update_Process	; if the addition carried, the length now = 0 mod 64
	jp pe,MD5Update_Loop	; if parity is clear (odd), BC now = 0

	add a,a
	ld (MD5_Length),a
	ld a,(MD5_Length+1)
	rra
	rlca
	ld (MD5_Length+1),a
	ret

MD5Update_Process:
	push hl
	 push bc

	  ;; A = bits 1-9 of the MD5 length
	  ;; (minus 256 = 512 bits = 64 bytes, which carried)
	  add a,a
	  ld hl,MD5_Length
	  ld (hl),a
	  inc hl
	  ld a,(hl)
	  rra
	  rlca
	  add a,2
	  ld (hl),a
	  jr nc,MD5Update_SkipCarry
MD5Update_CarryLoop:
	  ;; carry to higher bytes
	  inc hl
	  inc (hl)
	  jr z,MD5Update_CarryLoop
MD5Update_SkipCarry:
	  ;; Now process the current 64-byte block
	  ld hl,MD5_Hash
	  ld de,MD5_AltRegs
	  ld bc,16
	  ldir
	  call MD5_FourRounds
	  ld hl,MD5_AA
	  ld de,MD5_A
	  call Add32
	  inc hl
	  inc de
	  call Add32
	  inc hl
	  inc de
	  call Add32
	  inc hl
	  inc de
	  call Add32
	  pop bc
	 pop hl
	jr MD5Update


;; 
;; MD5Final
;;
;;  Complete the MD5 hash.
;;
;;  Input: MD5_Length, MD5_Hash, MD5_Buffer
;;  Output: MD5_Hash = completed hash
;;  Destroys: AF, BC, DE, HL, IX(!)
;;            MD5_Length, MD5_Buffer, MD5_AltRegs

MD5Final:
	ld hl,MD5_Length
	ld de,MD5_Temp
	ld bc,8
	ldir

	;; add 80 byte
	ld hl,MD5Final_Data
	ld bc,1
	call MD5Update
	;; and as many 00 bytes as needed to get 56 mod 64
	inc hl
MD5Final_PadLoop:
	ld a,(MD5_Length)
	cp 0c0h
	jr z,MD5Final_24mod32
MD5Final_ContinuePadding:
	dec hl
	inc c
	call MD5Update
	jr MD5Final_PadLoop
MD5Final_24mod32:
	ld a,(MD5_Length+1)
	and 1
	jr z,MD5Final_ContinuePadding

	ld hl,MD5_Temp
	ld bc,8
	jp MD5Update

MD5Final_Data:
	.db 80h, 00h


;;
;; Add32
;;
;;  Add two 32-bit integers.
;; 
;;  Input: HL -> first integer
;;         DE -> second integer
;;  Output: second integer increased by first
;;          HL -> last byte of first integer
;;          DE -> last byte of second integer

Add32:
	ld a,(de)
	add a,(hl)
	ld (de),a
	inc hl
	inc de
	ld a,(de)
	adc a,(hl)
	ld (de),a
	inc hl
	inc de
	ld a,(de)
	adc a,(hl)
	ld (de),a
	inc hl
	inc de
	ld a,(de)
	adc a,(hl)
	ld (de),a
	ret

#INCLUDE "md5tbl.asm"

#IFDEF SILVER
#INCLUDE "md5hw.asm"
#ELSE
#INCLUDE "md5sw.asm"
#ENDIF