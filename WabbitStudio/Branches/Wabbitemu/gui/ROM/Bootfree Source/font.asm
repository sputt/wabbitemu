;;; -*- mode: TI-Asm; ti-asm-byte-directive: " DB "; ti-asm-number-style: zmasm -*-
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
;;;  Font
;;;

;;
;; Load_LFont
;;
;;  Load a large font bitmap.
;;
;;  Input: HL = character * 8
;;  Output: HL -> bitmap (stored in lFont_record)
;;  Destroys: [A], F, BC, DE

Load_LFont:
	ld de,LargeFont+1
	add hl,de
	ld de,lFont_record
	push de
	 ld bc,7
	 ldir
	 pop hl
	ret


;;
;; Load_LFontV
;;
;;  Load a large font bitmap in variable-width format.
;;
;;  Input: HL = character * 8
;;  Output: HL -> bitmap (stored in lFont_record)
;;  Destroys: AF, BC, DE

Load_LFontV:
	ld de,lFont_record
	ld a,6
	ld (de),a
	push de
	 inc de
	 bit fracTallLFont,(iy+fontFlags)
	 jr nz,Load_LFontV_Tall
	 push de
Load_LFontV_Dest:
	  ld de,LargeFont+1
	  add hl,de
	  pop de
	 ld b,7
Load_LFontV_Loop:
	 ld a,(hl)
	 add a,a
	 ld (de),a
	 inc hl
	 inc de
	 djnz Load_LFontV_Loop
	 xor a
	 ld (de),a
	 pop hl
	ret

Load_LFontV_Tall:
	 xor a
	 ld (de),a
	 inc de
	 ld (de),a
	 inc de
	 ld (de),a
	 inc de
	 push de
	  ld d,a
	  ld e,a
	  ld (lFont_record+12),de
	  ld (lFont_record+14),a
	  jr Load_LFontV_Dest

#IFDEF TI73
Load_LFont2 .EQU Load_LFont
#ELSE
Load_LFont2 .EQU Load_LFontV
#ENDIF

LargeFont:

;;; 00
.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010100b
.db 00010100b
.db 00011111b
.db 00000101b
.db 00000101b
.db 00000111b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010110b
.db 00011101b
.db 00011001b
.db 00011001b
.db 00011001b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010011b
.db 00010010b
.db 00010010b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010001b
.db 00010001b
.db 00001010b
.db 00001010b
.db 00000100b

;;; 04

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010001b
.db 00010101b
.db 00010101b
.db 00010101b
.db 00001010b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000100b
.db 00000110b
.db 00011111b
.db 00000110b
.db 00000100b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00010001b
.db 00001010b
.db 00001010b
.db 00001010b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00001010b
.db 00001010b
.db 00001010b
.db 00010001b
.db 00001010b
.db 00000100b

;;; 08

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000101b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00010100b
.db 00001000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001010b
.db 00000100b
.db 00001010b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001110b
.db 00001010b
.db 00001110b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000100b
.db 00001110b
.db 00000100b
.db 00000000b
.db 00000000b

;;; 0C

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000100b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011100b
.db 00001000b
.db 00001000b
.db 00001000b

.db 6
;;;SPRITE 8x7x1
.db 00000111b
.db 00000010b
.db 00000001b
.db 00000111b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00001101b
.db 00001100b
.db 00001110b
.db 00001100b
.db 00001100b
.db 00011100b

;;; 10

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00000001b
.db 00000010b
.db 00000010b
.db 00011010b
.db 00001010b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00000011b
.db 00011001b
.db 00000001b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00000100b
.db 00001000b
.db 00011100b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000001b
.db 00000010b
.db 00000100b
.db 00001010b
.db 00010010b
.db 00011111b

;;; 14

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010100b
.db 00011100b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00010100b
.db 00011000b
.db 00010000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00001000b
.db 00001000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00000110b
.db 00011000b
.db 00000110b
.db 00010001b
.db 00001100b
.db 00000011b

;;; 18

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000010b
.db 00011111b
.db 00000100b
.db 00011111b
.db 00001000b

