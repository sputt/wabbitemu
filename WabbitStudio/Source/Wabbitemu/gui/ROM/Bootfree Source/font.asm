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
#if HW_VERSION >= 4
; Char 00
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 01
	.db	01010101b
	.db	10010101b
	.db	10101010b
	.db	01010010b
	.db	01010101b
	.db	10101001b
	.db	00101010b
	.db	01010101b
	.db	10010101b
	.db	10101010b
	.db	01010010b
	.db	01010101b
	.db	10101001b
	.db	00101010b
	.db	00000000b
	.db	00000000b
; Char 02
	.db	00000000b
	.db	11000000b
	.db	11111111b
	.db	11110111b
	.db	11111111b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 03
	.db	00000000b
	.db	00011100b
	.db	00000000b
	.db	00000111b
	.db	11000000b
	.db	00000001b
	.db	01110000b
	.db	00000000b
	.db	00011100b
	.db	00000000b
	.db	00000111b
	.db	11000000b
	.db	00000001b
	.db	01110000b
	.db	00000000b
	.db	00011100b
; Char 04
	.db	00000000b
	.db	00000000b
	.db	00111100b
	.db	10000000b
	.db	00011111b
	.db	11100000b
	.db	00001111b
	.db	11110000b
	.db	00000111b
	.db	11111110b
	.db	10000000b
	.db	00011111b
	.db	11000000b
	.db	00000011b
	.db	00000000b
	.db	00000000b
; Char 05
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00001110b
	.db	11000000b
	.db	00000111b
	.db	11111000b
	.db	00000011b
	.db	01111100b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 06
	.db	11100000b
	.db	00000000b
	.db	00111000b
	.db	10000000b
	.db	01001111b
	.db	01110000b
	.db	00011100b
	.db	00011100b
	.db	00000111b
	.db	00111110b
	.db	00000001b
	.db	00001110b
	.db	10000000b
	.db	00000011b
	.db	00000000b
	.db	00000000b
; Char 07
	.db	01100000b
	.db	00000000b
	.db	00111100b
	.db	10000000b
	.db	01001111b
	.db	11110000b
	.db	00011111b
	.db	11111100b
	.db	00000111b
	.db	00111110b
	.db	00000001b
	.db	00001111b
	.db	10000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 08
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00001100b
	.db	10000000b
	.db	00000111b
	.db	11100000b
	.db	00000001b
	.db	00110000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 09
	.db	00000000b
	.db	00000000b
	.db	01111000b
	.db	00000000b
	.db	00110011b
	.db	01000000b
	.db	00001000b
	.db	00010000b
	.db	00000010b
	.db	11001100b
	.db	00000000b
	.db	00011110b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 0A
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	01010000b
	.db	00000001b
	.db	01111100b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 0B
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00001110b
	.db	11000000b
	.db	00000111b
	.db	01010000b
	.db	00000001b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 0C
	.db	01000000b
	.db	00000000b
	.db	00111000b
	.db	00000000b
	.db	00011111b
	.db	00000000b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	01111100b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 0D
	.db	00000000b
	.db	00000000b
	.db	01000000b
	.db	00000000b
	.db	00110000b
	.db	11110000b
	.db	00011111b
	.db	11111100b
	.db	00000111b
	.db	11000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 0E
	.db	00000000b
	.db	00000000b
	.db	00000100b
	.db	10000000b
	.db	00000001b
	.db	11110000b
	.db	00011111b
	.db	11111100b
	.db	00000111b
	.db	00000110b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 0F
	.db	00000000b
	.db	00000000b
	.db	01000100b
	.db	10000000b
	.db	00110001b
	.db	11110000b
	.db	00011111b
	.db	11111100b
	.db	00000111b
	.db	11000110b
	.db	00000000b
	.db	00010001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 10
	.db	11111111b
	.db	11011111b
	.db	11111111b
	.db	00010111b
	.db	00000000b
	.db	00000101b
	.db	01000000b
	.db	00000001b
	.db	01010000b
	.db	00000000b
	.db	11110100b
	.db	11111111b
	.db	11111101b
	.db	01111111b
	.db	00000000b
	.db	00000000b
