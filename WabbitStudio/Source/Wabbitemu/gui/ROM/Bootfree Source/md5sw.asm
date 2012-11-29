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
;;;  MD5 Support Routines (Software Version)
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
	ld hl,MD5_Table
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop1:
	call MD5_FF
	djnz MD5_FourRounds_Loop1
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop2:
	call MD5_GG
	djnz MD5_FourRounds_Loop2
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop3:
	call MD5_HH
	djnz MD5_FourRounds_Loop3
	ld b,16
	ld ix,MD5_RegsTable
MD5_FourRounds_Loop4:
	call MD5_II
	djnz MD5_FourRounds_Loop4
	ret


;;
;; MD5_FF
;;
;;  Perform an FF operation.
;;
;;  Input: HL -> MD5_Table entry
;;         IX -> MD5_RegsTable entry
;;  Output: HL -> next MD5_Table entry
;;          IX -> next MD5_RegsTable entry
;;  Destroys: AF, DE, MD5_Hash

MD5_FF:
	push bc
	 push hl

	;; Compute FF(a,b,c,d,k,s,t)
	;; IX -> <a <b <c <d
	;; HL -> (<MD5_Buffer + 4k) <<t ><t <>t >>t s
	;;
	;; FF(a,b,c,d,k,s,t) = b + ((a + F(b,c,d) + X[k] + t) <<< s)
	;;
	;; where  F(X,Y,Z) = XY v not(X) Z
	;;                 = Z xor X (Y xor Z)

	  ld l,(ix+2)
	  ld h,high(MD5_Hash)
	  call Load_BCDE_iHL	; c

	  ld l,(ix+3)
	  call Xor_BCDE_iHL	; c xor d

	  ld l,(ix+1)
	  call And_BCDE_iHL	; b (c xor d)

	  ld l,(ix+3)
	  call Xor_BCDE_iHL	; d xor b (c xor d) = F(b,c,d)

MD5_Operation_Main:
	;; The rest of the computation is the same for all four
	;; rounds.

	  ld l,(ix+0)
	  call Add_BCDE_iHL	; a + f(b,c,d)

	  pop hl
	 push hl
	  ld l,(hl)
	  ld h,high(MD5_Buffer)
	  call Add_BCDE_iHL	; a + f(b,c,d) + X[k]

	  pop hl
	 inc hl
	 call Add_BCDE_iHL	; a + f(b,c,d) + X[k] + t
	 inc hl
	 ld a,(hl)
	 inc hl			; advance to next MD5_Table entry
	 push hl
	  call RLC_BCDE_A	; (a + f(b,c,d) + X[k] + t) <<< s

	  ld l,(ix+1)
	  ld h,high(MD5_Hash)
	  call Add_BCDE_iHL	; b + ((a + f(b,c,d) + X[k] + t) <<< s)

	;; Finally, we store the result back into
		
	;; [ o/~  the byte pointed to by SEC, and advance the pointer
	;;        SEC by one byte...  o/~ ]

	;; We store the result back into a, and advance to the next
	;; entry in the MD5_RegsTable.

	  ld l,(ix+0)
	  ld (hl),e
	  inc hl
	  ld (hl),d
	  inc hl
	  ld (hl),c
	  inc hl
	  ld (hl),b

	  dec ix
	  pop hl
	 pop bc
	ret


;;
;; MD5_GG
;;
;;  Perform a GG operation.
;;
;;  Input: HL -> MD5_Table entry
;;         IX -> MD5_RegsTable entry
;;  Output: HL -> next MD5_Table entry
;;          IX -> next MD5_RegsTable entry
;;  Destroys: AF, DE, MD5_Hash

MD5_GG:
	push bc
	 push hl

	;; Compute GG(a,b,c,d,k,s,t)
	;;
	;; GG(a,b,c,d,k,s,t) = b + ((a + G(b,c,d) + X[k] + t) <<< s)
	;;
	;; where  G(X,Y,Z) = XZ v Y not(Z)
	;;                 = Y xor Z (X xor Y)

	  ld l,(ix+1)
	  ld h,high(MD5_Hash)
	  call Load_BCDE_iHL	; b

	  ld l,(ix+2)
	  call Xor_BCDE_iHL	; b xor c

	  ld l,(ix+3)
	  call And_BCDE_iHL	; d (b xor c)

	  ld l,(ix+2)
	  call Xor_BCDE_iHL	; c xor d (b xor c) = G(b,c,d)

	  jr MD5_Operation_Main


;;
;; MD5_HH
;;
;;  Perform an HH operation.
;;
;;  Input: HL -> MD5_Table entry
;;         IX -> MD5_RegsTable entry
;;  Output: HL -> next MD5_Table entry
;;          IX -> next MD5_RegsTable entry
;;  Destroys: AF, DE, MD5_Hash

MD5_HH:
	push bc
	 push hl

	;; Compute HH(a,b,c,d,k,s,t)
	;;
	;; HH(a,b,c,d,k,s,t) = b + ((a + H(b,c,d) + X[k] + t) <<< s)
	;;
	;; where  H(X,Y,Z) = X xor Y xor Z

	  ld l,(ix+1)
	  ld h,high(MD5_Hash)
	  call Load_BCDE_iHL	; b

	  ld l,(ix+2)
	  call Xor_BCDE_iHL	; b xor c

	  ld l,(ix+3)
	  call Xor_BCDE_iHL	; b xor c xor d = H(b,c,d)

	  jr MD5_Operation_Main