.db 6
;;;SPRITE 8x7x1
.db 00010000b
.db 00001100b
.db 00000011b
.db 00001100b
.db 00010001b
.db 00000110b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001111b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001110b
.db 00011011b
.db 00011111b
.db 00011000b
.db 00001111b

;;; 1C

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000100b
.db 00000010b
.db 00011111b
.db 00000010b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00010111b
.db 00010101b
.db 00010101b
.db 00010101b
.db 00010101b
.db 00010101b
.db 00010111b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001110b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b
.db 00000100b

;;; 20

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001110b
.db 00001110b
.db 00000100b
.db 00000100b
.db 00000000b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00011011b
.db 00011011b
.db 00010010b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00001010b
.db 00011111b
.db 00001010b
.db 00011111b
.db 00001010b
.db 00000000b

;;; 24

.db 6
;;;SPRITE 8x7x1
.db 00010100b
.db 00010100b
.db 00011100b
.db 00000100b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00001101b
.db 00010101b
.db 00011010b
.db 00000100b
.db 00001011b
.db 00010101b
.db 00010110b

.db 6
;;;SPRITE 8x7x1
.db 00001100b
.db 00010100b
.db 00010100b
.db 00001001b
.db 00010101b
.db 00010011b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000110b
.db 00000110b
.db 00000100b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

;;; 28

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000010b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000100b
.db 00011111b
.db 00001110b
.db 00011111b
.db 00000100b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000100b
.db 00000100b
.db 00011111b
.db 00000100b
.db 00000100b

;;; 2C

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011000b
.db 00001000b
.db 00010000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011111b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011000b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00000010b
.db 00000010b
.db 00000100b
.db 00001000b
.db 00001000b
.db 00010000b

;;; 30

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010101b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00000001b
.db 00000110b
.db 00001000b
.db 00010000b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00000001b
.db 00000110b
.db 00000001b
.db 00010001b
.db 00001110b

;;; 34

.db 6
;;;SPRITE 8x7x1
.db 00001001b
.db 00001001b
.db 00010001b
.db 00010001b
.db 00011111b
.db 00000001b
.db 00000001b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00010000b
.db 00010000b
.db 00001110b
.db 00000001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001000b
.db 00010000b
.db 00011110b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00000001b
.db 00000010b
.db 00000010b
.db 00000100b
.db 00000100b
.db 00001000b

;;; 38

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010001b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010001b
.db 00001111b
.db 00000001b
.db 00000010b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011000b
.db 00011000b
.db 00000000b
.db 00011000b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011000b
.db 00011000b
.db 00000000b
.db 00001000b
.db 00011000b

;;; 3C

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000011b
.db 00001100b
.db 00010000b
.db 00001100b
.db 00000011b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011111b
.db 00000000b
.db 00011111b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011000b
.db 00000110b
.db 00000001b
.db 00000110b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010011b
.db 00000011b
.db 00000110b
.db 00000100b
.db 00000000b
.db 00000100b

;;; 40

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010011b
.db 00010101b
.db 00010011b
.db 00010000b
.db 00001111b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00001010b
.db 00010001b
.db 00011111b
.db 00010001b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00011110b
.db 00001001b
.db 00001001b
.db 00001110b
.db 00001001b
.db 00001001b
.db 00011110b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010000b
.db 00010000b
.db 00010000b
.db 00010001b
.db 00001110b

;;; 44

.db 6
;;;SPRITE 8x7x1
.db 00011110b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00011110b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00001001b
.db 00001000b
.db 00001110b
.db 00001000b
.db 00001001b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00001001b
.db 00001000b
.db 00001110b
.db 00001000b
.db 00001000b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010000b
.db 00010011b
.db 00010001b
.db 00010001b
.db 00001111b

;;; 48

.db 6
;;;SPRITE 8x7x1
.db 00011001b
.db 00001001b
.db 00001001b
.db 00001111b
.db 00001001b
.db 00001001b
.db 00011001b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000011b
.db 00000001b
.db 00000001b
.db 00000001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00011001b
.db 00001001b
.db 00001010b
.db 00001100b
.db 00001010b
.db 00001001b
.db 00011001b