; Char 11
	.db	01010101b
	.db	10010101b
	.db	10101010b
	.db	10101010b
	.db	10101010b
	.db	01010110b
	.db	01010101b
	.db	10101010b
	.db	10101010b
	.db	10101010b
	.db	01011010b
	.db	01010101b
	.db	10101001b
	.db	10101010b
	.db	10101010b
	.db	00101010b
; Char 12
	.db	10000000b
	.db	00111111b
	.db	11100000b
	.db	00001111b
	.db	11111000b
	.db	00000011b
	.db	11111110b
	.db	10000000b
	.db	00111111b
	.db	11100000b
	.db	00001111b
	.db	11111000b
	.db	00000011b
	.db	11111110b
	.db	10000000b
	.db	00111111b
; Char 13
	.db	01111111b
	.db	11000000b
	.db	00011111b
	.db	11110000b
	.db	00000111b
	.db	11111100b
	.db	00000001b
	.db	01111111b
	.db	11000000b
	.db	00011111b
	.db	11110000b
	.db	00000111b
	.db	11111100b
	.db	00000001b
	.db	01111111b
	.db	00000000b
; Char 14
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	11111111b
	.db	00111111b
; Char 15
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
; Char 16
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	11111110b
	.db	10000000b
	.db	00111111b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 17
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	11111110b
	.db	10000000b
	.db	00111111b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
; Char 18
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	11111110b
	.db	10000000b
	.db	00111111b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
; Char 19
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	11111100b
	.db	00000011b
	.db	11111111b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 1A
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	11111100b
	.db	00000011b
	.db	11111111b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
; Char 1B
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	11111100b
	.db	00000011b
	.db	11111111b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
; Char 1C
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	11111100b
	.db	11111111b
	.db	11111111b
	.db	00111111b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 1D
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	11111100b
	.db	11111111b
	.db	11111111b
	.db	00111111b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 1E
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	11111100b
	.db	11111111b
	.db	11111111b
	.db	00111111b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
; Char 1F
	.db	10000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	11111100b
	.db	11111111b
	.db	11111111b
	.db	00111111b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	10000000b
	.db	00000000b
; Char 20  
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 21 !
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	10000000b
	.db	00000011b
	.db	11110000b
	.db	00011011b
	.db	11111100b
	.db	00000110b
	.db	00001110b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 22 "
	.db	00000000b
	.db	10000000b
	.db	00000011b
	.db	11100000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	11100000b
	.db	00000001b
	.db	00111000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 23 #
	.db	00000000b
	.db	00000000b
	.db	01000100b
	.db	11000000b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	00010000b
	.db	00000001b
	.db	11111111b
	.db	11000001b
	.db	01111111b
	.db	01000000b
	.db	00000100b
	.db	00000000b
	.db	00000000b
; Char 24 $
	.db	00000000b
	.db	00000000b
	.db	11001110b
	.db	11000000b
	.db	01100111b
	.db	00010000b
	.db	00010001b
	.db	01000111b
	.db	11011100b
	.db	00010001b
	.db	11000111b
	.db	01111100b
	.db	01100000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
; Char 25 %
	.db	00000000b
	.db	00000000b
	.db	10000110b
	.db	10000001b
	.db	00110001b
	.db	00000000b
	.db	00000110b
	.db	11000000b
	.db	00000000b
	.db	00011000b
	.db	00000000b
	.db	01100011b
	.db	01100000b
	.db	00011000b
	.db	00000000b
	.db	00000000b
; Char 26 &
	.db	00000000b
	.db	00000000b
	.db	11100000b
	.db	10000000b
	.db	01111101b
	.db	11110000b
	.db	00010001b
	.db	11100100b
	.db	00000100b
	.db	11101111b
	.db	10000000b
	.db	01111101b
	.db	00000000b
	.db	00010001b
	.db	00000000b
	.db	00000000b
