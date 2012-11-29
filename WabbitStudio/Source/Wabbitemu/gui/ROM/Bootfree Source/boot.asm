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

#INCLUDE "defs.inc"

#define low(xx) xx & $FF
#define high(xx) xx >> 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Startup and B_CALL Tables
;;;

.ORG 4000h

;;; Start here on CPU reset (initially mapped at 8000h)

	ld a,BOOT_PAGE		; + 2 = 4002
	out (6),a		; + 2 = 4004
	out (7),a		; + 2 = 4006
	jp Init+4000h		; + 3 = 4009

	.db 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh ; + 6 = 400F

;;; Boot code version string

VersionStr:
	.db "11.246", 0		; + 7 = 4016
	.db 0FFh, 0FFh		; + 2 = 4018

;;; B_CALL address table
#macro BCPT(xx)
.dw xx
.db BOOT_PAGE
#endmacro

	BCPT(MD5Final
   BCPT(UnknownBC ;	BCPT(RSAValidate
	BCPT(BigNumCompare
	BCPT(WriteAByteUnsafe
	BCPT(EraseFlash
	BCPT(FindFirstCertificateField
	BCPT(ZeroToCertificate
	BCPT(GetCertificateEnd
	BCPT(FindGroupedField
	BCPT(DoNothing
	BCPT(DoNothing
	BCPT(DoNothing
	BCPT(DoNothing
	BCPT(DoNothing
	BCPT(ATimesE
	BCPT(ATimesDE
	BCPT(DivHLByE
	BCPT(DivHLByDE
    BCPT(UnknownBC
	BCPT(LoadAIndPaged
	BCPT(FlashToRAM2
	BCPT(GetCertificateStart
	BCPT(GetFieldSize
	BCPT(FindSubField
	BCPT(EraseCertificateSector
    BCPT(UnknownBC ;	BCPT(CheckHeaderKey
    BCPT(UnknownBC
    BCPT(UnknownBC
	BCPT(Load_LFont2
	BCPT(Load_LFontV
    BCPT(UnknownBC ;	BCPT(OSReceive
	BCPT(FindOSHeaderSubField
	BCPT(FindNextCertificateField
    BCPT(UnknownBC ;	BCPT(RecAByte2
	BCPT(GetCalcSerial
    BCPT(UnknownBC
	BCPT(EraseFlashPage
	BCPT(WriteFlashUnsafe
	BCPT(DispBootVer
	BCPT(MD5Init
	BCPT(MD5Update
    BCPT(UnknownBC ;	BCPT(MarkOSInvalid
    BCPT(UnknownBC ;	BCPT(FindAppKey
	BCPT(MarkOSValid
	BCPT(CheckOSValid
    BCPT(UnknownBC ;	BCPT(SetupAppPubKey
    BCPT(UnknownBC ;	BCPT(RabinValidate
    BCPT(UnknownBC ;	BCPT(TransformHash
    BCPT(IsAppFreeware
	BCPT(FindAppHeaderSubField
    BCPT(UnknownBC ;	BCPT(RecalcValidationBytes
	BCPT(Div32ByDE
	BCPT(FindSimpleGroupedField
	BCPT(GetBootVer
	BCPT(GetHWVer
	BCPT(XorA
    BCPT(UnknownBC ;	BCPT(RSAValidateBigB
	BCPT(ProdNrPart1
	BCPT(WriteAByteSafe
	BCPT(WriteFlashSafe
    BCPT(UnknownBC ;	BCPT(SetupDateStampPubKey

#IFNDEF TI73

  	BCPT(SetAppLimit
   BCPT(UnknownBC ;	BCPT(BatteryError

#endif
 
#ifdef USB

	; 40D5
	ld a,BOOT_PAGE		; + 2 = 40D7
	out (6),a		; + 2 = 40D9
	out (7),a		; + 2 = 40.db
	jp Init+4000h		; + 3 = 40DE

	.db 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh ; + 6 = 40E4

   BCPT(UnknownBC ;	BCPT(USBMainLoop
   BCPT(UnknownBC ;	BCPT(DisplaySysMessage
	BCPT(NewLine2
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(CheckBattery
   BCPT(UnknownBC ;	BCPT(CheckBattery46
   BCPT(UnknownBC ;	BCPT(OSReceive_USB
   BCPT(UnknownBC ;	BCPT(OSRPacketSetup
   BCPT(UnknownBC ;	BCPT(ForceReboot
   BCPT(UnknownBC ;	BCPT(SetupOSPubKey
   BCPT(UnknownBC ;	BCPT(CheckHeaderKeyHL
   BCPT(UnknownBC
   BCPT(UnknownBC
   BCPT(UnknownBC
   BCPT(UnknownBC
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff
   BCPT(UnknownBC ;	BCPT(DisplaySysErrorAndTurnOff

#endif



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Basic Boot Code
;;;

;;; 40CF / 40D5 / 412C

Init:	di

;;; Memory mapping
	ld a,BOOT_PAGE
	out (6),a
	out (7),a
#ifdef HW_VERSION >= 2
	ld a,06h
#else
	ld a,76h
#endif
	out (4),a
	jp InitMain
InitMain:
	ld a,RAM_PAGE|0
	out (7),a
#ifdef SILVER
	xor a
	out (5),a
#endif

;;; Set SP
	ld hl,0
	ld sp,hl
	push hl
	push hl
	push hl

;;; Link port
	xor a
	out (0),a

;;; Keypad
	ld a,0ffh
	out (1),a

;;; Interrupt timers
	ld a,08h
	out (3),a
	ld a,0bh
	out (3),a

;;; LCD
	ld a,18h		; reset test mode
	call LCDOut
	ld a,01h		; 8 bit mode
	call LCDOut
	ld a,05h		; increment down
	call LCDOut
	ld a,0f0h		; reasonable default contrast
	call LCDOut

;;; Protected hardware...
	call FlashWE

;;; Execution limits
 #ifdef TI73
	ld a,0ch
	nop
	nop
	im 1
	di
	out (16h),a
#ELSE
	ld a,FIRST_APP
	call SetAppLimit
#endif

;;; RAM execution protection
#ifdef TI83PBE
	ld a,7
	out (5),a
	ld a,0fh
	nop
	nop
	im 1
	di
	out (16h),a
#endif

#ifdef SILVER

;;; Link assist
	ld a,80h
	out (8),a
	xor a
	out (8),a

;;; Unknown - related to memory mapping?
	out (0eh),a
	out (0fh),a

;;; CPU
	ld a,1
	out (20h),a

;;; Hardware type
	ld a,PORT_21
	nop
	nop
	im 1
	di
	out (21h),a

;;; Low Flash execution limit
	ld a,8
	nop
	nop
	im 1
	di
	out (22h),a

;;; Unknown - related to execution protection?
	ld a,10h
	nop
	nop
	im 1
	di
	out (25h),a

;;; Unknown - related to execution protection?
	ld a,20h
	nop
	nop
	im 1
	di
	out (26h),a

;;; Block memory mapping
	xor a
	out (27h),a
	out (28h),a

;;; LCD delay
	ld a,14h
	out (29h),a
	ld a,27h
	out (2Ah),a
	ld a,2Fh
	out (2Bh),a
	ld a,3Bh
	out (2Ch),a

;;; Unknown
	ld a,01h
	out (2Dh),a

;;; CPU speed
	ld a,44h
	out (2Eh),a

;;; LCD delay timer
	ld a,4Ah
	out (2Fh),a
#endif

	call FlashWD

	or a
	ld hl,(0056h)
	ld bc,0A55Ah
	sbc hl,bc
	jp z,0053h

	ld a,3
	call LCDOut
	call ClearLCD
	ld hl,1
	ld (curRow),hl
	call DispBootVerX
	call NewLine2
	ld hl,OSNotLoadedStr
	call PutS
	di
	halt

OSNotLoadedStr:
	.db 0D6h
	.db "* No OS Loaded *"
	.db 0D6h
	.db "Download OS:", 0D6h
	.db "education.ti.com"
	.db 0
	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Boot Code API Routines
;;;

#include "bigmath.asm"
#include "cert.asm"
#include "field.asm"
#include "flash.asm"
#include "font.asm"
#include "lcd.asm"
#include "md5.asm"
#include "math.asm"
#include "mem.asm"
#include "misc.asm"
#include "ramcode.asm"
#include "exec.asm"

;;; The following is provided to allow testing programs that require
;;; Flash access

.fill 7FF0h - $
	nop
	nop
	im 1
	di
	out (14h),a
	ret

	.db 0FFh, 0FFh, 0FFh, 0FFh
	.db 0FFh, 0FFh, 0FFh, 0FFh