;;; 4C

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00001000b
.db 00001000b
.db 00001000b
.db 00001000b
.db 00001001b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00010001b
.db 00011011b
.db 00011111b
.db 00010101b
.db 00010001b
.db 00010001b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00010001b
.db 00011001b
.db 00011101b
.db 00010111b
.db 00010011b
.db 00010001b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

;;; 50

.db 6
;;;SPRITE 8x7x1
.db 00011110b
.db 00001001b
.db 00001001b
.db 00001110b
.db 00001000b
.db 00001000b
.db 00011100b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010101b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00011110b
.db 00001001b
.db 00001001b
.db 00001110b
.db 00001010b
.db 00001001b
.db 00011001b

.db 6
;;;SPRITE 8x7x1
.db 00001111b
.db 00010001b
.db 00010000b
.db 00001110b
.db 00000001b
.db 00010001b
.db 00011110b

;;; 54

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00010101b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001010b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010101b
.db 00011111b
.db 00011011b
.db 00010001b

;;; 58

.db 6
;;;SPRITE 8x7x1
.db 00010001b
.db 00001010b
.db 00001010b
.db 00000100b
.db 00001010b
.db 00001010b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00010001b
.db 00010001b
.db 00001010b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00010001b
.db 00000010b
.db 00000100b
.db 00001000b
.db 00010001b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010001b
.db 00011111b
.db 00010001b
.db 00010001b
.db 00001110b

;;; 5C

.db 6
;;;SPRITE 8x7x1
.db 00010000b
.db 00001000b
.db 00001000b
.db 00000100b
.db 00000010b
.db 00000010b
.db 00000001b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00011100b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00010001b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011111b

;;; 60

.db 6
;;;SPRITE 8x7x1
.db 00001100b
.db 00001100b
.db 00000100b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001100b
.db 00000010b
.db 00001110b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00011000b
.db 00001000b
.db 00001110b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00011110b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010000b
.db 00010001b
.db 00001110b

;;; 64

.db 6
;;;SPRITE 8x7x1
.db 00000110b
.db 00000010b
.db 00001110b
.db 00010010b
.db 00010010b
.db 00010010b
.db 00001111b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00011111b
.db 00010000b
.db 00001111b

.db 6
;;;SPRITE 8x7x1
.db 00000011b
.db 00000100b
.db 00000100b
.db 00001111b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001111b
.db 00010001b
.db 00001111b
.db 00000001b
.db 00011110b

;;; 68

.db 6
;;;SPRITE 8x7x1
.db 00011000b
.db 00001000b
.db 00001010b
.db 00001101b
.db 00001001b
.db 00001001b
.db 00011001b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00000000b
.db 00001100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00000000b
.db 00000011b
.db 00000001b
.db 00000001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00011000b
.db 00001000b
.db 00001001b
.db 00001010b
.db 00001100b
.db 00001010b
.db 00011001b

;;; 6C

.db 6
;;;SPRITE 8x7x1
.db 00001100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011010b
.db 00010101b
.db 00010101b
.db 00010101b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010110b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00011001b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

;;; 70

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011110b
.db 00001001b
.db 00001110b
.db 00001000b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001111b
.db 00010010b
.db 00001110b
.db 00000010b
.db 00000011b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010110b
.db 00001001b
.db 00001000b
.db 00001000b
.db 00011100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001111b
.db 00010000b
.db 00001110b
.db 00000001b
.db 00011110b

;;; 74

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00001000b
.db 00011110b
.db 00001000b
.db 00001000b
.db 00001001b
.db 00000110b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010011b
.db 00010010b
.db 00010010b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010001b
.db 00010001b
.db 00001010b
.db 00001010b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010001b
.db 00010101b
.db 00010101b
.db 00010101b
.db 00001010b

;;; 78

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010001b
.db 00001010b
.db 00000100b
.db 00001010b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010001b
.db 00001010b
.db 00000100b
.db 00001000b
.db 00010000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011111b
.db 00000010b
.db 00000100b
.db 00001000b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00000110b
.db 00000100b
.db 00000100b
.db 00001000b
.db 00000100b
.db 00000100b
.db 00000110b

