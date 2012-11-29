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
;;;  Certificate Data Management
;;;

;;
;; GetCertificateStart
;;
;;  Get the start of the certificate.
;;
;;  Input: None
;;  Output: HL = 4000h or 6000h
;;  Destroys: OP1, ramCode
;;  Protection: Must be disabled by caller

GetCertificateStart:
	push af
	 ld a,CERT_PAGE
	 ld hl,4000h
	 call LoadAIndPaged
	 or a
	 jr z,GetCertificateStart_4000
	 ld h,60h
GetCertificateStart_4000:
	 pop af
	ret


;;
;; GetCertificateEnd
;;
;;  Get the end of the certificate.
;;
;;  Input: None
;;  Output: HL -> end
;;  Destroys: OP1, ramCode
;;  Protection: Must be disabled by caller

GetCertificateEnd:
	push af
	 push bc
	  push de
	   ld de,0fff0h
	   call FindFirstCertificateField
	   pop de
	  pop bc
	 pop af
	ret


;;
;; GetCalcSerial
;;
;;  Retrieve the main part of the calculator ID.
;;
;;  Input: None
;;  Output: Z set and OP4 contains the ID if found; B = length of ID
;;  Destroys: AF, DE, HL
;;  Protection: Will be disabled and reenabled
;;  Interrupts: IFF preserved, IM set to 1

GetCalcSerial:
	ld a,i
	push af
	 call FlashWE
	 ld de,0A10h
	 call FindFirstCertificateField
	 jr nz,GetCalcSerial_Fail
	 inc hl
	 ld d,c
	 call GetFieldSize
	 ld e,d
	 ld d,c
	 push de
	  ld de,OP4
	  call FlashToRAM2
	  cp a
	  pop bc
GetCalcSerial_Fail:
	 call FlashWD
	 pop de
	push af
	 bit 2,e
	 jr z,GetCalcSerial_DI
	 ei
GetCalcSerial_DI:
	 pop af
	ret


;;
;; CheckOSValid
;;
;;  Check if the OS has been marked valid.
;;
;;  Input: None
;;  Output: Z if validated
;;  Destroys: AF, OP1, ramCode
;;  Protection: Must be disabled by caller

CheckOSValid:
	push hl
	 push de
	  call GetCertificateStart
	  ld de,1fe0h
	  add hl,de
	  ld a,CERT_PAGE
	  call LoadAIndPaged
	  and 1
	  pop de
	 pop hl
	ret


;;
;; MarkOSValid
;;
;;  Mark the current OS as valid.
;;
;;  Input: None
;;  Output: None
;;  Destroys: AF, OP1, ramCode
;;  Protection: Must be disabled by caller

MarkOSValid:
	push hl
	 push de
	  push bc
	   call GetCertificateStart
	   ld de,1fe0h
	   add hl,de
	   ld a,CERT_PAGE
	   call LoadAIndPaged
	   and 0feh
	   ld b,a
	   ld a,CERT_PAGE
	   call WriteAByteUnsafe
	   pop bc
	  pop de
	 pop hl
	ret

;;
;; MarkOSInvalid
;;
;;  Mark the current OS as invalid.
;;
;;  Input: None
;;  Output: None
;;  Destroys: AF, OP1, ramCode
;;  Protection: Must be disabled by caller

MarkOSInvalid:
	push hl
	 push de
	  push bc
	   call GetCertificateStart
	   ld de,1fe0h
	   add hl,de
	   ld a,CERT_PAGE
	   call LoadAIndPaged
	   or 1
	   ld b,a
	   ld a,CERT_PAGE
	   call WriteAByteUnsafe
	   pop bc
	  pop de
	 pop hl
	ret

;;
;; IsAppFreeware
;;
;;  Checks whether the current application is freeware or not.
;;
;;  Input: (appID + 2) is key
;;  Output: Z if is freeware. NZ otherwise
;;  Destroys: HL, DE

IsAppFreeware:
	ld hl,(appID + 2)
#ifdef TI73
	ld de,$0201
#else
	ld de,$0401
#endif
	or a
	sbc hl,de
#if HW_VERSION >= 2
	ret z
	add hl,de
	ld de,$0A01
	or a
	sbc hl,de
#endif
	ret