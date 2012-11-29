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
;;;  Flash ROM
;;;

;;
;; FlashWE
;;
;;  Write-enable Flash.
;;
;;  Input: None
;;  Output: None
;;  Destroys: IFF, IM

FlashWE:
	push af
	 ld a,1
	 nop
	 nop
	 im 1
	 di
	 out (14h),a
	 pop af
	ret


;;
;; FlashWD
;;
;;  Write-disable Flash.
;;
;;  Input: None
;;  Output: None
;;  Destroys: IFF, IM

FlashWD:
	push af
	 xor a
	 nop
	 nop
	 im 1
	 di
	 out (14h),a
	 pop af
	ret


;;
;; WriteAByte
;;
;;  Write a byte to Flash; don't write to page 1E/3E/7E.
;; 
;;  Input: B = byte to write
;;         A = page
;;         DE = address
;;  Output: Z on success
;;  Destroys: AF, BC, DE, HL, OP1 (1), ramCode
;;  Protection: Must be disabled by caller

WriteAByteSafe:
	cp CERT_PAGE
	jr c,WriteAByteUnsafe
	or a
	ret


;;
;; ZeroToCertificate
;;
;;  Write a zero to page 1E/3E/7E.
;;
;;  Input: HL = address
;;  Output: Z on success
;;  Destroys: AF, BC, DE, HL, OP1 (1), ramCode
;;  Protection: Must be disabled by caller

ZeroToCertificate:
	ld a,CERT_PAGE
	ld b,0
	ex de,hl
	;; fall through

;;
;; WriteAByteUnsafe
;;
;;  Write a byte to Flash; allow writing to page 1E/3E/7E.
;;
;;  In general programs should use WriteAByte rather than this routine
;;  whenever possible.
;;
;;  Input: B = byte to write
;;         A = page
;;         DE = address
;;  Output: Z on success
;;  Destroys: AF, BC, DE, HL, OP1 (1), ramCode
;;  Protection: Must be disabled by caller

WriteAByteUnsafe:
	ld hl,OP1
	ld (hl),b
	ld bc,1
	jr WriteFlashUnsafe


;;
;; WriteFlashSafe
;;
;;  Write data to Flash; don't write to page 1E/3E/7E.
;;
;;  Input: A = page
;;         DE = address
;;         HL = data to write (in RAM)
;;         BC = byte count
;;  Output: Z on success
;;  Destroys: AF, BC, DE, HL, ramCode
;;  Protection: Must be disabled by caller

WriteFlashSafe:
	cp CERT_PAGE
	jr c,WriteFlashUnsafe
	or a
	ret


;;
;; WriteFlashUnsafe
;;
;;  Write data to Flash; allow writing to page 1E/3E/7E.  Some sanity
;;  checks are still performed: you can't write to RAM, and you can't
;;  write both page 1D/3D/7D and 1E/3E/7E in a single call to this
;;  routine.  This routine will allow you to write to the boot
;;  sector(s), which on a real calculator would trigger a reset.
;;
;;  In general programs should use WriteFlash rather than this routine
;;  whenever possible.
;;
;;  Input: A = page
;;         DE = address
;;         HL = data to write (in RAM)
;;         BC = byte count
;;  Output: Z on success
;;  Destroys: AF, BC, DE, HL, ramCode
;;  Protection: Must be disabled by caller

WriteFlashUnsafe:
 BEGIN_RAM_CODE()
rc_WriteFlash:
	out (6),a
	and RAM_PAGE
	jr nz,rc_WriteFlash_Done
	;needed for an unlock exploit
	;bit 7,d
	;jr nz,rc_WriteFlash_Done

	ld a,b
	or c
	jr z,rc_WriteFlash_Done

rc_WriteFlash_Loop:
	push bc
	 in a,(6)
	 ld b,a

	 ;; Send program command to Flash chip (3 write cycles)
	 ld a,2
	 out (6),a
	 ld a,0aah
	 ld (6aaah),a

	 ld a,1
	 out (6),a
	 ld a,55h
	 ld (5555h),a
	
	 ld a,2
	 out (6),a
	 ld a,0a0h
	 ld (6aaah),a

	 ld a,b
	 out (6),a
	 pop bc
	ld a,(hl)
	ld (de),a
	inc hl
	dec bc
	push bc
	 and 80h
	 ld b,a

rc_WriteFlash_Wait:
	 ;; Wait for the write to finish
	 ld a,(de)
	 xor b				; bit 7 = complement while writing,
	 jp p,rc_WriteFlash_Next	; correct when done
	 and 20h			; bit 5 = set on an error
	 jr z,rc_WriteFlash_Wait
	 ld a,(de)			; re-check to prevent a race condition
	 xor b
	 jp m,rc_WriteFlash_Error
	 jr rc_WriteFlash_Wait