;;; 7C

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00001100b
.db 00000100b
.db 00000100b
.db 00000010b
.db 00000100b
.db 00000100b
.db 00001100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001001b
.db 00010101b
.db 00010010b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00011111b
.db 00010001b
.db 00011111b
.db 00010001b
.db 00011111b
.db 00011111b

;;; 80

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00001000b
.db 00010100b
.db 00010100b
.db 00001000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00001000b
.db 00011000b
.db 00001000b
.db 00011100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011100b
.db 00000100b
.db 00001000b
.db 00011100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011100b
.db 00001000b
.db 00000100b
.db 00011100b

;;; 84

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00010100b
.db 00010100b
.db 00011100b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011100b
.db 00011000b
.db 00000100b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00001100b
.db 00011000b
.db 00010100b
.db 00001000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011100b
.db 00000100b
.db 00001000b
.db 00001000b

;;; 88

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00001100b
.db 00011100b
.db 00010100b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00001000b
.db 00010100b
.db 00001100b
.db 00011000b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000100b
.db 00001010b
.db 00001010b
.db 00011111b
.db 00010001b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00001010b
.db 00001010b
.db 00011111b
.db 00010001b
.db 00010001b

;;; 8C

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000100b
.db 00001010b
.db 00001010b
.db 00011111b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00000100b
.db 00001010b
.db 00001010b
.db 00011111b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00000010b
.db 00001100b
.db 00000010b
.db 00001110b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00001100b
.db 00000010b
.db 00001110b
.db 00010010b
.db 00001101b

;;; 90

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00001100b
.db 00000010b
.db 00001110b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00001100b
.db 00000010b
.db 00001110b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00011111b
.db 00001000b
.db 00001110b
.db 00001000b
.db 00001001b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00011111b
.db 00001000b
.db 00001110b
.db 00001000b
.db 00001001b
.db 00011111b

;;; 94

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00011111b
.db 00001000b
.db 00001110b
.db 00001000b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00011111b
.db 00001000b
.db 00001110b
.db 00001000b
.db 00001001b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000100b
.db 00001110b
.db 00010001b
.db 00011111b
.db 00010000b
.db 00001111b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00001110b
.db 00010001b
.db 00011111b
.db 00010000b
.db 00001111b

;;; 98

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000000b
.db 00001110b
.db 00010011b
.db 00011100b
.db 00001111b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00011111b
.db 00010000b
.db 00001111b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00001110b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00001110b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

;;; 9C

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000000b
.db 00001110b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00001110b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000100b
.db 00000000b
.db 00001100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00000000b
.db 00001100b
.db 00000100b
.db 00000100b
.db 00001110b

;;; A0

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000000b
.db 00001100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00001100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00010000b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

;;; A4

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000100b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00001110b

;;; A8

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000100b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

;;; AC

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000000b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000010b
.db 00000100b
.db 00000000b
.db 00010011b
.db 00010010b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00001000b
.db 00000100b
.db 00000000b
.db 00010011b
.db 00010010b
.db 00010010b
.db 00001101b

;;; B0

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000000b
.db 00010011b
.db 00010010b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00001010b
.db 00000000b
.db 00010011b
.db 00010010b
.db 00010010b
.db 00010010b
.db 00001101b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010000b
.db 00010000b
.db 00010000b
.db 00001001b
.db 00011110b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010000b
.db 00001001b
.db 00011110b

;;; B4

.db 6
;;;SPRITE 8x7x1
.db 00000101b
.db 00001010b
.db 00010001b
.db 00011001b
.db 00010101b
.db 00010011b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00000101b
.db 00001010b
.db 00000000b
.db 00010110b
.db 00001001b
.db 00001001b
.db 00011001b

.db 6
;;;SPRITE 8x7x1
.db 00010000b
.db 00001000b
.db 00000100b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00000010b
.db 00000100b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

