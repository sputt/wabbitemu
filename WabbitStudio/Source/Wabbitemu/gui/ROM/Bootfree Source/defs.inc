;;; -*- TI-Asm -*-

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Definitions
;;;

BOOT_MAJOR	.equ 11
BOOT_MINOR	.equ 246

#IFDEF TI73
#define N_FLASH_PAGES 20h
#define FIRST_APP 08h
#define RAM_PAGE 40h
#define PRODUCT_ID 02h
#define HW_VERSION 0
#ENDIF

#IFDEF TI83PBE
#define N_FLASH_PAGES 20h
#define FIRST_APP 15h
#define RAM_PAGE 40h
#define PRODUCT_ID 04h
#define HW_VERSION 0
#ENDIF

#IFDEF TI83PSE
#define SILVER
#define PORT_21 1
#define N_FLASH_PAGES 80h
#define FIRST_APP 69h
#define RAM_PAGE 80h
#define PRODUCT_ID 04h
#define HW_VERSION 1
#ENDIF

#IFDEF TI84PBE
#define SILVER
#define USB
#define PORT_21 0
#define N_FLASH_PAGES 40h
#define FIRST_APP 29h
#define RAM_PAGE 80h
#define PRODUCT_ID 0Ah
#define HW_VERSION 2
#ENDIF

#IFDEF TI84PSE
#define SILVER
#define USB
#define PORT_21 1
#define N_FLASH_PAGES 80h
#define FIRST_APP 69h
#define RAM_PAGE 80h
#define PRODUCT_ID 0Ah
#define HW_VERSION 3
#ENDIF

#IFDEF TI84PCSE
#define SILVER
#define USB
#define PORT_21 1
#define N_FLASH_PAGES 100h
#define FIRST_APP 0E9h
#define RAM_PAGE 80h
#define PRODUCT_ID 0Ah
#define HW_VERSION 4
#define FLASH_MASK	(N_FLASH_PAGES - 1) & $7F
#define BOOT_PAGE	(N_FLASH_PAGES - 1) & $7F
#define CERT_PAGE	(N_FLASH_PAGES - 2) & $7F
#else
#define FLASH_MASK	N_FLASH_PAGES - 1
#define BOOT_PAGE	N_FLASH_PAGES - 1
#define CERT_PAGE	N_FLASH_PAGES - 2
#ENDIF


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  Macros
;;;
#include "relocate.inc"

#define RAM_CODE_NUM 0
#macro BEGIN_RAM_CODE
	call LoadRC
#define RAM_CODE_NUM RAM_CODE_NUM+1
	clr()
	wr(".db _EndRamCode", RAM_CODE_NUM, " - $")
	run()
	relocate(ramcode)
#endmacro

#macro END_RAM_CODE
	endrelocate()
	clr()
	wr("_EndRamCode", RAM_CODE_NUM, ":")
	run()
#endmacro
;#macro END_RAM_CODE(name)
;name:
;	.ENDM

;;; Provide DL (define long) (used in the MD5 table)

#macro DL(xxx)
.db low(xxx), high(xxx), low (xxx >> 16), high(xxx >> 16)
#endmacro
#define .dl DL(

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  RAM Areas
;;;

#IFDEF TI73

appData         .equ 8000h
ramCode         .equ 8100h
bootTemp        .equ 8241h

MD5_Temp        .equ 8241h
MD5_AltRegs     .equ 8249h
MD5_Length      .equ 8259h
MD5_Hash        .equ 8282h
appID           .equ 837Dh
MD5_Buffer      .equ 8395h

curRow          .equ 83E2h
curCol          .equ 83E3h

OP1             .equ 842Bh
OP2             .equ 8436h
OP3             .equ 8441h
OP4             .equ 844Ch
OP5             .equ 8457h
OP6             .equ 8462h

lFont_record    .equ 84A3h

fontFlags       .equ 37
#ELSE

appData         .equ 8000h
ramCode         .equ 8100h
bootTemp        .equ 8251h

MD5_Temp        .equ 8251h
MD5_AltRegs     .equ 8259h
MD5_Length      .equ 8269h
MD5_Hash        .equ 8292h
appID           .equ 838Dh
MD5_Buffer      .equ 83A5h

curRow          .equ 844Bh
curCol          .equ 844Ch

OP1             .equ 8478h
OP2             .equ 8483h
OP3             .equ 848Eh
OP4             .equ 8499h
OP5             .equ 84A4h
OP6             .equ 84AFh

lFont_record    .equ 845Ah

fontFlags       .equ 50

#ENDIF

;;; MD5 registers

MD5_AA .equ MD5_AltRegs
MD5_BB .equ MD5_AltRegs+4
MD5_CC .equ MD5_AltRegs+8
MD5_DD .equ MD5_AltRegs+12

MD5_A .equ MD5_Hash
MD5_B .equ MD5_Hash+4
MD5_C .equ MD5_Hash+8
MD5_D .equ MD5_Hash+12

;;; Bignum registers

bigA .equ ramCode
bigB .equ ramCode+82h
bigC .equ ramCode+0C3h
bigN .equ appData

;;; Flags

fracDrawNum	.equ	   0
fracDrawDenom	.equ	   1
fracDrawLFont	.equ	   2
fracTallLFont	.equ	   3
fracLastChar	.equ	   4
fracCursor	.equ	   5
DrawLfont       .equ       6
CustomFont      .equ       7
