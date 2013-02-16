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
;;;  RAM Code loading
;;;

LoadRC:
	ex (sp),hl
	push de
	 push bc
	  push af
	   ld c,(hl)
	   ld b,0
	   inc hl
	   ld de,ramCode
	   ldir
	   pop af
	  pop bc
	 pop de
	pop hl
	jp ramCode
