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
;;;  Fields
;;;

;;; Warning: Pay close attention to the outputs for these routines --
;;; many are quite subtle.  (All of the weird, apparently accidental
;;; outputs noted here are actually used by TI -- I had to find them
;;; by trial and error.)

;;; While many of the routines below preserve A, TI's field searching
;;; routines, in general, do not.  At least one (FindAppSubField)
;;; intentionally returns something other than a page number.

;;; Like TI's routines, none of the following allow a field to span a page
;;; boundary.

;;
;; FindField
;;
;;  Search for a field.  Give up when we reach FF.
;;
;;  Input: AHL -> first type byte
;;         DE = field to search for
;;  Output: Z set and AHL -> first type byte if found
;;  Destroys: F, BC, OP1, ramCode

FindField_Skip:
	 pop af
	push de
	 call GetFieldSize
	 pop de
	add hl,bc
FindField:
	push af
	 call LoadAIndPaged
	 inc hl
	 cp d
	 jr nz,FindField_SkipMajor
	 pop af
	push af
	 call LoadAIndPaged
	 xor e
	 and 0f0h
	 jr nz,FindField_Skip
	 pop af
	dec hl
	cp a
	ret
FindField_SkipMajor:
	 inc a
	 jr nz,FindField_Skip
	 inc a
	 pop bc
	ld a,b
	ret


;;
;; FindAppField
;;
;;  Search for a field.  Give up when we reach anything other than 80,
;;  02, or 03 (those being the legal major types for application
;;  fields.)
;;
;;  Input: AHL -> first type byte
;;         DE = field to search for
;;  Output: Z set and AHL -> first type byte if found
;;  Destroys: F, BC, OP1, ramCode

FindAppField_Skip:
	 pop af
	push de
	 call GetFieldSize
	 pop de
	add hl,bc
FindAppField:
	push af
	 call LoadAIndPaged
	 inc hl
	 cp d
	 jr nz,FindAppField_SkipMajor
	 pop af
	push af
	 call LoadAIndPaged
	 xor e
	 and 0f0h
	 jr nz,FindAppField_Skip
	 pop af
	dec hl
	cp a
	ret
FindAppField_SkipMajor:
	 cp 80h
	 jr z,FindAppField_Skip
	 sub 02h
	 jr z,FindAppField_Skip
	 dec a
	 jr z,FindAppField_Skip
	 pop bc
	ld a,b
	ret


;;
;; FindSimpleGroupedField
;;
;;  Search for a field, but only within blocks of the same major field
;;  type (for example, public key blocks in the certificate, where all
;;  fields have major type 7.)
;; 
;;  Input: AHL -> first type byte
;;         DE = field to search for
;;  Output: Z and HL -> first type byte if found
;;  Destroys: [A], F, BC, OP1, ramCode

FindSimpleGroupedField_Skip:
	 pop af
	push de
	 call GetFieldSize
	 pop de
	add hl,bc
FindSimpleGroupedField:
	push af
	 call LoadAIndPaged
	 inc hl
	 cp d
	 jr nz,FindSimpleGroupedField_SkipMajor
	 pop af
	push af
	 call LoadAIndPaged
	 xor e
	 and 0f0h
	 jr nz,FindSimpleGroupedField_Skip
	 pop af
	dec hl
	cp a
	ret
FindSimpleGroupedField_SkipMajor:
	 pop bc
	ld a,b
	ret


