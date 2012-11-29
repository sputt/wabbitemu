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
;;;  Miscellaneous Boot Routines
;;;

;;; Unknown B_CALL
UnknownBC:
	ret

;;
;; GetBootVer
;;
;;  Return boot version.
;; 
;;  Input: None
;;  Output: A = major version
;;          B = minor version
;;  Destroys: None

GetBootVer:
	ld a,BOOT_MAJOR
	ld b,BOOT_MINOR
	ret


;;
;; GetHWVer
;;
;;  Return hardware version.
;; 
;;  Input: None
;;  Output: A = version
;;  Destroys: None

GetHWVer:
	ld a,HW_VERSION
	ret


;;
;; ProdNrPart1
;;
;;  Return the product ID and hardware version.
;;
;;  Input: None
;;  Output: E = product ID, D = hardware version * 16
;;  Destroys: [HL]

ProdNrPart1:
	ld de,HW_VERSION * 4096 + PRODUCT_ID
	ret


;;
;; DoNothing
;;
;;  ...
;;
;;  Input: None
;;  Output: None
;;  Destroys: None

DoNothing:
	ret


;;
;; XorA
;;
;;  Compute the XOR of A with itself.
;;
;;  Input: A = number
;;  Ouptut: A = A xor A
;;  Destroys: F

XorA:
	xor a
	ret


;;
;; DispBootVer
;;
;;  Display boot version.
;; 
;;  Input: None
;;  Output: None
;;  Destroys: HL

DispBootVer:
#IFDEF TI73
	ld hl,2
#ELSE
	ld hl,0
#ENDIF
	ld (curRow),hl
DispBootVerX:
	ld hl,NameStr
	call PutS
	ld hl,VersionStr
	jp PutS

NameStr:
.db "BootFree ",0