; Char 27 '
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	01111000b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 28 (
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00011111b
	.db	11100000b
	.db	00001111b
	.db	00001100b
	.db	00000110b
	.db	00000001b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 29 )
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	00010000b
	.db	00001100b
	.db	00000110b
	.db	11111110b
	.db	00000000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 2A *
	.db	01000000b
	.db	00000000b
	.db	01010100b
	.db	00000000b
	.db	00011111b
	.db	10000000b
	.db	00000011b
	.db	11100000b
	.db	00000000b
	.db	01111100b
	.db	00000000b
	.db	00010101b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 2B +
	.db	01000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	11100000b
	.db	00001111b
	.db	11111000b
	.db	00000011b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 2C ,
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	10000000b
	.db	00000000b
	.db	00111100b
	.db	00000000b
	.db	00000111b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 2D -
	.db	01000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
; Char 2E .
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00011000b
	.db	00000000b
	.db	00000110b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 2F /
	.db	00000000b
	.db	00000011b
	.db	01100000b
	.db	00000000b
	.db	00001100b
	.db	10000000b
	.db	00000001b
	.db	00110000b
	.db	00000000b
	.db	00000110b
	.db	11000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 30 0
	.db	00000000b
	.db	00000000b
	.db	11111110b
	.db	11000000b
	.db	01111111b
	.db	00010000b
	.db	00010011b
	.db	01100100b
	.db	00000100b
	.db	00001101b
	.db	11000001b
	.db	01111111b
	.db	11100000b
	.db	00001111b
	.db	00000000b
	.db	00000000b
; Char 31 1
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	01000001b
	.db	01100000b
	.db	00010000b
	.db	11111100b
	.db	00000111b
	.db	11111111b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000000b
; Char 32 2
	.db	00000000b
	.db	00000000b
	.db	10000010b
	.db	11000001b
	.db	01110000b
	.db	00010000b
	.db	00010110b
	.db	11000100b
	.db	00000100b
	.db	00011001b
	.db	11000001b
	.db	01100011b
	.db	01100000b
	.db	00011000b
	.db	00000000b
	.db	00000000b
; Char 33 3
	.db	00000000b
	.db	00000000b
	.db	10000010b
	.db	11000000b
	.db	01100000b
	.db	00010000b
	.db	00010001b
	.db	01000100b
	.db	00000100b
	.db	00010001b
	.db	11000001b
	.db	01111111b
	.db	11100000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
; Char 34 4
	.db	00000000b
	.db	00000000b
	.db	00110000b
	.db	00000000b
	.db	00001110b
	.db	11000000b
	.db	00000010b
	.db	10011000b
	.db	00000100b
	.db	11111111b
	.db	11000001b
	.db	01111111b
	.db	00000000b
	.db	00010010b
	.db	00000000b
	.db	00000000b
; Char 35 5
	.db	00000000b
	.db	00000000b
	.db	10011111b
	.db	11000000b
	.db	01100111b
	.db	00010000b
	.db	00010001b
	.db	01000100b
	.db	00000100b
	.db	00010001b
	.db	01000001b
	.db	01111100b
	.db	00010000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
; Char 36 6
	.db	00000000b
	.db	00000000b
	.db	11111100b
	.db	10000000b
	.db	01111111b
	.db	00110000b
	.db	00010001b
	.db	01000100b
	.db	00000100b
	.db	00010001b
	.db	00000001b
	.db	01111100b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
; Char 37 7
	.db	00000000b
	.db	00000000b
	.db	00000011b
	.db	11000000b
	.db	00000000b
	.db	00010000b
	.db	00011110b
	.db	11000100b
	.db	00000111b
	.db	00011001b
	.db	11000000b
	.db	00000011b
	.db	01110000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 38 8
	.db	00000000b
	.db	00000000b
	.db	11101110b
	.db	11000000b
	.db	01111111b
	.db	00010000b
	.db	00010001b
	.db	01000100b
	.db	00000100b
	.db	00010001b
	.db	11000001b
	.db	01111111b
	.db	11100000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
; Char 39 9
	.db	00000000b
	.db	00000000b
	.db	00001110b
	.db	11000000b
	.db	01000111b
	.db	00010000b
	.db	00010001b
	.db	01000100b
	.db	00000100b
	.db	10010001b
	.db	11000001b
	.db	00111111b
	.db	11100000b
	.db	00000111b
	.db	00000000b
	.db	00000000b