;;; B8

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00001010b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00000000b
.db 00000100b
.db 00001100b
.db 00011000b
.db 00011001b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00000000b
.db 00000100b
.db 00000100b
.db 00001110b
.db 00001110b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001101b
.db 00010010b
.db 00010010b
.db 00010010b
.db 00001101b

;;; BC

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000110b
.db 00001001b
.db 00001110b
.db 00001001b
.db 00010110b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001001b
.db 00010101b
.db 00000101b
.db 00000010b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00001010b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00000110b
.db 00001000b
.db 00000100b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00001110b

;;; C0

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001111b
.db 00010000b
.db 00001110b
.db 00010000b
.db 00001111b

.db 6
;;;SPRITE 8x7x1
.db 00000111b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000111b

.db 6
;;;SPRITE 8x7x1
.db 00010000b
.db 00001000b
.db 00000100b
.db 00000100b
.db 00001010b
.db 00001010b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00010010b
.db 00010010b
.db 00010110b
.db 00011001b
.db 00010000b

;;; C4

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011111b
.db 00001010b
.db 00001010b
.db 00001010b
.db 00011011b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001110b
.db 00010001b
.db 00010001b
.db 00011110b
.db 00010000b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00010001b
.db 00001000b
.db 00000100b
.db 00001000b
.db 00010001b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00001111b
.db 00010010b
.db 00010010b
.db 00010010b
.db 00001100b

;;; C8

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011111b
.db 00010100b
.db 00000100b
.db 00000100b
.db 00000011b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000100b
.db 00001110b
.db 00010101b
.db 00010101b
.db 00001110b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00001010b
.db 00010001b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00000000b
.db 00010001b
.db 00001010b
.db 00000100b
.db 00001010b
.db 00010001b

;;; CC

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00000000b
.db 00010001b
.db 00001010b
.db 00000100b
.db 00001000b
.db 00010000b

.db 6
;;;SPRITE 8x7x1
.db 00000101b
.db 00000010b
.db 00000101b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00010101b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000100b
.db 00001100b
.db 00011111b
.db 00001100b
.db 00000100b

;;; D0

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00011111b
.db 00010001b
.db 00010001b
.db 00010001b
.db 00011111b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000001b
.db 00000010b
.db 00000010b
.db 00000100b
.db 00001000b
.db 00001000b
.db 00010000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00001110b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00000100b
.db 00001000b
.db 00011100b
.db 00000000b
.db 00000000b
.db 00000000b

;;; D4

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010100b
.db 00011100b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00001000b
.db 00000100b
.db 00011100b
.db 00000000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011000b
.db 00010100b
.db 00010100b
.db 00011011b
.db 00000110b
.db 00000101b
.db 00000010b

.db 6

#IFDEF TI73
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00010001b
.db 00011111b
#ELSE
;;;SPRITE 8x7x1
.db 00000000b
.db 00000001b
.db 00000000b
.db 00000110b
.db 00000010b
.db 00000100b
.db 00001110b
#ENDIF

;;; D8

.db 6
#IFDEF TI73
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000100b
.db 00001000b
.db 00011111b
.db 00001000b
.db 00000100b
#ELSE
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00000000b
.db 00011110b
.db 00001001b
.db 00001110b
.db 00011000b
#ENDIF

.db 6
#IFDEF TI73
;;;SPRITE 8x7x1
.db 00001110b
.db 00010001b
.db 00011101b
.db 00011001b
.db 00011101b
.db 00010001b
.db 00001110b
#ELSE
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00011001b
.db 00001010b
.db 00000100b
.db 00001010b
.db 00010011b
#ENDIF

.db 6
#IFDEF TI73
;;;SPRITE 8x7x1
.db 00011100b
.db 00001000b
.db 00001000b
.db 00001000b
.db 00001000b
.db 00001001b
.db 00011111b
#ELSE
;;;SPRITE 8x7x1
.db 00011111b
.db 00001001b
.db 00001000b
.db 00001110b
.db 00001010b
.db 00001000b
.db 00011000b
#ENDIF

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000110b
.db 00001001b
.db 00011110b
.db 00010000b
.db 00001110b

