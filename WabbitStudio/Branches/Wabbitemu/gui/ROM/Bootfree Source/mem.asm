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
;;;  Memory
;;;

;;
;; LoadAIndPaged
;;
;;  Load A from (AHL).
;;
;;  Input: A = page
;;         HL = address
;;  Output: A = data
;;  Destroys: F, [OP1 (1)], ramCode

LoadAIndPaged:
 BEGIN_RAM_CODE
rc_LoadAIndPaged:
	and FLASH_MASK
	out (6),a
	ld a,(hl)
	push af
	 ld a,BOOT_PAGE
	 out (6),a
	 pop af
	ret
 END_RAM_CODE


;;
;; FlashToRAM2
;;
;;  Copy BC bytes from AHL to DE.
;;
;;  Input: A = page (must be in Flash; A = 0 copies from RAM)
;;         HL = source address
;;         DE = destination address
;;         BC = byte count
;;  Output: AHL += BC
;;          DE += BC
;;          BC = 0
;;  Destroys: ramCode, [A]

FlashToRAM2:
 BEGIN_RAM_CODE
rc_FlashToRAM:
	and FLASH_MASK
	out (6),a
	or a
	jr z,rc_FlashToRAM_FromRAM
	ld a,b
	or c
	jr z,rc_FlashToRAM_Done
rc_FlashToRAM_Loop:
	ldi
	jp po,rc_FlashToRAM_Done
	bit 7,h
	jr z,rc_FlashToRAM_Loop
	ld a,h
	sub 40h
	ld h,a
	in a,(6)
	inc a
	out (6),a
	jr rc_FlashToRAM_Loop

rc_FlashToRAM_FromRAM:
	ld a,b
	or c
	jr z,rc_FlashToRAM_Done
	ldir
rc_FlashToRAM_Done:
	in a,(6)
	push af
	 ld a,BOOT_PAGE
	 out (6),a
	 pop af
	ret
 END_RAM_CODE