; Char 3A :
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	01100000b
	.db	00001100b
	.db	00011000b
	.db	00000011b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 3B ;
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	01000000b
	.db	01100000b
	.db	00011100b
	.db	00011000b
	.db	00000011b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 3C <
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00001110b
	.db	11000000b
	.db	00000110b
	.db	00011000b
	.db	00000011b
	.db	10000011b
	.db	01000001b
	.db	01000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 3D =
	.db	00000000b
	.db	00000000b
	.db	01001000b
	.db	00000000b
	.db	00010010b
	.db	10000000b
	.db	00000100b
	.db	00100000b
	.db	00000001b
	.db	01001000b
	.db	00000000b
	.db	00010010b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 3E >
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01100000b
	.db	01100000b
	.db	00001100b
	.db	10110000b
	.db	00000001b
	.db	00111000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 3F ?
	.db	00000000b
	.db	00000000b
	.db	00000110b
	.db	11000000b
	.db	00000001b
	.db	00010000b
	.db	00000000b
	.db	11000100b
	.db	00000110b
	.db	10111001b
	.db	11000001b
	.db	00000011b
	.db	01100000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 40 @
	.db	00000000b
	.db	00000000b
	.db	11111110b
	.db	11000000b
	.db	01111111b
	.db	00010000b
	.db	00010000b
	.db	11100100b
	.db	00000101b
	.db	01111001b
	.db	11000001b
	.db	01011111b
	.db	11100000b
	.db	00010011b
	.db	00000000b
	.db	00000000b
; Char 41 A
	.db	00000000b
	.db	00000000b
	.db	11111000b
	.db	00000001b
	.db	01111111b
	.db	01100000b
	.db	00000010b
	.db	10001100b
	.db	00000000b
	.db	00100110b
	.db	00000000b
	.db	01111111b
	.db	10000000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 42 B
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000100b
	.db	00000100b
	.db	00010001b
	.db	11000001b
	.db	01111111b
	.db	11100000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
; Char 43 C
	.db	00000000b
	.db	00000000b
	.db	01111100b
	.db	10000000b
	.db	00111111b
	.db	00110000b
	.db	00011000b
	.db	00000100b
	.db	00000100b
	.db	00000001b
	.db	11000001b
	.db	01100000b
	.db	01100000b
	.db	00001100b
	.db	00000000b
	.db	00000000b
; Char 44 D
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	00000100b
	.db	00000100b
	.db	10000011b
	.db	10000001b
	.db	00111111b
	.db	11000000b
	.db	00000111b
	.db	00000000b
	.db	00000000b
; Char 45 E
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000100b
	.db	00000100b
	.db	00111001b
	.db	11000001b
	.db	01100000b
	.db	01110000b
	.db	00011100b
	.db	00000000b
	.db	00000000b
; Char 46 F
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000100b
	.db	00000100b
	.db	00111001b
	.db	11000000b
	.db	00000000b
	.db	01110000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 47 G
	.db	00000000b
	.db	00000000b
	.db	01111100b
	.db	10000000b
	.db	00111111b
	.db	00110000b
	.db	00011000b
	.db	10000100b
	.db	00000100b
	.db	00100001b
	.db	11000001b
	.db	00111000b
	.db	01100000b
	.db	00011110b
	.db	00000000b
	.db	00000000b
; Char 48 H
	.db	00000000b
	.db	00000000b
	.db	11111111b
	.db	11000001b
	.db	01111111b
	.db	00000000b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00010000b
	.db	11000000b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 49 I
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	01000000b
	.db	01000000b
	.db	11110000b
	.db	00011111b
	.db	11111100b
	.db	00000111b
	.db	00000001b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 4A J
	.db	00000000b
	.db	00000000b
	.db	11000000b
	.db	00000000b
	.db	01110000b
	.db	00000000b
	.db	00010000b
	.db	00000100b
	.db	00000100b
	.db	11111111b
	.db	11000001b
	.db	00111111b
	.db	00010000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 4B K
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000000b
	.db	00000000b
	.db	01111100b
	.db	11000000b
	.db	01111011b
	.db	00110000b
	.db	00011000b
	.db	00000000b
	.db	00000000b
; Char 4C L
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	00000100b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	01100000b
	.db	00000000b
	.db	00011100b
	.db	00000000b
	.db	00000000b