;;
;; MD5_II
;;
;;  Perform an II operation.
;;
;;  Input: HL -> MD5_Table entry
;;         IX -> MD5_RegsTable entry
;;  Output: HL -> next MD5_Table entry
;;          IX -> next MD5_RegsTable entry
;;  Destroys: AF, DE, MD5_Hash

MD5_II:
	push bc
	 push hl

	;; Compute II(a,b,c,d,k,s,t)
	;;
	;; II(a,b,c,d,k,s,t) = b + ((a + I(b,c,d) + X[k] + t) <<< s)
	;;
	;; where  I(X,Y,Z) = Y xor (X v not(Z))

	  ld l,(ix+3)
	  ld h,high(MD5_Hash)
	  ld a,(hl)
	  cpl
	  ld e,a
	  inc hl
	  ld a,(hl)
	  cpl
	  ld d,a
	  inc hl
	  ld a,(hl)
	  cpl
	  ld c,a
	  inc hl
	  ld a,(hl)
	  cpl
	  ld b,a		; not(d)

	  ld l,(ix+1)
	  call Or_BCDE_iHL	; b v not(d)

	  ld l,(ix+2)
	  call Xor_BCDE_iHL	; c xor (b v not(d)) = H(b,c,d)

	  jp MD5_Operation_Main


;;
;; Load_BCDE_iHL
;;
;;  Load BCDE from (HL) in little-endian order.
;;
;;  Input: HL -> 32-bit integer
;;  Output: BCDE = (HL)
;;          HL increased by 3
;;  Destroys: None

Load_BCDE_iHL:
	ld e,(hl)
	inc hl
	ld d,(hl)
	inc hl
	ld c,(hl)
	inc hl
	ld b,(hl)
	ret


;;
;; And_BCDE_iHL
;;
;;  AND BCDE with (HL).
;;
;;  Input: HL -> 32-bit integer
;;         BCDE = 32-bit integer
;;  Output: BCDE = BCDE & (HL)
;;          HL increased by 3
;;  Destroys: AF

And_BCDE_iHL:
	ld a,(hl)
	and e
	ld e,a
	inc hl
	ld a,(hl)
	and d
	ld d,a
	inc hl
	ld a,(hl)
	and c
	ld c,a
	inc hl
	ld a,(hl)
	and b
	ld b,a
	ret


;;
;; Or_BCDE_iHL
;;
;;  OR BCDE with (HL).
;;
;;  Input: HL -> 32-bit integer
;;         BCDE = 32-bit integer
;;  Output: BCDE = BCDE v (HL)
;;          HL increased by 3
;;  Destroys: AF

Or_BCDE_iHL:
	ld a,(hl)
	or e
	ld e,a
	inc hl
	ld a,(hl)
	or d
	ld d,a
	inc hl
	ld a,(hl)
	or c
	ld c,a
	inc hl
	ld a,(hl)
	or b
	ld b,a
	ret


;;
;; Xor_BCDE_iHL
;;
;;  XOR BCDE with (HL).
;;
;;  Input: HL -> 32-bit integer
;;         BCDE = 32-bit integer
;;  Output: BCDE = BCDE xor (HL)
;;          HL increased by 3
;;  Destroys: AF

Xor_BCDE_iHL:
	ld a,(hl)
	xor e
	ld e,a
	inc hl
	ld a,(hl)
	xor d
	ld d,a
	inc hl
	ld a,(hl)
	xor c
	ld c,a
	inc hl
	ld a,(hl)
	xor b
	ld b,a
	ret


;;
;; Add_BCDE_iHL
;;
;;  Add (HL) to BCDE.
;;
;;  Input: HL -> 32-bit integer
;;         BCDE = 32-bit integer
;;  Output: BCDE = BCDE + (HL)
;;          HL increased by 3
;;  Destroys: AF

Add_BCDE_iHL:
	ld a,(hl)
	add a,e
	ld e,a
	inc hl
	ld a,(hl)
	adc a,d
	ld d,a
	inc hl
	ld a,(hl)
	adc a,c
	ld c,a
	inc hl
	ld a,(hl)
	adc a,b
	ld b,a
	ret


;;
;; RLC_BCDE_A
;;
;;  Rotate BCDE left circularly by A bits.
;;
;;  Input: A = count of bits
;;         BCDE = 32-bit integer
;;  Output: BCDE = BCDE <<< A
;;  Destroys: AF, L

RLC_BCDE_A:
	sub 8
	jr c,RLC_BCDE_A_Bits

	ld l,b
	ld b,c
	ld c,d
	ld d,e
	ld e,l
	jr RLC_BCDE_A

RLC_BCDE_A_Bits:
	add a,8
	ret z
	ld l,a
	ld a,b
	ld b,l
	ex de,hl

RLC_BCDE_A_Loop:
	add hl,hl
	rl c
	rla
	jr nc,RLC_BCDE_A_NC
	inc l
RLC_BCDE_A_NC:
	djnz RLC_BCDE_A_Loop

	ex de,hl
	ld b,a
	ret
