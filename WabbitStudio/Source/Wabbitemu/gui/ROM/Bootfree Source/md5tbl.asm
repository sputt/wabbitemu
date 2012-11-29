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
;;;  MD5 Tables
;;;

;;; Table of offsets (k), shifts (s), and table constants (t) for each
;;; of the 64 MD5 operations

#macro MTBL(kkk, sss, ttt)
.db (low(MD5_BUFFER))+(kkk * 4)
.dl ttt
.db sss
#endmacro

MD5_Table:
	MTBL(0,  7, 0d76aa478h)
	MTBL(1, 12, 0e8c7b756h)
	MTBL(2, 17, 0242070dbh)
	MTBL(3, 22, 0c1bdceeeh)
	MTBL(4,  7, 0f57c0fafh)
	MTBL(5, 12, 04787c62ah)
	MTBL(6, 17, 0a8304613h)
	MTBL(7, 22, 0fd469501h)
	MTBL(8,  7, 0698098d8h)
	MTBL(9, 12, 08b44f7afh)
	MTBL(10, 17, 0ffff5bb1h)
	MTBL(11, 22, 0895cd7beh)
	MTBL(12,  7, 06b901122h)
	MTBL(13, 12, 0fd987193h)
	MTBL(14, 17, 0a679438eh)
	MTBL(15, 22, 049b40821h)

	MTBL(1,  5, 0f61e2562h)
	MTBL(6,  9, 0c040b340h)
	MTBL(11, 14, 0265e5a51h)
	MTBL(0, 20, 0e9b6c7aah)
	MTBL(5,  5, 0d62f105dh)
	MTBL(10,  9, 002441453h)
	MTBL(15, 14, 0d8a1e681h)
	MTBL(4, 20, 0e7d3fbc8h)
	MTBL(9,  5, 021e1cde6h)
	MTBL(14,  9, 0c33707d6h)
	MTBL(3, 14, 0f4d50d87h)
	MTBL(8, 20, 0455a14edh)
	MTBL(13,  5, 0a9e3e905h)
	MTBL(2,  9, 0fcefa3f8h)
	MTBL(7, 14, 0676f02d9h)
	MTBL(12, 20, 08d2a4c8ah)

	MTBL(5,  4, 0fffa3942h)
	MTBL(8, 11, 08771f681h)
	MTBL(11, 16, 06d9d6122h)
	MTBL(14, 23, 0fde5380ch)
	MTBL(1,  4, 0a4beea44h)
	MTBL(4, 11, 04bdecfa9h)
	MTBL(7, 16, 0f6bb4b60h)
	MTBL(10, 23, 0bebfbc70h)
	MTBL(13,  4, 0289b7ec6h)
	MTBL(0, 11, 0eaa127fah)
	MTBL(3, 16, 0d4ef3085h)
	MTBL(6, 23, 004881d05h)
	MTBL(9,  4, 0d9d4d039h)
	MTBL(12, 11, 0e6db99e5h)
	MTBL(15, 16, 01fa27cf8h)
	MTBL(2, 23, 0c4ac5665h)

	MTBL(0,  6, 0f4292244h)
	MTBL(7, 10, 0432aff97h)
	MTBL(14, 15, 0ab9423a7h)
	MTBL(5, 21, 0fc93a039h)
	MTBL(12,  6, 0655b59c3h)
	MTBL(3, 10, 08f0ccc92h)
	MTBL(10, 15, 0ffeff47dh)
	MTBL(1, 21, 085845dd1h)
	MTBL(8,  6, 06fa87e4fh)
	MTBL(15, 10, 0fe2ce6e0h)
	MTBL(6, 15, 0a3014314h)
	MTBL(13, 21, 04e0811a1h)
	MTBL(4,  6, 0f7537e82h)
	MTBL(11, 10, 0bd3af235h)
	MTBL(2, 15, 02ad7d2bbh)
	MTBL(9, 21, 0eb86d391h)


;;; Table of register addresses for each of the 16 operations in each
;;; round (we restart at the beginning of the next round.)

;;; This table is read backwards, since the sequence of registers
;;; shifts right after each instruction.  So after each operation we
;;; move back by one byte in this table.

	.DB            low(MD5_B), low(MD5_C), low(MD5_D)
	.DB low(MD5_A), low(MD5_B), low(MD5_C), low(MD5_D)
	.DB low(MD5_A), low(MD5_B), low(MD5_C), low(MD5_D)
	.DB low(MD5_A), low(MD5_B), low(MD5_C), low(MD5_D)
MD5_RegsTable:
	.DB low(MD5_A), low(MD5_B), low(MD5_C), low(MD5_D)