; Char 4D M
	.db	11111100b
	.db	00000111b
	.db	11111111b
	.db	10000001b
	.db	00000001b
	.db	11000000b
	.db	00000000b
	.db	00110000b
	.db	00000000b
	.db	00000110b
	.db	11000000b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 4E N
	.db	00000000b
	.db	00000000b
	.db	11111111b
	.db	11000001b
	.db	01111111b
	.db	11100000b
	.db	00000000b
	.db	01110000b
	.db	00000000b
	.db	00111000b
	.db	11000000b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 4F O
	.db	00000000b
	.db	00000000b
	.db	01111100b
	.db	10000000b
	.db	00111111b
	.db	00110000b
	.db	00011000b
	.db	00000100b
	.db	00000100b
	.db	10000011b
	.db	10000001b
	.db	00111111b
	.db	11000000b
	.db	00000111b
	.db	00000000b
	.db	00000000b
; Char 50 P
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000100b
	.db	00000100b
	.db	00010001b
	.db	11000000b
	.db	00000111b
	.db	11100000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 51 Q
	.db	00000000b
	.db	00000000b
	.db	01111110b
	.db	11000000b
	.db	00111111b
	.db	00010000b
	.db	00001000b
	.db	10000100b
	.db	00000011b
	.db	11000001b
	.db	11000011b
	.db	11111111b
	.db	11100000b
	.db	00100111b
	.db	00000000b
	.db	00000000b
; Char 52 R
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000100b
	.db	00000000b
	.db	00110001b
	.db	11000000b
	.db	01111111b
	.db	11100000b
	.db	00011100b
	.db	00000000b
	.db	00000000b
; Char 53 S
	.db	00000000b
	.db	00000000b
	.db	11000110b
	.db	11000000b
	.db	01110011b
	.db	10010000b
	.db	00010001b
	.db	01000100b
	.db	00000100b
	.db	00110001b
	.db	11000001b
	.db	01111001b
	.db	01100000b
	.db	00001100b
	.db	00000000b
	.db	00000000b
; Char 54 T
	.db	00011100b
	.db	00000000b
	.db	00000011b
	.db	01000000b
	.db	01000000b
	.db	11110000b
	.db	00011111b
	.db	11111100b
	.db	00000111b
	.db	00000001b
	.db	11000001b
	.db	00000000b
	.db	01110000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 55 U
	.db	00000000b
	.db	00000000b
	.db	11111111b
	.db	11000000b
	.db	01111111b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00001111b
	.db	00000000b
	.db	00000000b
; Char 56 V
	.db	11111100b
	.db	00000000b
	.db	01111111b
	.db	00000000b
	.db	00110000b
	.db	00000000b
	.db	00011000b
	.db	00000000b
	.db	00000110b
	.db	11000000b
	.db	11000000b
	.db	00011111b
	.db	11110000b
	.db	00000011b
	.db	00000000b
	.db	00000000b
; Char 57 W
	.db	11111100b
	.db	00000001b
	.db	11111111b
	.db	00000001b
	.db	01110000b
	.db	00000000b
	.db	00000111b
	.db	11000000b
	.db	00000001b
	.db	11000000b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00000111b
	.db	00000000b
	.db	00000000b
; Char 58 X
	.db	00001100b
	.db	00000110b
	.db	11000111b
	.db	00000001b
	.db	00011011b
	.db	10000000b
	.db	00000011b
	.db	11100000b
	.db	00000000b
	.db	01101100b
	.db	11000000b
	.db	01110001b
	.db	00110000b
	.db	00011000b
	.db	00000000b
	.db	00000000b
; Char 59 Y
	.db	00011100b
	.db	00000000b
	.db	00001111b
	.db	00000000b
	.db	01000110b
	.db	00000000b
	.db	00011111b
	.db	11000000b
	.db	00000111b
	.db	00011000b
	.db	11000001b
	.db	00000011b
	.db	01110000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 5A Z
	.db	00011100b
	.db	00000110b
	.db	11000011b
	.db	01000001b
	.db	01011000b
	.db	00010000b
	.db	00010011b
	.db	01100100b
	.db	00000100b
	.db	00001101b
	.db	11000001b
	.db	01100001b
	.db	00110000b
	.db	00011100b
	.db	00000000b
	.db	00000000b