rc_WriteFlash_Next:
	 inc de
	 pop bc
	ld a,b
	or c
	jr z,rc_WriteFlash_Done

	;; calculate next address
	; BuckeyeDude 2/17
	; there is an unlock exploit that writes to ram, by overwriting
	; the stack. For now were going to check if it just hit 0x80XX range
	; since you shouldn't write to ram with this anyway, I think it 
	; should be fine
	ld a, $80
	cp d
	jr nz,rc_WriteFlash_Loop
	ld a,d
	sub 40h
	ld d,a
	in a,(6)
	inc a
	out (6),a
	cp CERT_PAGE			; make sure we don't flip to the
	jr c,rc_WriteFlash_Loop		; certificate page (or beyond)
	push bc
rc_WriteFlash_Error:
	 pop bc
	or 0ffh
rc_WriteFlash_Done:
	ld a,0f0h		; return chip to read mode
	ld (3fffh),a
	ld a,BOOT_PAGE
	out (6),a
	ret
 END_RAM_CODE()


;;
;; EraseFlash
;;
;;  Erase a sector of Flash.
;;
;;  In general programs should use EraseFlashPage rather than this
;;  routine whenever possible.
;;
;;  Input: A = page
;;         HL = address
;;  Output: Z on success
;;  Destroys: AF, ramCode
;;  Protection: Must be disabled by caller

EraseFlash:
 BEGIN_RAM_CODE()
rc_EraseFlash:
	push af
	 and RAM_PAGE
	 jr nz,rc_EraseFlash_Error1
	 bit 7,h
	 jr nz,rc_EraseFlash_Error1

	 ;; Send erase command to Flash chip (6 write cycles)
	 ld a,2
	 out (6),a
	 ld a,0aah
	 ld (6aaah),a

	 ld a,1
	 out (6),a
	 ld a,55h
	 ld (5555h),a
	
	 ld a,2
	 out (6),a
	 ld a,80h
	 ld (6aaah),a

	 ld a,2
	 out (6),a
	 ld a,0aah
	 ld (6aaah),a

	 ld a,1
	 out (6),a
	 ld a,55h
	 ld (5555h),a
	
	 pop af
	out (6),a
	ld (hl),30h
rc_EraseFlash_Verify:
	ld a,(hl)
	bit 7,a			; check if done
	jr nz,rc_EraseFlash_OK
	bit 5,a			; check for an error
	jr z,rc_EraseFlash_Verify
	ld a,(hl)		; check again...
	bit 7,a
	jr z,rc_EraseFlash_Error
rc_EraseFlash_OK:
	xor a
rc_EraseFlash_Done:
	ld a,0f0h
	ld (0),a
	ld a,BOOT_PAGE
	out (6),a
	ret
rc_EraseFlash_Error1:
	 pop af
rc_EraseFlash_Error:
	or 0ffh
	jr rc_EraseFlash_Done
 END_RAM_CODE()


;;
;; EraseFlashPage
;;
;;  Erase a sector of Flash (fill it with all FFs.)  A sector is
;;  usually 65536 bytes; pages 0-3 form sector 0, pages 4-7 form
;;  sector 1, and so forth.  The last four pages are special: pages
;;  1C/3C/7C and 1D/3D/7D form a 32k sector, page 1E/3E/7E is composed
;;  of two 8k sectors, and page 1F/3F/7F forms a 16k sector.
;;
;;  This routine cannot be used to erase the certificate page.
;;
;;  Input: A = page
;;  Output: Z on success
;;  Destroys: AF, HL, ramCode
;;  Protection: Must be disabled by caller

EraseFlashPage:
	cp CERT_PAGE
	jr z,EraseFlashPage_Error
	ld hl,4000h
	jr EraseFlash
EraseFlashPage_Error:
	or a
	ret


;;
;; EraseCertificateSector
;;
;;  Erase one of the sectors of the certificate page.
;;
;;  Input: HL = address (must equal 4000h or 6000h)
;;  Output: None
;;  Destroys: ramCode
;;  Protection: Must be disabled by caller

EraseCertificateSector:
	push af
	 ld a,l
	 or a
	 jr nz,EraseCertificateSector_Error
	 ld a,h
	 and 0dfh
	 cp 40h
	 jr nz,EraseCertificateSector_Error
	 ld a,CERT_PAGE
	 call EraseFlash
EraseCertificateSector_Error:
	 pop af
	ret