;;; DC

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000010b
.db 00000010b
.db 00000011b

.db 6
#IFDEF TI73
;;;SPRITE 8x7x1
.db 00000000b
.db 00000001b
.db 00000011b
.db 00000110b
.db 00001100b
.db 00011000b
.db 00010000b
#ELSE
;;;SPRITE 8x7x1
.db 00010001b
.db 00011001b
.db 00011101b
.db 00011011b
.db 00011001b
.db 00011001b
.db 00011001b
#ENDIF

.db 6
;;;SPRITE 8x7x1
.db 00010010b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00001001b
.db 00010010b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00011100b
.db 00010110b
.db 00010111b
.db 00010110b
.db 00011100b
.db 00000000b

;;; E0

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00011111b
.db 00011111b
.db 00011111b
.db 00011111b
.db 00011111b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00011011b
.db 00010001b
.db 00011011b
.db 00011011b
.db 00011011b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00011011b
.db 00010101b
.db 00010101b
.db 00010001b
.db 00010101b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00011111b
.db 00011001b
.db 00010101b
.db 00010101b
.db 00011001b
.db 00011111b

;;; E4

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00000000b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001110b
.db 00000100b
.db 00000100b
.db 00000100b
.db 00000000b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00001010b
.db 00001110b
.db 00001010b
.db 00000000b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000110b
.db 00001010b
.db 00001010b
.db 00000110b
.db 00000000b
.db 00011111b

;;; E8

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000101b
.db 00001010b
.db 00010000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000101b
.db 00001111b
.db 00011010b
.db 00010000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00011111b
.db 00011111b
.db 00011010b
.db 00010000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000101b
.db 00001111b
.db 00011111b
.db 00011111b
.db 00011111b

;;; EC

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000111b
.db 00001101b
.db 00010111b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000111b
.db 00000101b
.db 00000111b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00000000b
.db 00000101b
.db 00000000b
.db 00010000b
.db 00000000b
.db 00000000b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001110b
.db 00011111b
.db 00010001b
.db 00010001b
.db 00011111b
.db 00000000b

;;; F0

.db 6
;;;SPRITE 8x7x1
.db 00000000b
.db 00011111b
.db 00010001b
.db 00010001b
.db 00011111b
.db 00001110b
.db 00000100b

.db 6
;;;SPRITE 8x7x1
.db 00011111b
.db 00001110b
.db 00010101b
.db 00011011b
.db 00010101b
.db 00001110b
.db 00011111b

.db 6
;;;SPRITE 8x7x1
.db 00001110b
.db 00010101b
.db 00010100b
.db 00001110b
.db 00000101b
.db 00010101b
.db 00001110b

.db 6
;;;SPRITE 8x7x1
.db 00000100b
.db 00001010b
.db 00010001b
.db 00001010b
.db 00001010b
.db 00001010b
.db 00001110b

;;; F4

.db 6
;;;SPRITE 8x7x1
.db 00000110b
.db 00001001b
.db 00001001b
.db 00011011b
.db 00001010b
.db 00001001b
.db 00011011b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010111b
.db 00000110b
.db 00000001b
.db 00000110b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010011b
.db 00000110b
.db 00000101b
.db 00000010b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010111b
.db 00000001b
.db 00000010b
.db 00000010b

;;; F8

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011011b
.db 00010101b
.db 00000111b
.db 00000101b
.db 00000110b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010010b
.db 00000101b
.db 00000011b
.db 00000110b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010010b
.db 00000101b
.db 00000111b
.db 00000101b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011110b
.db 00010101b
.db 00000110b
.db 00000101b
.db 00000110b

;;; FC

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010011b
.db 00000100b
.db 00000100b
.db 00000011b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010110b
.db 00000101b
.db 00000101b
.db 00000110b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011011b
.db 00010010b
.db 00000011b
.db 00000010b
.db 00000011b

.db 6
;;;SPRITE 8x7x1
.db 00011100b
.db 00010000b
.db 00011000b
.db 00010111b
.db 00000100b
.db 00000110b
.db 00000100b