; Char 5B [
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	11000000b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	00000100b
	.db	00000100b
	.db	00000001b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 5C \
	.db	00011100b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000111b
	.db	10000000b
	.db	00000011b
	.db	11000000b
	.db	00000001b
	.db	11100000b
	.db	00000000b
	.db	01110000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 5D ]
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	01000000b
	.db	01000000b
	.db	00010000b
	.db	00010000b
	.db	11111100b
	.db	00000111b
	.db	11111111b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 5E ^
	.db	00000000b
	.db	00000000b
	.db	00000010b
	.db	11000000b
	.db	00000000b
	.db	00011000b
	.db	00000000b
	.db	00000011b
	.db	10000000b
	.db	00000001b
	.db	11000000b
	.db	00000000b
	.db	00100000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 5F _
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00000000b
; Char 60 `
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00001100b
	.db	00000000b
	.db	00000111b
	.db	00000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 61 a
	.db	00000000b
	.db	00000000b
	.db	11000000b
	.db	00000000b
	.db	01111010b
	.db	10000000b
	.db	00010010b
	.db	10100000b
	.db	00000100b
	.db	11111000b
	.db	00000000b
	.db	01111100b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000000b
; Char 62 b
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00001111b
	.db	00100000b
	.db	00000100b
	.db	00011000b
	.db	00000001b
	.db	01111100b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000000b
; Char 63 c
	.db	00000000b
	.db	00000000b
	.db	11110000b
	.db	00000000b
	.db	01111110b
	.db	10000000b
	.db	00010000b
	.db	00100000b
	.db	00000100b
	.db	00001000b
	.db	00000001b
	.db	01100110b
	.db	00000000b
	.db	00001001b
	.db	00000000b
	.db	00000000b
; Char 64 d
	.db	00000000b
	.db	00000000b
	.db	11100000b
	.db	00000000b
	.db	01111100b
	.db	10000000b
	.db	00010001b
	.db	00100100b
	.db	00000100b
	.db	11111111b
	.db	11000000b
	.db	01111111b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000000b
; Char 65 e
	.db	00000000b
	.db	00000000b
	.db	11110000b
	.db	00000000b
	.db	01111110b
	.db	10000000b
	.db	00010010b
	.db	10100000b
	.db	00000100b
	.db	00101000b
	.db	00000001b
	.db	01101110b
	.db	00000000b
	.db	00001011b
	.db	00000000b
	.db	00000000b
; Char 66 f
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	10000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000100b
	.db	00000100b
	.db	00000011b
	.db	10000000b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 67 g
	.db	00000000b
	.db	00000000b
	.db	01110000b
	.db	00000010b
	.db	10111110b
	.db	10000001b
	.db	01001000b
	.db	00100000b
	.db	00010010b
	.db	11110000b
	.db	00000111b
	.db	11111110b
	.db	10000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 68 h
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	01000000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	01111110b
	.db	00000000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 69 i
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	10000000b
	.db	00010000b
	.db	11101100b
	.db	00000111b
	.db	11111011b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 6A j
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000011b
	.db	11000000b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00010000b
	.db	11111011b
	.db	11000111b
	.db	11111110b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 6B k
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	11000001b
	.db	01111111b
	.db	11110000b
	.db	00011111b
	.db	10000000b
	.db	00000000b
	.db	01110000b
	.db	00000000b
	.db	01110110b
	.db	10000000b
	.db	00011000b
	.db	00000000b
	.db	00000000b
; Char 6C l
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	00010000b
	.db	11111100b
	.db	00000111b
	.db	11111111b
	.db	00000001b
	.db	01000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 6D m
	.db	11100000b
	.db	00000111b
	.db	11111000b
	.db	00000001b
	.db	00000110b
	.db	00000000b
	.db	00011111b
	.db	11000000b
	.db	00000111b
	.db	00011000b
	.db	00000000b
	.db	01111110b
	.db	00000000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 6E n
	.db	00000000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	01111110b
	.db	00000000b
	.db	00011111b
	.db	00100000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	01111110b
	.db	00000000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 6F o
	.db	00000000b
	.db	00000000b
	.db	11110000b
	.db	00000000b
	.db	01111110b
	.db	10000000b
	.db	00010000b
	.db	00100000b
	.db	00000100b
	.db	00001000b
	.db	00000001b
	.db	01111110b
	.db	00000000b
	.db	00001111b
	.db	00000000b
	.db	00000000b
; Char 70 p
	.db	00000000b
	.db	00000000b
	.db	00001000b
	.db	00000100b
	.db	11111110b
	.db	00000001b
	.db	01111111b
	.db	00100000b
	.db	00010010b
	.db	10001000b
	.db	00000000b
	.db	00111110b
	.db	00000000b
	.db	00000111b
	.db	00000000b
	.db	00000000b
; Char 71 q
	.db	11000000b
	.db	00000001b
	.db	11111000b
	.db	00000000b
	.db	00100010b
	.db	10000000b
	.db	01001000b
	.db	11000000b
	.db	00011111b
	.db	11111000b
	.db	00000111b
	.db	00000010b
	.db	00000001b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 72 r
	.db	00000000b
	.db	00000000b
	.db	00001000b
	.db	00000001b
	.db	01111110b
	.db	00000000b
	.db	00011111b
	.db	01100000b
	.db	00000100b
	.db	00001000b
	.db	00000000b
	.db	00001110b
	.db	00000000b
	.db	00000011b
	.db	00000000b
	.db	00000000b
; Char 73 s
	.db	00000000b
	.db	00000000b
	.db	10010000b
	.db	00000000b
	.db	01101110b
	.db	10000000b
	.db	00010010b
	.db	10100000b
	.db	00000101b
	.db	01001000b
	.db	00000001b
	.db	01110110b
	.db	00000000b
	.db	00001001b
	.db	00000000b
	.db	00000000b
; Char 74 t
	.db	00000000b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000010b
	.db	11100000b
	.db	00001111b
	.db	11111100b
	.db	00000111b
	.db	00001000b
	.db	00000001b
	.db	01100010b
	.db	00000000b
	.db	00001000b
	.db	00000000b
	.db	00000000b
; Char 75 u
	.db	00000000b
	.db	00000000b
	.db	11111000b
	.db	00000000b
	.db	01111110b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	11111000b
	.db	00000000b
	.db	01111110b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000000b
; Char 76 v
	.db	11100000b
	.db	00000000b
	.db	01111000b
	.db	00000000b
	.db	00110000b
	.db	00000000b
	.db	00011000b
	.db	00000000b
	.db	00000110b
	.db	11000000b
	.db	00000000b
	.db	00011110b
	.db	10000000b
	.db	00000011b
	.db	00000000b
	.db	00000000b
; Char 77 w
	.db	11100000b
	.db	00000011b
	.db	11111000b
	.db	00000001b
	.db	01100000b
	.db	00000000b
	.db	00001110b
	.db	10000000b
	.db	00000011b
	.db	10000000b
	.db	00000001b
	.db	01111110b
	.db	10000000b
	.db	00001111b
	.db	00000000b
	.db	00000000b
; Char 78 x
	.db	00000000b
	.db	00000000b
	.db	00001000b
	.db	00000001b
	.db	01100110b
	.db	00000000b
	.db	00001111b
	.db	10000000b
	.db	00000001b
	.db	11110000b
	.db	00000000b
	.db	01100110b
	.db	10000000b
	.db	00010000b
	.db	00000000b
	.db	00000000b
; Char 79 y
	.db	00000000b
	.db	00000000b
	.db	01111000b
	.db	00000000b
	.db	00111110b
	.db	00000001b
	.db	01001000b
	.db	00000000b
	.db	00010010b
	.db	10000000b
	.db	00000110b
	.db	11111110b
	.db	10000000b
	.db	00011111b
	.db	00000000b
	.db	00000000b
; Char 7A z
	.db	00000000b
	.db	00000000b
	.db	00011000b
	.db	00000001b
	.db	01100110b
	.db	10000000b
	.db	00011100b
	.db	10100000b
	.db	00000101b
	.db	00111000b
	.db	00000001b
	.db	01100110b
	.db	10000000b
	.db	00011000b
	.db	00000000b
	.db	00000000b
; Char 7B {
	.db	00000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	11100000b
	.db	00001111b
	.db	10111100b
	.db	00000111b
	.db	00000001b
	.db	01000001b
	.db	01000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 7C |
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	11110000b
	.db	00011110b
	.db	10111100b
	.db	00000111b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 7D }
	.db	00000000b
	.db	00000000b
	.db	00000001b
	.db	01000001b
	.db	01000000b
	.db	11110000b
	.db	00011110b
	.db	11111000b
	.db	00000011b
	.db	00010000b
	.db	00000000b
	.db	00000100b
	.db	00000000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 7E ~
	.db	00000000b
	.db	00000000b
	.db	00000010b
	.db	11000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00001100b
	.db	00000000b
	.db	00000010b
	.db	11000000b
	.db	00000000b
	.db	00010000b
	.db	00000000b
	.db	00000000b
	.db	00000000b
; Char 7F
	.db	00000000b
	.db	00000000b
	.db	11111000b
	.db	00000011b
	.db	11111111b
	.db	01000001b
	.db	00001001b
	.db	01010000b
	.db	00000010b
	.db	10010100b
	.db	00000000b
	.db	00111111b
	.db	10000000b
	.db	00000110b
	.db	00000000b
	.db	00000000b
; Char 80
	.db	11111111b
	.db	11011111b
	.db	11111111b
	.db	11110111b
	.db	11111111b
	.db	11111101b
	.db	01111111b
	.db	11111111b
	.db	11011111b
	.db	11111111b
	.db	11110111b
	.db	11111111b
	.db	11111101b
	.db	01111111b
	.db	00000000b
	.db	00000000b
; Char 81
	.db	11111111b
	.db	11011111b
	.db	11111011b
	.db	01110111b
	.db	11111110b
	.db	00001101b
	.db	01100000b
	.db	00000011b
	.db	11011000b
	.db	11111001b
	.db	11110111b
	.db	11111110b
	.db	11111101b
	.db	01111111b
	.db	00000000b
	.db	00000000b
; Char 82
	.db	11111111b
	.db	11011111b
	.db	00000011b
	.db	01110110b
	.db	10000000b
	.db	11001101b
	.db	01111110b
	.db	10110011b
	.db	11011111b
	.db	00000001b
	.db	11110110b
	.db	10000000b
	.db	11111101b
	.db	01111111b
	.db	00000000b
	.db	00000000b
; Char 83
	.db	11111111b
	.db	11011111b
	.db	00011111b
	.db	11110111b
	.db	10000010b
	.db	10111101b
	.db	01101110b
	.db	10101111b
	.db	11011011b
	.db	00000011b
	.db	11110110b
	.db	10000001b
	.db	11111101b
	.db	01111111b
	.db	00000000b
	.db	00000000b
; Char 84
	.db	00000011b
	.db	11011000b
	.db	00000000b
	.db	00010110b
	.db	00000000b
	.db	00000101b
	.db	01000000b
	.db	00000001b
	.db	01010000b
	.db	00000000b
	.db	00110100b
	.db	10000000b
	.db	00001101b
	.db	01100000b
	.db	00000000b
	.db	00000000b
; Char 85
	.db	00000011b
	.db	11011000b
	.db	00000100b
	.db	10010110b
	.db	00000001b
	.db	11110101b
	.db	01011111b
	.db	11111101b
	.db	01010111b
	.db	00000110b
	.db	00110100b
	.db	10000001b
	.db	00001101b
	.db	01100000b
	.db	00000000b
	.db	00000000b
; Char 86
	.db	00000011b
	.db	11011000b
	.db	11111100b
	.db	10010110b
	.db	00111111b
	.db	00110101b
	.db	01000001b
	.db	01001101b
	.db	01010000b
	.db	11111110b
	.db	00110100b
	.db	10111111b
	.db	00001101b
	.db	01100000b
	.db	00000000b
	.db	00000000b
; Char 87
	.db	00000011b
	.db	11011000b
	.db	01100000b
	.db	00010110b
	.db	00111101b
	.db	01000101b
	.db	01001001b
	.db	01010001b
	.db	01010010b
	.db	11111100b
	.db	00110100b
	.db	10111110b
	.db	00001101b
	.db	01100000b
	.db	00000000b
	.db	00000000b

#else
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
#endif