;;
;; FindGroupedField
;;
;;  Search for a field, but only within blocks of the same major field
;;  type, and stop before a minor-type-1 field.  (For example, this
;;  routine is used to search for 073 fields following a given 071
;;  field; if you come to another 071 field or anything with major
;;  type != 7, you've gone too far.)
;; 
;;  Input: AHL -> first type byte
;;         DE = field to search for
;;  Output: Z and BC = length and HL -> first data byte if found
;;  Destroys: [A], F, OP1, ramCode

FindGroupedField_Skip:
	 pop af
	push de
	 call GetFieldSize
	 ld b,a
	 ld a,e
	 pop de
	cp e
	ret nz
	add hl,bc
FindGroupedField:
	push af
	 call LoadAIndPaged
	 inc hl
	 cp d
	 jr nz,FindGroupedField_SkipMajor
	 pop af
	push af
	 call LoadAIndPaged
	 xor e
	 and 0f0h
	 jr nz,FindGroupedField_Skip
	 pop af
	push de
	 call GetFieldSize
	 pop de
	cp a
	ret
FindGroupedField_SkipMajor:
	 pop bc
	ld a,b
	ret


;;
;; FindFirstCertificateField
;;
;;  Search for a field on the certificate page, starting at the
;;  beginning.  Give up when we reach FF.
;;
;;  Input: DE = field to search for
;;  Output: Z and HL -> first type byte if found
;;  Destroys: AF, BC, OP1, ramCode
;;  Protection: Must be disabled by caller

FindFirstCertificateField:
	call GetCertificateStart
	ld a,CERT_PAGE
	call LoadAIndPaged
	or a
	ret nz
	ld a,CERT_PAGE
	inc hl
	jp FindField


;;
;; FindNextCertificateField
;;
;;  Search for a field on the certificate page, skipping the current
;;  one.  Give up when we reach FF.
;;
;;  Input: HL -> first type byte
;;         DE = field to search for
;;  Output: Z and HL -> first type byte if found
;;  Destroys: AF, BC, OP1, ramCode
;;  Protection: Must be disabled by caller

FindNextCertificateField:
	ld a,CERT_PAGE
	push de
	 call GetFieldSize
	 pop de
	add hl,bc
	jp FindField


;;
;; FindSubField
;;
;;  Skip to the data section of this field, and search its subfields.
;;  Give up when we reach FF (don't bother checking if it's actually a
;;  subfield of the original field.)
;;
;;  Input: AHL -> first type byte
;;         DE = subfield to search for
;;  Output: Z and HL -> first type byte if found
;;  Destroys: [A], BC, OP1, ramCode

FindSubField:
	inc hl
	push de
	 call GetFieldSize
	 pop de
	jp FindField


;;
;; FindAppHeaderSubField and FindOSHeaderSubField
;;
;;  Give up immediately if (AHL) = FFh.  For FindOSHeaderSubField, give
;;  up if (AHL) = 0.  Otherwise, skip to the data section of this
;;  field, and search for a given subfield.  Give up when we reach any
;;  non-app field.
;;
;;  [The official version has a bug and gives unpredictable results
;;  when given an initial "nibble"-style field.]
;;
;;  Input: AHL -> first type byte
;;         DE = subfield to search for
;;  Output: Z set and HL -> first type byte if found
;;          A = (HL)
;;  Destroys: OP1, ramCode

FindOSHeaderSubField:
	push af
	 call LoadAIndPaged
	 or a
	 jr z,FindAppHeaderSubField_Invalid
	 inc a
	 jr nz,FindAppHeaderSubField_OK
FindAppHeaderSubField_Invalid:
	 pop af
FindAppHeaderSubField_RetNZ:
	or a
	ret nz
	cp 42
	ret
FindAppHeaderSubField:
	push af
	 call LoadAIndPaged
	 inc a
	 jr z,FindAppHeaderSubField_Invalid
FindAppHeaderSubField_OK:
	 pop af
	inc hl
	push bc
	 push de
	  call GetFieldSize
	  pop de
	 call FindAppField
	 jr nz,FindAppHeaderSubField_Failed
	 call LoadAIndPaged
	 cp a
	 pop bc
	ret
FindAppHeaderSubField_Failed:
	 call LoadAIndPaged
	 pop bc
	jr FindAppHeaderSubField_RetNZ


;;
;; GetFieldSize
;;
;;  Determine the length of a field and the address of its first data
;;  byte.
;;
;;  [The official version has a bug and returns garbage in BC when it
;;  is used with a "long"-style field.]
;;
;;  Input: A = page
;;         HL -> second type byte of the field
;;  Output: BC = length (0 for "long" fields)
;;          HL -> first data byte
;;          E = high 4 bits of the second type byte
;;  Destroys: OP1, ramCode

GetFieldSize:
	push af
	 call LoadAIndPaged
	 ld e,a
	 or 0f0h
	 inc a
	 jr z,GetFieldSize_Long
	 inc a
	 jr z,GetFieldSize_Word
	 ld b,0
	 inc a
	 jr z,GetFieldSize_Byte
	 add a,10h-3
	 ld c,a
	 jr GetFieldSize_Done
GetFieldSize_Long:
	 ld bc,4
	 add hl,bc
	 ld bc,0
	 jr GetFieldSize_Done
GetFieldSize_Word:
	 inc hl
	 pop af
	push af
	 call LoadAIndPaged
	 ld b,a
GetFieldSize_Byte:
	 inc hl
	 pop af
	push af
	 call LoadAIndPaged
	 ld c,a
GetFieldSize_Done:
	 inc hl
	 ld a,e
	 and 0fh
	 ld e,a
	 pop af
	ret

