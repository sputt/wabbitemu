.nolist
;===============================================================================
;====== Include Stuff ==========================================================
;===============================================================================

;Included for TASM Compatibility
;-------------------------------
#define equ .equ
#define EQU .equ
#define end .end
#define END .end


;====== Hardware Ports =========================================================
; p = Port
; LnkAst = Link Assist, Pg = Page
; Se = Silver Edition, Be = Basic Edition (anything NOT a TI-83+)
; Cfg = Configuration, Ctrl = Control, Buf = Buffer, Prot = Protection
; M = Memory, Int = Interrupt, Id = Identification, Crstl = Crystal
; Ack = Acknowledge, B = Bit number (without this suffix, it's a bit mask)
; Tmr = Timer, Div = Divided by, Rtc = Real Time Clock, Usb = USB
; Unk = Unknown, Exec = Execution
;
; R:n = RAM page n (always indexed from 0; no more saying RAM page 80h)
; F:nn = Flash page nn
; P:nn = Port nn
; LR:nn = LCD Register nn
pLink           equ 0
pKey            equ 1
pStatus         equ 2
pIntAck         equ 2
pIntMask        equ 3
pIntId          equ 4
pMMapTmrFreq        equ 4
pMPgC           equ 5
pLnkAstBeRead       equ 5
pMPgA           equ 6
pMPgB           equ 7
pLnkAstSeEnable     equ 8
pLnkAstSeStatus     equ 9
pLnkAstSeClkCpu6MHz equ 9
pLnkAstSeInBuf      equ 0Ah
pLnkAstSeClkCpu15MHz    equ 0Ah
pLnkAstSeClkCpu20Mhz    equ 0Bh
pLnkAstSeClkCpu25Mhz    equ 0Ch
pLnkAstSeOutBuf     equ 0Dh
pMPgAHigh       equ 0Eh
pMPgBHigh       equ 0Fh
pLcdCmd         equ 10h
pLcdData        equ 11h
pProtCtrl       equ 14h
pAsicId         equ 15h
pBeFlashExecCtrl    equ 16h
pMd5RegA        equ 18h
pMd5RegB        equ 19h
pMd5RegC        equ 1Ah
pMd5RegD        equ 1Bh
pMd5RegX        equ 1Ch
pMd5Byte0       equ 1Ch
pMd5RegAC       equ 1Dh
pMd5Byte1       equ 1Dh
pMd5RegS        equ 1Eh
pMd5Byte2       equ 1Eh
pMd5FunctionSelect  equ 1Fh
pMd5Byte3       equ 1Fh
pCpuSpeed       equ 20h
pFlashRamType       equ 21h
pFlashExecLowerLimit    equ 22h
pFlashExecUpperLimit    equ 23h
pFlashExecHighBit   equ 24h
pRamExecLowerLimit  equ 25h
pRamExecUpperLimit  equ 26h
pBlockMRemapC       equ 27h
pBlockMRemapB       equ 28h
pLcdDelayCpu6MHz    equ 29h
pLcdDelayCpu15MHz   equ 2Ah
pLcdDelayCpu20MHz   equ 2Bh
pLcdDelayCpu25MHz   equ 2Ch
pCrstlKeepAlive     equ 2Dh
pMemDelay       equ 2Eh
pLcdGenDelay        equ 2Fh
pCrstlTmr1Freq      equ 30h
pCrstlTmr1Cfg       equ 31h
pCrstlTmr1Count     equ 32h
pCrstlTmr2Freq      equ 33h
pCrstlTmr2Cfg       equ 34h
pCrstlTmr2Count     equ 35h
pCrstlTmr3Freq      equ 36h
pCrstlTmr3Cfg       equ 37h
pCrstlTmr3Count     equ 38h
pGpioDirection      equ 39h
pGpioData       equ 3Ah
pRtcCtrl        equ 40h
pRtcSet0        equ 41h
pRtcSet1        equ 42h
pRtcSet2        equ 43h
pRtcSet3        equ 44h
pRtc0           equ 45h ; Reading
pRtc1           equ 46h
pRtc2           equ 47h
pRtc3           equ 48h

; Constants for bitfields
; St = Status, Batt = Batteries
; Rcv = Receive, Snd = Send, Err = Error
; B = "Bit"
; Port 2 (pStatus)
stBattGood      equ 1
stBattGoodB     equ 0
stLcdReady      equ 2
stLcdReadyB     equ 1
stFlashUnlocked     equ 4
stFlashUnlockedB    equ 3
;stIsNot83pSe       equ 5
stIsSeModel     equ 128
stIsSeModelB        equ 7
; Port 2 (pIntAck) Writes
intAckAll       equ 0
; Port 3 (pIntMask)
intOnKey        equ 1
intOnKeyB       equ 0
intTmr1         equ 2
intTmr1B        equ 1
intTmr2         equ 4
intTmr2B        equ 2
intDisableLowPowerMode  equ 8
intDisableLowPowerModeB equ 3
intLinkport     equ 16
intLinkportB        equ 4
intMaskNormal       equ intOnKey+intTmr1+intDisableLowPowerMode
; Port 4 (pIntId) Reads
intOnKeyDown        equ 8
intOnKeyDownB       equ 3
intCrstlTmr1        equ 32
intCrstlTmr1B       equ 5
intCrstlTmr2        equ 64
intCrstlTmr2B       equ 6
intCrstlTmr3        equ 128
intCrstlTmr3B       equ 7
; Port 4 (pMMapTmrFreq) Writes
memMapMode0     equ 0
memMapMode1     equ 1
tmrFreq0        equ 0
tmrFreq1        equ 2
tmrFreq2        equ 4
tmrFreq3        equ 6
battVoltage0        equ 0
battVoltage1        equ 64
battVoltage2        equ 128
battVoltage3        equ 192
; Port 8 (pLnkAstSeEnable)
lnkSeRcv        equ 1
lnkSeRcvB       equ 0
lnkSeSnd        equ 2
lnkSeSndB       equ 1
lnkSeErr        equ 4
lnkSeErrB       equ 2
lnkSeTimeoutDisable equ 64
lnkSeTimeoutDisableB    equ 6
lnkSeForceReset     equ 128
lnkSeForceResetB    equ 7
; Port 9 (pLnkAstSeStatus)
lnkSeRcving     equ 8
lnkSeRcvingB        equ 3
lnkSeRcvd       equ 16
lnkSeRcvdB      equ 4
lnkSeSndReady       equ 32
lnkSeSndReadyB      equ 5
lnkSeSndErr     equ 64
lnkSeSndErrB        equ 6
lnkSeSnding     equ 128
lnkSeSndingB        equ 7
lnkSeIntDisable     equ 128
; Ports 9-C (pLnkAstSeClkCpu6-25MHZ)
lnkSeBitHoldTimeMask    equ 31
lnkSeClockDiv1      equ 0
lnkSeClockDiv2      equ 32
lnkSeClockDiv4      equ 64
lnkSeClockDiv8      equ 96
lnkSeClockDiv16     equ 128
lnkSeClockDiv32     equ 160
lnkSeClockDiv64     equ 192
lnkSeStopClock      equ 224
; Port 14h (pProtCtrl)
protLockFlash       equ 0
protUnlockFlash     equ 1
; Port 15h (pAsicId)
asic83pTA2      equ 33h
asic84pTA2      equ 44h
asic84pTA3      equ 45h
asic84pTA1      equ 55h
; Port 1Fh (pMd5FunctionSelect)
md5FunctionFF       equ 0
md5FunctionGG       equ 1
md5FunctionHH       equ 2
md5FunctionII       equ 3
; Port 20h (pCpuSpeed)
cpu6MHz         equ 0
cpu15MHz        equ 1
cpu20MHz        equ 2   ; left over from TI-83+SE beta
cpu25MHz        equ 3   ; disabled on production models
; Port 21h (pFlashRamType)
flashTypeMask       equ 3
flashType1MB        equ 0
flashType2MB        equ 1
flashType4MB        equ 2
flashTyle8MB        equ 3
ramTypeMask     equ 30h
ramType32K      equ 00h
ramType64K      equ 10h
ramType128K     equ 20h
ramType256K     equ 30h
; Port 24h (pFlashExecHighBit)
flashExecLowerLimitHigh equ 1
flashExecUpperLimitHigh equ 2
; Ports 29h-2Ch (pLcdDelay6-25MHz)
delayFlashEnable    equ 1
delayRamEnable      equ 2
delayCycleCountMask equ 0FCh
delayCycleCountShift    equ 2
delayCycleMin       equ 0Ch
; Port 2D (pCrstlKeepAlive)
crstlTmrKeepAlive   equ 1
crstlClockKeepAlive equ 2
; Port 2E (pMemDelay)
delayFlashExec      equ 1
delayFlashRead      equ 2
delayFlashWrite     equ 4
delayRamExec        equ 10h
delayRamRead        equ 20h
delayRamWrite       equ 40h
; Port 2F (pLcdGenDelay)
cpu1LcdDelay48      equ 0
cpu1LcdDelay112     equ 1
cpu1LcdDelay176     equ 2
cpu1LcdDelay240     equ 3
cpu2LcdDelay48      equ 0
cpu2LcdDelay112     equ 4
cpu2LcdDelay176     equ 8
cpu2LcdDelay240     equ 12
cpu2LcdDelay304     equ 16
cpu2LcdDelay368     equ 20
cpu2LcdDelay432     equ 24
cpu2LcdDelay496     equ 28
cpu3LcdDelay48      equ 0
cpu3LcdDelay112     equ 32
cpu3LcdDelay176     equ 64
cpu3LcdDelay240     equ 96
cpu3LcdDelay304     equ 128
cpu3LcdDelay368     equ 160
cpu3LcdDelay432     equ 192
cpu3LcdDelay496     equ 224
; Crystal Timer Equates
tmrOff          equ 0
tmr11000Hz      equ 40h
tmr1000Hz       equ 41h
tmr100Hz        equ 42h
tmr10Hz         equ 43h
tmr32768Hz      equ 44h
tmr2048Hz       equ 45h
tmr128Hz        equ 46h
tmr8Hz          equ 47h
tmrCpu          equ 80h
tmrCpuDiv2      equ 81h
tmrCpuDiv4      equ 82h
tmrCpuDiv8      equ 84h
tmrCpuDiv16     equ 88h
tmrCpuDiv32     equ 90h
tmrCpuDiv64     equ 0A0h
tmrNullAck      equ 0
tmrLoop         equ 1
tmrInt          equ 2
tmrLoopAndInt       equ 3
; Port 40h (pRtcCtrl)
rtcClear        equ 1
rtcLoadCommand      equ 2


;------ Color LCD Control Registers --------------------------------------------
; Color LCD-Related Stuff
; lr = LCD Register, Disp = Display, Win = Window, Img = Image
; Col = Column, Cur = Cursor
; Use these macros with the bitfield definitions below.
#define lcdHigh(x) x >> 80
#define lcdLow(x) x & 255
lcdWidth = 320
lcdHeight = 240

; Registers
lrSync          equ 0
lrDriverCode        equ 0
lrOutCtrl1      equ 1
lrDriveCtrl     equ 2
lrEntryMode     equ 3
lrRow           equ 20h
lrCol           equ 21h
lrGram          equ 22h
lrWinTop        equ 50h
lrWinBottom     equ 51h
lrWinLeft       equ 52h
lrWinRight      equ 53h
lrBaseImgDispCtrl   equ 61h
lrHorizScroll       equ 6Ah

; Register 1 (lrOutCtrl1) Bitfields
lcdReverseRow       equ 0100h
lcdInterlaced       equ 0400h
; Register 2 (lrDriveCtrl) Bitfields
lcdEOR          equ 0100h
lcdBDivC        equ 0200h
; Register 3 (lrEntryMode) Bitfields
lcdAM           equ 0008h   ; If set, the cursor moves left/right after every write.
lcdCurMoveHoriz     equ lcdAM   ; Alias
lcdRowInc       equ 0010h   ; If unset, the cursor is instead decremented
lcdColInc       equ 0020h   
lcdORG          equ 0080h   ; If set, when window is changed, cursor is reset to corner
lcdBGR          equ 1000h
lcdTRI          equ 4000h   ; If set, 18-bit color is accepted
lcdDFM          equ 8000h   ; Unpacked mode: Write R, G, and B separately if TRI is set
lcdDispCtrlDefault  equ 1038h
; Register 61 (lrBaseImgDispCtrl) Bitfields
lcdREV          equ 0001h   ; If set, reverse color mode
lcdVLE          equ 0002h   ; If set, lrHorizScroll is enabled



;====== System Entry Points ====================================================
;Banked Call Equates/Macros
;-----------------------------------------------
#define bcall(xxxx) rst 28h \ .dw xxxx
#define b_call(xxxx) rst 28h \ .dw xxxx
#define bjump(xxxx) call 50h \ .dw xxxx
rOP1TOOP2   equ 00008h
LCD_BUSY_LOL    equ 0000Bh
rFINDSYM    equ 00010h
rPUSHREALO1 equ 00018h
rMOV9TOOP1  equ 00020h
rBR_CALL    equ 00028h
rFPADD  equ 00030h
BRT_JUMP0   equ 00050h
APP_PUSH_ERRORH equ 00059h
APP_POP_ERRORH  equ 0005Ch
#define AppOnErr(xxxx) ld hl,xxxx \ call 59h
#define AppOffErr call 5Ch

; System routines
_JErrorNo   .equ    4000h
_CallFontHook   .equ    4003h
_CallLocalizeHook   .equ    4006h
_LdHLind    .equ    4009h
_CpHLDE .equ    400Ch
_DivHLBy10  .equ    400Fh
_DivHLByA   .equ    4012h
_KbdScan    .equ    4015h
_GetCSC .equ    4018h
_coorMon    .equ    401Bh
_Mon    .equ    401Eh
_monForceKey    .equ    4021h
_SendKPress .equ    4024h
_JForceCmdNoChar    .equ    4027h
_JForceCmd  .equ    402Ah
_sysErrHandler  .equ    402Dh
_NewContext .equ    4030h
_NewContext0    .equ    4033h
_PPutAwayPrompt .equ    4036h
_PPutAway   .equ    4039h
_PutAway    .equ    403Ch
_maybe_SizeWind .equ    403Fh
_ErrorEP    .equ    4042h
_CallMain   .equ    4045h
_monErrHand .equ    4048h
_maybe_AppInit  .equ    404Bh
_initialize .equ    404Eh
_Min    .equ    4051h
_Max    .equ    4054h
_AbsO1PAbsO2    .equ    4057h
_Intgr  .equ    405Ah
_Trunc  .equ    405Dh
_InvSub .equ    4060h
_Times2 .equ    4063h
_Plus1  .equ    4066h
_Minus1 .equ    4069h
_FPSub  .equ    406Ch
_FPAdd  .equ    406Fh
_maybe_DToR .equ    4072h
_maybe_RToD .equ    4075h
_Cube   .equ    4078h
_TimesPt5   .equ    407Bh
_FPSquare   .equ    407Eh
_FPMult .equ    4081h
_LJRnd  .equ    4084h
_InvOP1SC   .equ    4087h
_InvOP1S    .equ    408Ah
_InvOP2S    .equ    408Dh
_Frac   .equ    4090h
_FPRecip    .equ    4093h
_FPDiv  .equ    4096h
_SqRoot .equ    4099h
_RndGuard   .equ    409Ch
_RnFx   .equ    409Fh
_Int    .equ    40A2h
_Round  .equ    40A5h
_LnX    .equ    40A8h
_LogX   .equ    40ABh
_LJNoRnd    .equ    40AEh
_EToX   .equ    40B1h
_TenX   .equ    40B4h
_SinCosRad  .equ    40B7h
_Sin    .equ    40BAh
_Cos    .equ    40BDh
_Tan    .equ    40C0h
_SinHCosH   .equ    40C3h
_TanH   .equ    40C6h
_CosH   .equ    40C9h
_SinH   .equ    40CCh
_ACosRad    .equ    40CFh
_ATanRad    .equ    40D2h
_ATan2Rad   .equ    40D5h
_ASinRad    .equ    40D8h
_ACos   .equ    40DBh
_ATan   .equ    40DEh
_ASin   .equ    40E1h
_ATan2  .equ    40E4h
_ATanH  .equ    40E7h
_ASinH  .equ    40EAh
_ACosH  .equ    40EDh
_PToR   .equ    40F0h
_RToP   .equ    40F3h
_HLTimes9   .equ    40F6h
_CkOP1Cplx  .equ    40F9h
_CkOP1Real  .equ    40FCh
_Angle  .equ    40FFh
_COP1Set0   .equ    4102h
_CpOP4OP3   .equ    4105h
_Mov9OP2Cp  .equ    4108h
_AbsO1O2Cp  .equ    410Bh
_CpOP1OP2   .equ    410Eh
_OP3ToOP4   .equ    4111h
_OP1ToOP4   .equ    4114h
_OP2ToOP4   .equ    4117h
_OP4ToOP2   .equ    411Ah
_OP3ToOP2   .equ    411Dh
_OP1ToOP3   .equ    4120h
_OP5ToOP2   .equ    4123h
_OP5ToOP6   .equ    4126h
_OP5ToOP4   .equ    4129h
_OP1ToOP2   .equ    412Ch
_OP6ToOP2   .equ    412Fh
_OP6ToOP1   .equ    4132h
_OP4ToOP1   .equ    4135h
_OP5ToOP1   .equ    4138h
_OP3ToOP1   .equ    413Bh
_OP6ToOP5   .equ    413Eh
_OP4ToOP5   .equ    4141h
_OP3ToOP5   .equ    4144h
_OP2ToOP5   .equ    4147h
_OP2ToOP6   .equ    414Ah
_OP1ToOP6   .equ    414Dh
_OP1ToOP5   .equ    4150h
_OP2ToOP1   .equ    4153h
_Mov11B .equ    4156h
_Mov10B .equ    4159h
_Mov9B  .equ    415Ch
_Mov8B  .equ    4162h
_Mov7B  .equ    4165h
_OP2ToOP3   .equ    416Bh
_OP4ToOP3   .equ    416Eh
_OP5ToOP3   .equ    4171h
_OP4ToOP6   .equ    4174h
_Mov9ToOP1  .equ    4177h
_Mov9OP1OP2 .equ    417Ah
_Mov9ToOP2  .equ    417Dh
_MovFrOP1   .equ    4180h
_OP4Set1    .equ    4183h
_OP3Set1    .equ    4186h
_OP2Set8    .equ    4189h
_OP2Set5    .equ    418Ch
_OP2SetA    .equ    418Fh
_OP2Set4    .equ    4192h
_OP2Set3    .equ    4195h
_OP1Set1    .equ    4198h
_OP1Set4    .equ    419Bh
_OP1Set3    .equ    419Eh
_OP3Set2    .equ    41A1h
_OP1Set2    .equ    41A4h
_OP2Set2    .equ    41A7h
_OP2Set1    .equ    41AAh
_Zero16D    .equ    41ADh
_OP5Set0    .equ    41B0h
_OP4Set0    .equ    41B3h
_OP3Set0    .equ    41B6h
_OP2Set0    .equ    41B9h
_OP1Set0    .equ    41BCh
_SetNum0    .equ    41BFh
_ZeroOP1    .equ    41C2h
_ZeroOP2    .equ    41C5h
_ZeroOP3    .equ    41C8h
_ZeroOP .equ    41CBh
_ClrLp  .equ    41CEh
_ShRAcc .equ    41D1h
_ShLAcc .equ    41D4h
_ShR18  .equ    41D7h
_ShR18A .equ    41DAh
_ShR16  .equ    41DDh
_ShR14  .equ    41E0h
_ShL16  .equ    41E3h
_ShL14  .equ    41E6h
_SRDO1  .equ    41E9h
_ShRDRnd    .equ    41ECh
_MantPA .equ    41EFh
_ADDPROP    .equ    41F2h
_ADDPROPLP  .equ    41F5h
_Add16D .equ    41F8h
_Add14D .equ    41FBh
_Sub16D .equ    41FEh
_Sub14D .equ    4201h
_OP2ExOP6   .equ    4204h
_OP5ExOP6   .equ    4207h
_OP1ExOP5   .equ    420Ah
_OP1ExOP6   .equ    420Dh
_OP2ExOP4   .equ    4210h
_OP2ExOP5   .equ    4213h
_OP1ExOP3   .equ    4216h
_OP1ExOP4   .equ    4219h
_OP1ExOP2   .equ    421Ch
_ExLp   .equ    421Fh
_CkOP1C0    .equ    4222h
_CkOP1FP0   .equ    4225h
_CkOP2FP0   .equ    4228h
_PosNo0Int  .equ    422Bh
_CkPosInt   .equ    422Eh
_CkInt  .equ    4231h
_CkOdd  .equ    4234h
_CkOP1M .equ    4237h
_GetCon1    .equ    423Ah
_GetCon .equ    423Dh
_PIDIV2 .equ    4240h
_PIDIV4 .equ    4243h
_TWOPI  .equ    4246h
_PICON  .equ    4249h
_ExpToHex   .equ    424Ch
_OP1ExpToDec    .equ    424Fh
_CkOP2Pos   .equ    4252h
_CkOP1Pos   .equ    4255h
_ClrOP2S    .equ    4258h
_ClrOP1S    .equ    425Bh
_FDiv100    .equ    425Eh
_FDiv10 .equ    4261h
_DecO1Exp   .equ    4264h
_IncO1Exp   .equ    4267h
_IncExp .equ    426Ah
_CkValidNum .equ    426Dh
_GetExp .equ    4270h
_HTimesL    .equ    4273h
_EOP1NotReal    .equ    4276h
_ThetaName  .equ    4279h
_RName  .equ    427Ch
_RegEqName  .equ    427Fh
_RecurNName .equ    4282h
_XName  .equ    4285h
_YName  .equ    4288h
_TName  .equ    428Bh
_RealName   .equ    428Eh
_SetEStoFPS .equ    4291h
_ChkTempDirt    .equ    4294h
_OP1MOP2Exp .equ    4297h
_OP1ExpMDE  .equ    429Ah
_ChkErrBreak    .equ    429Dh
_IsA2ByteTok    .equ    42A0h
_maybe_GetLastEntry .equ    42A3h
_maybe_GetLastEntryPtr  .equ    42A6h
_maybe_RegrClrChng  .equ    42A9h
_ResetWinTop    .equ    42ACh
_IsO1NonTLstOrProg  .equ    42AFh
_IsO1NonTempLst .equ    42B2h
_Is_A_LstOrCLst .equ    42B5h
_Chk_HL_999 .equ    42B8h
_Equ_or_NewEqu  .equ    42BBh
_ErrD_OP1NotPos .equ    42BEh
_ErrD_OP1Not_R  .equ    42C1h
_ErrD_OP1NotPosInt  .equ    42C4h
_ErrD_OP1_LE_0  .equ    42C7h
_ErrD_OP1_0 .equ    42CAh
_FindSym_Get_Size   .equ    42CDh
_Sto_StatVar    .equ    42D0h
_maybe_Rcl_StatVar  .equ    42D3h
_CkOP2Real  .equ    42D6h
_maybe_Get_X_Indirect   .equ    42D9h
_MemChk .equ    42DCh
_CmpPrgNamLen1  .equ    42DFh
_CmpPrgNamLen   .equ    42E2h
_FindProgSym    .equ    42E5h
_ChkFindSym .equ    42E8h
_FindSym    .equ    42EBh
_InsertMem  .equ    42EEh
_InsertMemA .equ    42F1h
_EnoughMem  .equ    42F4h
_CmpMemNeed .equ    42F7h
_maybe_CreatePVar4  .equ    42FAh
_maybe_CreatePVar3  .equ    42FDh
_maybe_CreateVar3   .equ    4300h
_CreateCplx .equ    4303h
_CreateReal .equ    4306h
_CreateTRList   .equ    4309h
_CreateRList    .equ    430Ch
_CreateTCList   .equ    430Fh
_CreateCList    .equ    4312h
_CreateTRMat    .equ    4315h
_CreateRMat .equ    4318h
_CreateTStrng   .equ    431Bh
_CreateStrng    .equ    431Eh
_Create0Equ .equ    4321h
_CreateTEqu .equ    4324h
_CreateEqu  .equ    4327h
_CreateGDB  .equ    432Dh
_CreateProg .equ    4330h
_ChkDel .equ    4333h
_ChkDelA    .equ    4336h
_AdjParser  .equ    4339h
_AdjMath    .equ    433Ch
_AdjM7  .equ    433Fh
_DelMemA    .equ    4342h
_Get_Form_Num   .equ    4345h
_DelVar .equ    4348h
_DelVarIO   .equ    434Bh
_DelMem .equ    434Eh
_DelVar3D   .equ    4351h
_DelVar3C   .equ    4354h
_DelVar3DC  .equ    4357h
_Sym_Prog_Non_T_Lst .equ    435Ah
_AdjSymPtrs .equ    435Dh
_DataSizeA  .equ    4360h
_DataSize   .equ    4363h
_PopMCplxO1 .equ    4366h
_PopMCplx   .equ    4369h
_MovCplx    .equ    436Ch
_PopOP5 .equ    436Fh
_PopOP3 .equ    4372h
_PopOP1 .equ    4375h
_PopRealO6  .equ    4378h
_PopRealO5  .equ    437Bh
_PopRealO4  .equ    437Eh
_PopRealO3  .equ    4381h
_PopRealO2  .equ    4384h
_PopRealO1  .equ    4387h
_PopReal    .equ    438Ah
_FPopCplx   .equ    438Dh
_FPopReal   .equ    4390h
_FPopFPS    .equ    4393h
_DeallocFPS .equ    4396h
_DeallocFPS1    .equ    4399h
_AllocFPS   .equ    439Ch
_AllocFPS1  .equ    439Fh
_PushRealO6 .equ    43A2h
_PushRealO5 .equ    43A5h
_PushRealO4 .equ    43A8h
_PushRealO3 .equ    43ABh
_PushRealO2 .equ    43AEh
_PushRealO1 .equ    43B1h
_PushReal   .equ    43B4h
_PushOP5    .equ    43B7h
_PushOP3    .equ    43BAh
_PushMCplxO3    .equ    43BDh
_PushOP1    .equ    43C0h
_PushMCplxO1_lets_see_if_ti_screws_this_one_up  .equ    43C3h
_PushMCplx  .equ    43C6h
_ExMCplxO1  .equ    43C9h
_Exch9  .equ    43CCh
_CpyTo1FPS11    .equ    43CFh
_CpyTo2FPS5 .equ    43D2h
_CpyTo1FPS5 .equ    43D5h
_CpyTo2FPS6 .equ    43D8h
_CpyTo1FPS6 .equ    43DBh
_CpyTo2FPS7 .equ    43DEh
_CpyTo1FPS7 .equ    43E1h
_CpyTo1FPS8 .equ    43E4h
_CpyTo2FPS8 .equ    43E7h
_CpyTo1FPS10    .equ    43EAh
_CpyTo1FPS9 .equ    43EDh
_CpyTo2FPS4 .equ    43F0h
_CpyTo6FPS3 .equ    43F3h
_CpyTo6FPS2 .equ    43F6h
_CpyTo2FPS3 .equ    43F9h
_CpyCTo1FPS3    .equ    43FCh
_CpyTo1FPS3 .equ    43FFh
_CpyFPS3    .equ    4402h
_CpyTo1FPS4 .equ    4405h
_CpyTo3FPS2 .equ    4408h
_CpyTo5FPST .equ    440Bh
_CpyTo6FPST .equ    440Eh
_CpyTo4FPST .equ    4411h
_CpyTo3FPST .equ    4414h
_CpyTo2FPST .equ    4417h
_CpyTo1FPST .equ    441Ah
_CpyFPST    .equ    441Dh
_CpyStack   .equ    4420h
_CpyTo3FPS1 .equ    4423h
_CpyTo2FPS1 .equ    4426h
_CpyTo1FPS1 .equ    4429h
_CpyFPS1    .equ    442Ch
_CpyTo2FPS2 .equ    442Fh
_CpyTo1FPS2 .equ    4432h
_CpyFPS2    .equ    4435h
_CpyO3ToFPST    .equ    4438h
_CpyO2ToFPST    .equ    443Bh
_CpyO6ToFPST    .equ    443Eh
_CpyO1ToFPST    .equ    4441h
_CpyToFPST  .equ    4444h
_CpyToStack .equ    4447h
_CpyO3ToFPS1    .equ    444Ah
_CpyO5ToFPS1    .equ    444Dh
_CpyO2ToFPS1    .equ    4450h
_CpyO1ToFPS1    .equ    4453h
_CpyToFPS1  .equ    4456h
_CpyO2ToFPS2    .equ    4459h
_CpyO3ToFPS2    .equ    445Ch
_CpyO6ToFPS2    .equ    445Fh
_CpyO1ToFPS2    .equ    4462h
_CpyToFPS2  .equ    4465h
_CpyO5ToFPS3    .equ    4468h
_CpyO2ToFPS3    .equ    446Bh
_CpyO1ToFPS3    .equ    446Eh
_CpyToFPS3  .equ    4471h
_CpyO1ToFPS6    .equ    4474h
_CpyO1ToFPS7    .equ    4477h
_CpyO1ToFPS5    .equ    447Ah
_CpyO2ToFPS4    .equ    447Dh
_CpyO1ToFPS4    .equ    4480h
_ErrNotEnoughMem    .equ    4483h
_FPSMinus9  .equ    4486h
_HLMinus9   .equ    4489h
_maybe_ErrOverflow  .equ    448Ch
_maybe_ErrDivBy0    .equ    448Fh
_ErrSingularMat .equ    4492h
_ErrDomain  .equ    4495h
_ErrIncrement   .equ    4498h
_maybe_ErrNon_Real  .equ    449Bh
_ErrSyntax  .equ    449Eh
_ErrDataType    .equ    44A1h
_ErrArgument    .equ    44A4h
_ErrDimMismatch .equ    44A7h
_ErrDimension   .equ    44AAh
_ErrUndefined   .equ    44ADh
_maybe_ErrMemory    .equ    44B0h
_maybe_ErrInvalid   .equ    44B3h
_ErrBreak   .equ    44B6h
_maybe_ErrStat  .equ    44B9h
_ErrSignChange  .equ    44BCh
_maybe_ErrIterations    .equ    44BFh
_maybe_ErrBadGuess  .equ    44C2h
_maybe_ErrTolTooSmall   .equ    44C5h
_maybe_ErrStatPlot  .equ    44C8h
_maybe_ErrLinkXmit  .equ    44CBh
_JError .equ    44CEh
_noErrorEntry   .equ    44D1h
_pushErrorHandler   .equ    44D4h
_popErrorHandler    .equ    44D7h
_StrCopy    .equ    44DAh
_StrCat .equ    44DDh
_IsInSet    .equ    44E0h
_GEQNAMEA   .equ    44E3h
_RECNAME    .equ    44E6h
___bank_call    .equ    44E9h
___bank_ret .equ    44ECh
___bank_jump    .equ    44EFh
___bank_entry   .equ    44F2h
_PutMap .equ    44F8h
_PutC   .equ    44FBh
_DispHL .equ    44FEh
_PutS   .equ    4501h
_PutPSB .equ    4504h
_PutPS  .equ    4507h
_WPutPS .equ    450Ah
_PutBuf .equ    450Dh
_PutBuf1    .equ    4510h
_WPutC  .equ    4513h
_WPutS  .equ    4516h
_WPutSEOL   .equ    4519h
_WDispEOL   .equ    451Ch
_WHomeUp    .equ    451Fh
_SetNumWindow   .equ    4522h
_NewLine    .equ    4525h
_MoveDown   .equ    4528h
_ScrollUp   .equ    452Bh
_ShrinkWindow   .equ    452Eh
_MoveUp .equ    4531h
_ScrollDown .equ    4534h
_maybe_ClrLCDFull   .equ    4537h
_maybe_ClrLCD   .equ    453Ah
_maybe_ClrScrnFull  .equ    453Dh
_maybe_ClrScrn  .equ    4540h
_maybe_ClrTxtShd    .equ    4543h
_ClrWindow  .equ    4546h
_maybe_EraseEOL .equ    4549h
_EraseEOW   .equ    454Ch
_HomeUp .equ    454Fh
_GetCurLoc  .equ    4552h
_VPutMap    .equ    4555h
_VPutS  .equ    4558h
_VPutSN .equ    455Bh
_VPutSNG    .equ    455Eh
_VPutSNT    .equ    4561h
_maybe_RunIndicOn   .equ    4564h
_maybe_RunIndicOff  .equ    4567h
_SaveCmdShadow  .equ    456Ah
_SaveShadow .equ    456Dh
_maybe_RstrShadow   .equ    4570h
_RstrPartial    .equ    4573h
_RstrCurRow .equ    4576h
_RstrUnderMenu  .equ    4579h
_maybe_RstrBotRow   .equ    457Ch
_GetKeyPress    .equ    457Fh
_GetTokLen  .equ    4582h
_Get_Tok_Strng  .equ    4585h
_GetTokString   .equ    4588h
_maybe_PUTBPATBUF2  .equ    458Bh
_maybe_PUTBPATBUF   .equ    458Eh
_maybe_PUTBPAT  .equ    4591h
_maybe_putcCheckScroll  .equ    4594h
_maybe_DispEOL  .equ    4597h
_maybe_fdispEOL .equ    459Ah
_MakeRowCmd .equ    459Dh
_TOTOSTRP   .equ    45A0h
_DispDone   .equ    45A6h
_FinishOutput   .equ    45A9h
_CurBlink   .equ    45ACh
_CursorOff  .equ    45AFh
_HideCursor .equ    45B2h
_CursorOn   .equ    45B5h
_ShowCursor .equ    45B8h
_KeyToString    .equ    45BBh
_maybe_PullDownChk  .equ    45BEh
_maybe_MenuCatCommon    .equ    45C1h
_LoadCurCat .equ    45C4h
_NCifprgmedmode .equ    45C7h
_LoadMenuNum    .equ    45CAh
_LoadMenuNumL   .equ    45CDh
_maybe_MenuEdKey    .equ    45D0h
_MenCatRet  .equ    45D3h
_NotAlphNum .equ    45D6h
_DispListName   .equ    45E5h
_DispLAlphaName .equ    45E8h
_AbortPrgmode   .equ    45EBh
_Is_FullCntx    .equ    45EEh
_AdrMRow    .equ    45F1h
_AdrMEle    .equ    45F4h
_GetMatOP1A .equ    45F7h
_GetM1ToOP1 .equ    45FAh
_GetM1TOP1A .equ    45FDh
_GetMToOP1  .equ    4600h
_PutToM1A   .equ    4603h
_PutToMa1   .equ    4606h
_PutToMat   .equ    4609h
_Mat_El_Div .equ    460Ch
_CMATFUN    .equ    460Fh
_RowEch_Poly    .equ    4612h
_RowEchelon .equ    4615h
_AdrLEle    .equ    4618h
_GetL1ToOP1 .equ    461Bh
_GetL1TOP1A .equ    461Eh
_GetLToOP1  .equ    4621h
_GetL1ToOP2 .equ    4624h
_GetL1TOP2A .equ    4627h
_GetL2TOP1A .equ    462Ah
_PutToLA1   .equ    462Dh
_PutToL .equ    4630h
_MaxMinLst  .equ    4633h
_LLow   .equ    4636h
_LHigh  .equ    4639h
_LSum   .equ    463Ch
_CumSum .equ    463Fh
_ToFrac .equ    4642h
_SeqSet .equ    4645h
_SeqSolve   .equ    4648h
_Cmp_Num_Init   .equ    464Bh
_BinOPExec  .equ    464Eh
_ExMean1    .equ    4651h
_Set2MVLPtrs    .equ    4654h
_SetMat1    .equ    4657h
_CreateTList    .equ    465Ah
_UnOPExec   .equ    465Dh
_ThreeExec  .equ    4660h
_RestoreErrNo   .equ    4663h
_FourExec   .equ    4666h
_FiveExec   .equ    4669h
_CpyTo2ES1  .equ    466Ch
_CpyTo6ES1  .equ    466Fh
_CpyTo1ES1  .equ    4672h
_CpyTo3ES1  .equ    4675h
_CpyTo3ES2  .equ    4678h
_CpyTo2ES2  .equ    467Bh
_CpyTo1ES2  .equ    467Eh
_CpyTo2ES3  .equ    4681h
_CpyTo1ES3  .equ    4684h
_CpyTo3ES4  .equ    4687h
_CpyTo6ES3  .equ    468Ah
_CpyTo2ES4  .equ    468Dh
_CpyTo1ES4  .equ    4690h
_CpyTo2ES5  .equ    4693h
_CpyTo1ES5  .equ    4696h
_CpyTo4EST  .equ    4699h
_CpyTo2EST  .equ    469Ch
_CpyTo1EST  .equ    469Fh
_CpyTo2ES6  .equ    46A2h
_CpyTo1ES6  .equ    46A5h
_CpyTo2ES7  .equ    46A8h
_CpyTo1ES7  .equ    46ABh
_CpyTo2ES8  .equ    46AEh
_CpyTo1ES8  .equ    46B1h
_CpyTo1ES9  .equ    46B4h
_CpyTo2ES9  .equ    46B7h
_CpyTo2ES10 .equ    46BAh
_CpyTo1ES10 .equ    46BDh
_CpyTo2ES11 .equ    46C0h
_CpyTo1ES11 .equ    46C3h
_CpyTo2ES12 .equ    46C6h
_CpyTo1ES12 .equ    46C9h
_CpyTo2ES13 .equ    46CCh
_CpyTo1ES13 .equ    46CFh
_CpyTo1ES14 .equ    46D2h
_CpyTo1ES16 .equ    46D5h
_CpyTo1ES17 .equ    46D8h
_CpyTo1ES18 .equ    46DBh
_CpyTo1ES15 .equ    46DEh
_CpyTo2ES15 .equ    46E1h
_CpyO1ToEST .equ    46E4h
_CpyO1ToES1 .equ    46E7h
_CpyO6ToES1 .equ    46EAh
_CpyO6ToES3 .equ    46EDh
_CpyO1ToES2 .equ    46F0h
_CpyO2ToES2 .equ    46F3h
_CpyO1ToES3 .equ    46F6h
_CpyO1ToES4 .equ    46F9h
_CpyO1ToES5 .equ    46FCh
_CpyO1ToES6 .equ    46FFh
_CpyO1ToES7 .equ    4702h
_CpyO2ToES4 .equ    4705h
_CpyO2ToES5 .equ    4708h
_CpyO2ToES6 .equ    470Bh
_CpyO2ToES7 .equ    470Eh
_CpyO2ToES8 .equ    4711h
_CpyO2ToES9 .equ    4714h
_CpyO1ToES8 .equ    4717h
_CpyO1ToES9 .equ    471Ah
_CpyO1ToES10    .equ    471Dh
_CpyO1ToES11    .equ    4720h
_CpyO1ToES12    .equ    4723h
_CpyO1ToES13    .equ    4726h
_CpyO1ToES14    .equ    4729h
_EvalF3A    .equ    472Ch
_GetK   .equ    472Fh
_SetTitle   .equ    4732h
_DispVarVal .equ    4735h
_maybe_RecallEd .equ    4738h
_maybe_SetupBuffer  .equ    473Bh
_maybe_CreateNumEditBuf .equ    473Eh
_maybe_CallCommon   .equ    4741h
_CommonKeys .equ    4744h
_Leftmore   .equ    4747h
_fDel   .equ    474Ah
_maybe_fClear   .equ    474Dh
_maybe_fInsDisp .equ    4750h
_maybe_fInsDisp02   .equ    4753h
_SetIndicator   .equ    4756h
_CloseEditBufNoR    .equ    4759h
_ReleaseBuffer  .equ    475Ch
_VarNameToOP1HL .equ    475Fh
_NameToOP1  .equ    4762h
_maybe_numPPutAway  .equ    4765h
_maybe_numRedisp    .equ    4768h
_maybe_numError02   .equ    476Bh
_Load_SFont .equ    476Eh
_SFont_Len  .equ    4771h
_maybe_InitNumVec   .equ    4774h
_SetXXOP1   .equ    4777h
_SetXXOP2   .equ    477Ah
_SetXXXXOP2 .equ    477Dh
_UCLineS    .equ    4780h
_CLine  .equ    4783h
_CLineS .equ    4786h
_XRootY .equ    4789h
_YToX   .equ    478Ch
_Mov18B .equ    47C5h
_DarkLine   .equ    47C8h
_ILine  .equ    47CBh
_IPoint .equ    47CEh
_XYRndBoth  .equ    47D1h
_XYRnd  .equ    47D4h
_CheckTop   .equ    47D7h
_CheckXY    .equ    47DAh
_DarkPnt    .equ    47DDh
_CPointS    .equ    47E0h
_WToV   .equ    47E3h
_VtoWHLDE   .equ    47E6h
_Xitof  .equ    47E9h
_YftoI  .equ    47ECh
_XftoI  .equ    47EFh
_maybe_GrLabels .equ    4807h
_ChkTextCurs    .equ    4876h
_DoRefFlags02   .equ    487Ch
_YRes   .equ    487Fh
_PlotPtXY2  .equ    4882h
_maybe_PDspGrph .equ    488Eh
_maybe_HorizCmd .equ    4891h
_maybe_VertCmd  .equ    4894h
_maybe_LineCmd  .equ    4897h
_maybe_UnLineCmd    .equ    489Ah
_PutSM  .equ    48C4h
_VPutBlank  .equ    48DCh
_DispListNameY  .equ    48E2h
_CurNameLength  .equ    48E5h
_NameToBuf  .equ    48E8h
_JPromptCursor  .equ    48EBh
_BufLeft    .equ    48EEh
_BufRight   .equ    48F1h
_BufInsert  .equ    48F4h
_BufQueueChar   .equ    48F7h
_BufReplace .equ    48FAh
_BufDelete  .equ    48FDh
_BufPeek    .equ    4900h
_BufPeek1   .equ    4903h
_BufPeek2   .equ    4906h
_BufPeek3   .equ    4909h
_BufToBtm   .equ    490Ch
_SetupEditEqu   .equ    490Fh
_BufToTop   .equ    4912h
_IsEditFull .equ    4915h
_IsEditEmpty    .equ    4918h
_IsAtTop    .equ    491Bh
_IsAtBtm    .equ    491Eh
_JCursorFirst   .equ    4924h
_JCursorLast    .equ    4927h
_CursorLeft .equ    492Ah
_JCursorRight   .equ    492Dh
_JCursorUp  .equ    4930h
_CursorDown .equ    4933h
_CursorToOffset .equ    4936h
_InsDisp    .equ    4939h
_FDispBOL1  .equ    493Ch
_FDispBOL   .equ    493Fh
_DispEOW    .equ    4942h
_DispHead   .equ    4945h
_DispTail   .equ    4948h
_PutTokString   .equ    494Bh
_SetupEditCmd   .equ    494Eh
_SetEmptyEditEqu    .equ    4951h
_SetEmptyEditPtr    .equ    4954h
_CloseEditEqu   .equ    4957h
_GetPrevTok .equ    495Ah
_GetKey .equ    495Dh
_LCD_DriverOn   .equ    4963h
_FormDisp   .equ    4969h
_FormMatrix .equ    496Ch
_WScrollLeft    .equ    496Fh
_WScrollUp  .equ    4972h
_WScrollDown    .equ    4975h
_WScrollRight   .equ    4978h
_FormEReal  .equ    497Bh
_FormERealTok   .equ    497Eh
_FormDCplx  .equ    4981h
_FormReal   .equ    4984h
_FormScrollUp   .equ    4987h
_SetWinAbove    .equ    498Ah
_DisarmScroll   .equ    498Dh
_OP1toEdit  .equ    4990h
_RclVarToEdit   .equ    4996h
_RclVarToEditPtr    .equ    4999h
_RclEntryToEdit .equ    499Ch
_RclToQueue .equ    499Fh
_Disp_Interval  .equ    49A5h
_maybe_EditEqu  .equ    49AEh
_maybe_CloseEquField    .equ    49B1h
_maybe_AutoSelect   .equ    49B4h
_DispYEOS   .equ    49B7h
_DispNumEOS .equ    49BAh
_SetupDispEq    .equ    49BDh
_DispForward    .equ    49C0h
_DispYPrompt2   .equ    49C3h
_PopCx  .equ    49CCh
_LoadNoEEntry   .equ    49CFh
_SaveScreen .equ    49D2h
_RetScreen  .equ    49D5h
_RetScreenErr   .equ    49D8h
_CheckSplitFlag .equ    49DBh
_maybe_SolveRedisp  .equ    49DEh
_maybe_SolveDisp    .equ    49E1h
_maybe_ItemName .equ    49E4h
_ConvKeyToTok   .equ    49EDh
_KeyToTokNew2B  .equ    49F0h
_KeyToTok2Byte  .equ    49F3h
_TokToKey   .equ    49F6h
_Send1BErr  .equ    49F9h
_GetVarCmd  .equ    49FCh
_SendVarCmd .equ    49FFh
_KeyScnLnk  .equ    4A05h
_DelRes     .equ    4A0Bh
_ConvLcToLr .equ    4A0Eh
_RedimMat   .equ    4A11h
_IncLstSize .equ    4A14h
_InsertList .equ    4A17h
_DelListEl  .equ    4A1Ah
_EditProg   .equ    4A1Dh
_CreateTemp .equ    4A38h
_BC2NonReal .equ    4A74h
_ErrNonReal .equ    4A77h
_Write_Text .equ    4A7Ah
_ForSeqInit .equ    4A7Dh
_GrphPars   .equ    4A80h
_PlotPars   .equ    4A83h
_ParseInp   .equ    4A86h
_ParseOff   .equ    4A89h
_ParseScan  .equ    4A8Ch
_GetParse   .equ    4A8Fh
_SaveParse  .equ    4A92h
_InitPFlgs  .equ    4A95h
_CkEndLinErr    .equ    4A98h
_OP2Set60   .equ    4A9Bh
_GetStatPtr .equ    4A9Eh
_Cmp_StatPtr    .equ    4AA1h
_VarSysAdr  .equ    4AA4h
_StoSysTok  .equ    4AA7h
_maybe_StoAns   .equ    4AAAh
_StoTheta   .equ    4AADh
_StoR   .equ    4AB0h
_StoY   .equ    4AB3h
_StoN   .equ    4AB6h
_StoT   .equ    4AB9h
_StoX   .equ    4ABCh
_StoOther   .equ    4ABFh
_RclAns .equ    4AC2h
_RclY   .equ    4AC5h
_RclN   .equ    4AC8h
_RclX   .equ    4ACBh
_RclVarSym  .equ    4ACEh
_RclSysTok  .equ    4AD1h
_maybe_StMatEl  .equ    4AD4h
_maybe_StLstVecEl   .equ    4AD7h
_ConvOP1    .equ    4ADAh
_Find_Parse_Formula .equ    4ADDh
_Parse_Formula  .equ    4AE0h
_StrngEnt1  .equ    4AE3h
_PrgRdLp    .equ    4AE6h
_VarEnt .equ    4AE9h
_ParseOnC   .equ    4AECh
_ParseOn    .equ    4AEFh
_ParseCmd   .equ    4AF2h
_StoType    .equ    4AF5h
_CreatePair .equ    4AF8h
_PushNum    .equ    4AFBh
_IncCurPCErrEnd .equ    4AFEh
_ErrEnd .equ    4B01h
_CommaErrF  .equ    4B04h
_CommaErr   .equ    4B07h
_StEqArg2   .equ    4B0Ah
_StEqArg    .equ    4B0Dh
_InpArg .equ    4B10h
_StEqArg3   .equ    4B13h
_NxtFetch   .equ    4B16h
_maybe_CkFetchVar   .equ    4B19h
_maybe_FetchVarA    .equ    4B1Ch
_maybe_FetchVar .equ    4B1Fh
_CkEndLin   .equ    4B22h
_CkEndExp   .equ    4B25h
_CkParsEnd  .equ    4B28h
_StoTypeArg .equ    4B2Bh
_ConvDim    .equ    4B2Eh
_ConvDim00  .equ    4B31h
_maybe_ParsAheadS   .equ    4B37h
_AnsName    .equ    4B3Dh
_StoCmpReals    .equ    4B40h
_Push2BOper .equ    4B46h
_Pop2BOper  .equ    4B49h
_PushOper   .equ    4B4Ch
_PopOper    .equ    4B4Fh
_Find_E_UndefOrArchived .equ    4B52h
_StTmpEq    .equ    4B55h
_FindEOL    .equ    4B58h
_BrkInc .equ    4B5Bh
_IncFetch   .equ    4B5Eh
_CurFetch   .equ    4B61h
_ResetStacks    .equ    4B6Dh
_maybe_Factorial    .equ    4B70h
_YOnOff .equ    4B73h
_EqSelUnsel .equ    4B76h
_ITSOLVER   .equ    4B79h
_GRITSOLVER .equ    4B7Ch
_ITSOLVERB  .equ    4B7Fh
_ITSOLVERNB .equ    4B82h
_ExTest_INT .equ    4B85h
_Dist_Fun   .equ    4B88h
_LogGamma   .equ    4B8Bh
_OutputExpr .equ    4B9Dh
_Next_Y_Style   .equ    4BC7h
_maybe_SetPenX  .equ    4BD3h
_maybe_SetPenY  .equ    4BD6h
_maybe_SetPenT  .equ    4BD9h
_maybe_DispOP1A .equ    4BE2h
_SetTblGraphDraw    .equ    4BEBh
_MemClear   .equ    4C1Bh
_MemSet .equ    4C1Eh
_maybe_ReloadAppEntryVecs   .equ    4C21h
_PointOn    .equ    4C24h
_ExecuteNewPrgm .equ    4C27h
_UserPutMap .equ    4C2Dh
_GetCurrentPageSub  .equ    4C30h
_FindAppUp  .equ    4C33h
_FindAppDn  .equ    4C36h
_maybe_FindApp  .equ    4C39h
_maybe_ExecuteApp   .equ    4C3Ch
_IBounds    .equ    4C4Bh
_IOffset    .equ    4C4Eh
_maybe_DrawCirc2    .equ    4C51h
_maybe_CanAlphIns   .equ    4C54h
_Redisp .equ    4C57h
_GetBaseVer .equ    4C5Ah
_SetFP0 .equ    4C5Dh
_maybe_AppGetCbl    .equ    4C60h
_maybe_AppGetCalc   .equ    4C63h
_SetIgnoreKey   .equ    4C69h
_SetSendThisKeyBack .equ    4C6Ch
_DisableApd .equ    4C6Fh
_EnableAPD  .equ    4C72h
_forcecmd   .equ    4C7Bh
_ApdSetup   .equ    4C7Eh
_HandleLinkActivity .equ    4C84h
_SGetTokString  .equ    4C96h
_LoadPattern    .equ    4C99h
_SStringLength  .equ    4C9Ch
_SPutTokString  .equ    4CF0h
_maybe_ErrCustom1   .equ    4D29h
_maybe_ErrCustom2   .equ    4D2Ch
_ClearRect  .equ    4D44h
_FillRect   .equ    4D4Ah
_RstrCursorAndFlags .equ    4D53h
_DrawRectBorder .equ    4D65h
_EraseRectBorder    .equ    4D6Eh
_DrawRectBorderClear    .equ    4D74h
_VerticalLine   .equ    4D7Dh
_IBoundsFull    .equ    4D80h
_CPoint .equ    4DADh
_maybe_DeleteApp    .equ    4DB0h
_HorizontalLine .equ    4E4Ch
_CreateAppVar   .equ    4E4Fh
_CreateProtProg .equ    4E52h
_CreateVar  .equ    4E55h
_AsmComp    .equ    4E58h
_GetAsmSize .equ    4E5Bh
_SquishPrgm .equ    4E5Eh
_ExecutePrgm    .equ    4E61h
_ChkFindSymAsm  .equ    4E64h
_ParsePrgmName  .equ    4E67h
_CSub   .equ    4E6Ah
_CAdd   .equ    4E6Dh
_CSquare    .equ    4E70h
_CMult  .equ    4E73h
_CRecip .equ    4E76h
_CDiv   .equ    4E79h
_CAbs   .equ    4E7Ch
_CSqrAbs    .equ    4E7Fh
_CSqRoot    .equ    4E82h
_CLN    .equ    4E85h
_CLog   .equ    4E88h
_CTenX  .equ    4E8Bh
_CEtoX  .equ    4E8Eh
_CXrootY    .equ    4E91h
_CYtoX  .equ    4E97h
_Conj   .equ    4E9Ah
_CMltByReal .equ    4E9Dh
_CDivByReal .equ    4EA0h
_CTrunc .equ    4EA3h
_CFrac  .equ    4EA6h
_CIntgr .equ    4EA9h
_SendHeaderPacket   .equ    4EACh
_CancelTransmission .equ    4EAFh
_SendRAMVarData .equ    4EB5h
_SendRAMCmd .equ    4EB8h
_SendPacket .equ    4EBBh
_ReceiveAck .equ    4EBEh
_Send4BytePacket    .equ    4EC1h
_SendDataByte   .equ    4EC4h
_Send4Bytes .equ    4EC7h
_SendAByte  .equ    4ECAh
_SendCByte  .equ    4ECDh
_GetSmallPacket .equ    4ED0h
_GetDataPacket  .equ    4ED3h
_SendAck    .equ    4ED6h
_Get4Bytes  .equ    4ED9h
_Get3Bytes  .equ    4EDCh
_Rec1stByte .equ    4EDFh
_Rec1stByteNC   .equ    4EE2h
_ContinueGetByte    .equ    4EE5h
_RecAByteIO .equ    4EE8h
_ReceiveVar .equ    4EEBh
_ReceiveVarData2    .equ    4EEEh
_ReceiveVarData .equ    4EF1h
_Get4BytesCursor    .equ    4EFDh
_Get4BytesNC    .equ    4F00h
_SendReadyPacket    .equ    4F09h
_SendCmd    .equ    4F24h
_Disp   .equ    4F2Ah
_GetHookByte    .equ    4F36h
_LoadBIndPaged  .equ    4F39h
_CallCursorHook .equ    4F3Ch
_CallRawKeyHook .equ    4F42h
_SetCursorHook  .equ    4F45h
_SetRawKeyHook  .equ    4F4Bh
_ClrCursorHook  .equ    4F4Eh
_ClrRawKeyHook  .equ    4F54h
_AdjustAllHooks .equ    4F5Ah
_CallGetKeyHook .equ    4F5Dh
_SetGetKeyHook  .equ    4F60h
_ClrGetKeyHook  .equ    4F63h
_SetLocalizeHook    .equ    4F78h
_ClrLocalizeHook    .equ    4F7Bh
_Bit_VertSplit  .equ    4F8Dh
_SetWindowHook  .equ    4F96h
_ClrWindowHook  .equ    4F99h
_DelVarArc  .equ    4FABh
_DelVarNoArc    .equ    4FAEh
_SetAllPlots    .equ    4FB1h
_Arc_Unarc  .equ    4FBDh
_ArchiveVar .equ    4FC0h
_UnarchiveVar   .equ    4FC3h
_SetFontHook    .equ    4FC9h
_ClrFontHook    .equ    4FCCh
_SetRegraphHook .equ    4FCFh
_ClrRegraphHook .equ    4FD2h
_CallRegraphHook    .equ    4FD5h
_JForceGraphNoKey   .equ    4FE7h
_JForceGraphKey .equ    4FEAh
_maybe_PowerOff .equ    4FEDh
_GetKeyRetOff   .equ    4FF0h
_FindGroupSym   .equ    4FF3h
_maybe_FillBasePageTable    .equ    4FF6h
_ArcChk .equ    4FF9h
_FlashToRam .equ    4FFCh
_LoadDEIndPaged .equ    4FFFh
_LoadCIndPaged  .equ    5002h
_SetupPagedPtr  .equ    5005h
_PagedGet   .equ    5008h
_SetParserHook  .equ    500Bh
_ClrParserHook  .equ    500Eh
_SetDrawingHook .equ    5017h
_ClrDrawingHook .equ    501Ah
_IPoint_NoHook  .equ    501Dh
_ILine_NoHook   .equ    5020h
_CLineS_NoHook  .equ    5023h
_DispMenuTitle  .equ    504Ah
_GetBCOffsetIX  .equ    506Eh
_ForceFullScreen    .equ    5074h
_GetVariableData    .equ    5077h
_FindSwapSector .equ    507Ah
_CopyFlashPage  .equ    507Dh
_FindAppNumPages    .equ    5080h
_HLMinus5   .equ    5083h
_SendArcPacket  .equ    5086h
_FormBase   .equ    508Fh
_Fake_Chk_Batt_Low  .equ    5098h
_Fake_Chk_Batt_Low2 .equ    509Bh
_Arc_Unarc2 .equ    509Eh
_maybe_GetAppBasePage   .equ    50A1h
_SetExSpeed .equ    50A4h
_WriteToFlash   .equ    50B0h
_SendUSBData    .equ    50D4h
_GetVarCmdUSB   .equ    50DDh
_InvertTextInsMode  .equ    5104h
_DispHeader .equ    5110h
_GetRelSeconds  .equ    5125h
_DisableClock   .equ    5128h
_EnableClock    .equ    512Bh
_GetDayOfWeek   .equ    512Eh
_GetDate    .equ    5131h
_FormDate   .equ    5134h
_GetDateFmt .equ    5137h
_FormDateString .equ    513Ah
_GetTime    .equ    513Dh
_FormTime   .equ    5140h
_GetTimeFmt .equ    5143h
_FormTimeString .equ    5146h
_GetClockStatus .equ    5149h
_SetDateMakeList    .equ    514Ch
_SetDateFmt .equ    514Fh
_SetTimeMakeList    .equ    5152h
_SetTimeFmt .equ    5155h
_GetAbsSeconds  .equ    5158h
_AbsSecondsToTimeList   .equ    515Bh
_ClrWindowAndFlags  .equ    5161h
_ResetLists .equ    5167h
_ExecLib    .equ    5173h
_OpenLib    .equ    517Fh
_WaitEnterKey   .equ    5182h
_Disp32 .equ    51AFh
_RectBorder .equ    5215h
_GetTokenCompatLevel    .equ    5221h
_oldbc5254  .equ    5233h
_oldbc5257  .equ    5236h
_oldbc525A  .equ    5239h
_oldbc525D  .equ    523Ch
_oldbc5260  .equ    523Fh
_SetUSBHook .equ    5269h
_ClrUSBHook .equ    526Ch
_InitUSBDevice  .equ    526Fh
_KillUSBPeripheral  .equ    5272h
_GraphLine  .equ    527Bh
_oldbc52E1  .equ    52C0h
_DelVarSym  .equ    52C3h
_FindAppUpNoCase    .equ    52C6h
_FindAppDnNoCase    .equ    52C9h
_maybe_DeleteApp_Link   .equ    52CFh
_maybe_CheckAppRestrictions .equ    52DBh
_maybe_DispAppRestrictions  .equ    52DEh
_oldbc5308  .equ    52E7h
_oldbc530B  .equ    52EAh
_CheckPort81Bit6    .equ    52EDh
_oldbc5311  .equ    52F0h
_PolarEquToOP1  .equ    52F3h
_maybe_GetRestrictionsOptions   .equ    52FCh
_maybe_DispResetComplete    .equ    52FFh
_FindAppCustom  .equ    5305h
_GetFastCircCoord   .equ    53B0h
_WPutSN .equ    5428h
_Load_LFont .equ    5482h
_DrawStatusBar  .equ    54F4h
_DrawBatteryIndicator   .equ    5503h
_GetBatteryLevel    .equ    5509h
_DrawStatusBarInfo  .equ    555Ah
_BacklightPowerOnInit   .equ    555Dh
_BacklightExitPowerSave .equ    5560h
_BacklightEnterPowerSave    .equ    5563h
_BacklightAdjust2x  .equ    5566h
_SendScreenContents .equ    5575h
_SetTextFGColor_3   .equ    5581h
_SetTextFGColor_2   .equ    5584h
_SetTextFGColor .equ    5587h
_VPutPS .equ    558Ah
_SetLCDWindowAndPos .equ    5596h
_SetLCDWindowFull   .equ    5599h
_DrawThickRectBorder    .equ    55A2h
_GetColorValue  .equ    5626h
_MD5Final   .equ    8018h
_BigNumCompare  .equ    801Eh
_WriteAByteUnsafe   .equ    8021h
_EraseFlash .equ    8024h
_FindFirstCertificateField  .equ    8027h
_ZeroToCertificate  .equ    802Ah
_GetCertificateEnd  .equ    802Dh
_FindGroupedField   .equ    8030h
_ATimesE    .equ    8042h
_ATimesDE   .equ    8045h
_DivHLByE   .equ    8048h
_DivHLByDE  .equ    804Bh
_LoadAIndPaged  .equ    8051h
_FlashToRAM2    .equ    8054h
_GetCertificateStart    .equ    8057h
_GetFieldSize   .equ    805Ah
_FindSubField   .equ    805Dh
_EraseCertificateSector .equ    8060h
_ClearFullScreen    .equ    806Ch
_FindOSHeaderSubField   .equ    8075h
_FindNextCertificateField   .equ    8078h
_GetCalcSerial  .equ    807Eh
_EraseFlashPage .equ    8084h
_WriteFlashUnsafe   .equ    8087h
_DispBootVer    .equ    808Ah
_MD5Init    .equ    808Dh
_MD5Update  .equ    8090h
_FindAppHeaderSubField  .equ    80ABh
_Div32ByDE  .equ    80B1h
_FindSimpleGroupedField .equ    80B4h
_GetBootVer .equ    80B7h
_GetHWVer   .equ    80BAh
_WriteAByteSafe .equ    80C6h
_WriteFlashSafe .equ    80C9h
_SetAppLimit    .equ    80CFh


;====== System Variables =======================================================
appData         equ 8000h   ; scrap 256
ramCode         equ 8100h   ; scrap 119
appSearchPage   equ 08283h  ; scrap
tempSwapArea        equ 82B3h   ; scrap 232
; There should be MD5 buffer stuff here?
kbdScanCode equ 0844Dh
kbdLGSC equ 0844Eh
kbdPSC  equ 0844Fh
kbdWUR  equ 08450h
kbdDebncCnt equ 08451h
kbdKey  equ 08452h
kbdGetKy    equ 08453h
keyExtend   equ 08454h
apdSubTimer equ 08456h
apdTimer    equ 08457h
curTime equ 08458h
curRow  equ 08459h
curCol  equ 0845Ah
lFont_record    equ 08463h  ; expanded to 36 bytes
sFont_record    equ 08487h
OP1 equ 084AEh
OP2 equ 084B9h
OP3 equ 084C4h
OP4 equ 084CFh
OP5 equ 084DAh
OP6 equ 084E5h
; progToEdit ought to somewhere around here?
nameBuff    equ 08519h
iMathPtr1   equ 08525h  ; scrap RAM
iMathPtr2   equ 08527h
iMathPtr3   equ 08529h
iMathPtr4   equ 0852Bh
iMathPtr5   equ 0852Dh
iMathPtr6   equ 0852Fh
iMathPtr7   equ 08531h
iMathPtr8   equ 08533h
chkDelPtr1  equ 08535h
chkDelPtr2  equ 08537h
insDelPtr   equ 08539h
maybe_fOutDat   equ 0853Dh
asm_data_ptr1   equ 08543h
asm_data_ptr2   equ 08545h
textShadow  equ 08560h  ; expanded to 260 bytes
textShadCur equ 08664h
textShadTop equ 08666h
textShadAlph    equ 08667h
textShadIns equ 08668h
; monQueue?
cxCurApp    equ 08676h
onSP    equ 08686h
; Scrap RAM here? Should be promptStuff, varType, menuStuff, menuFlagBackups
MenuCurrent equ 086A9h
maybe_ioPrompt  equ 08700h
ioFlag  equ 0871Bh
header  equ 0871Fh
ioData  equ 08728h
ioNewData   equ 08734h
; Where is progCurrent (120), linkStuff (120)?
penCol  equ 08782h
penRow  equ 08784h
rclQueue    equ 08785h
errNo   equ 08789h
errSP   equ 0878Ah
saveSScreen equ 8798h
; Here lies asm_prgm_size, somewhere.  Hopefully.
usermem_offset  equ 08B22h
asm_prgm_size   equ 08B22h
flags   equ 08B26h
statVars        equ 8C1Ch   ; scrap 531 (clear b_call(_DelRes))
; Where's infVars and anovaf_vars?
maybe_curGX2    equ 08E6Eh
maybe_lcdTallP  equ 08F02h
maybe_pixWide_m_1   equ 08F05h
; Bunch of graph vars, probably, possibly usable as scrap RAM
orgXmin equ 09250h
panShift    equ 09259h
uXmin   equ 09264h
uXmax   equ 0926Dh
uXscl   equ 09276h
uYmin   equ 0927Fh
uYmax   equ 09288h
uYscl   equ 09291h
uThetMin    equ 0929Ah
uThetMax    equ 092A3h
uThetStep   equ 092ACh
uTmin   equ 092B5h
uTmax   equ 092BEh
uTStep  equ 092C7h
uPlotStart  equ 092D0h
unMax   equ 092D9h
uu0 equ 092E2h
uv0 equ 092EBh
unMin   equ 092F4h
uu02    equ 092FDh
uv02    equ 09306h
uw02    equ 0932Ah
Xmin    equ 09336h
Xmax    equ 0933Fh
Xscl    equ 09348h
Ymin    equ 09351h
Ymax    equ 0935Ah
Yscl    equ 09363h
ThetaMin    equ 0936Ch
ThetaMax    equ 09375h
ThetaStep   equ 0937Eh
TminPar equ 09387h
TmaxPar equ 09390h
Tstep   equ 09399h
PlotStart   equ 093A2h
nMax    equ 093ABh
u0  equ 093B4h
v0  equ 093BDh
nMin    equ 093C6h
u02 equ 093CFh
v02 equ 093D8h
w02 equ 093FCh
un1 equ 09405h
vn1 equ 09417h
wn1 equ 09429h
XFact   equ 09525h
YFact   equ 0952Eh
deltaX  equ 09538h
deltaY  equ 09541h
shortX  equ 0954Ah
shortY  equ 09553h
maybe_XOutSym   equ 09574h
XOutDat equ 09576h
maybe_YOutSym   equ 09578h
YOutDat equ 0957Ah
inputSym    equ 0957Ch
maybe_inputDat  equ 0957Eh
TblMin  equ 097EBh
TblStep equ 097F4h
; There's probably a lot of usable scrap RAM here?
ES  equ 0983Eh
plotSScreen equ 987Ch
parseVar    equ 09B8Eh
begPC   equ 09B97h
curPC   equ 09B99h
endPC   equ 09B9Bh
cmdShadow   equ 09BAAh ; Also expanded
editTop equ 09CB4h
editCursor  equ 09CB6h
editTail    equ 09CB8h
editBtm equ 09CBAh
maybe_editSym   equ 09CC6h
maybe_editDat   equ 09CC8h
; ?
modePtr equ 09D84h
winTop  equ 09D86h
winBtm  equ 09D87h
winLeftEdge equ 09D88h
winLeft equ 09D89h
winAbove    equ 09D8Bh
fmtDigits   equ 09D91h
fmtString   equ 09D92h
maybe_fmtMatSym equ 09DE9h
maybe_fmtMatMem equ 09DEBh
EQS equ 09DEDh
maybe_tSymPtr1  equ 09DF9h
maybe_tSymPtr2  equ 09DFBh
maybe_chkDelPtr3    equ 09DFDh
maybe_chkDelPtr4    equ 09DFFh
FPS equ 09E05h
OPBase  equ 09E07h
OPS equ 09E09h
pTempCnt    equ 09E0Bh
pTemp   equ 09E0Fh
progPtr equ 09E11h
maybe_newDataPtr    equ 09E13h
pagedCount  equ 09E15h
pagedPN equ 09E16h
pagedGetPtr equ 09E17h
pagedPutPtr equ 09E19h
pagedBuf    equ 09E1Bh
localLanguage       equ 9E54h
cursorHookPtr   equ 09E5Dh
rawKeyHookPtr   equ 09E65h
getKeyHookPtr   equ 09E69h
windowHookPtr   equ 09E71h
fontHookPtr equ 09E7Dh
regraphHookPtr  equ 09E81h
drawingHookPtr  equ 09E85h
localizeHookPtr equ 09EADh
usbHookPtr  equ 09EB5h
; ?
DBKeyScanCode   equ 09FABh
DBKeyRptCtr equ 09FACh
textFGColor equ 0A015h
textBGColor equ 0A017h
; ?
drawBGColor equ 0A036h
drawFGColor equ 0A038h
; What's here? 1491 bytes of scrap RAM?
userMem equ 0A60Bh
symTable    equ 0FD9Eh



;====== Flags ==================================================================
kbdFlags    .equ    00h
trigFlags   .equ    00h
trigDeg .equ    2
kbdSCR  .equ    3
kbdKeyPress .equ    4
doneFlags   EQU  0      ;display "Done"
donePrgm    EQU    5    ; 1=display "Done" after prgm
ioDelFlag   EQU  0
inDelete    EQU    0    ;1 = DELETE SCREEN 

editFlags   EQU  1
editOpen    EQU    2    ; 1=edit buffer is open
monFlags    EQU  1      ;monitor flags
monAbandon  EQU    4    ; 1=don't start any long process
                ; in put away (#715)
plotFlags   .equ    02h
plotTrace   .equ    0
plotDisp    .equ    2
plotLoc     EQU    1    ; 0=bkup & display, 1=display only

grfModeFlags    EQU  2      ;graph mode settings
grfFuncM    EQU    4    ; 1=function graph
grfPolarM   EQU    5    ; 1=polar graph
grfParamM   EQU    6    ; 1=parametric graph
grfRecurM       EQU    7        ; 1=RECURSION graph

graphFlags  .equ    03h
graphProg   .equ    1
graphDraw   EQU    0    ; 0=graph is valid, 1=redraw graph

grfDBFlags  EQU  4
grfDot      EQU    0    ; 0=line, 1=dot
grfSimul    EQU    1    ; 0=sequential, 1=simultaneous
grfGrid     EQU    2    ; 0=no grid, 1=grid
grfPolar    EQU    3    ; 0=rectangular, 1=polar coordinates
grfNoCoord  EQU    4    ; 0=display coordinates, 1=off
grfNoAxis   EQU    5    ; 0=axis, 1=no axis
grfLabel    EQU    6    ; 0=off, 1=axis label

textFlags   .equ    05h
textEraseBelow  .equ    1
textScrolled    .equ    2
textInverse .equ    3
textInsMode .equ    4

ParsFlag2   .equ    07h
numOP1  .equ    0

apdFlags    .equ    08h
maybe_preClrForMode .equ    0
apdAble .equ    2
apdRunning  .equ    3

onFlags .equ    09h
appWantAlphaUpDn    .equ    0
onInterrupt .equ    4
statsValid  .equ    6
onRunning   EQU  3      ; 1 = calculator is running
statANSDISP EQU  7      ; 1=display stat results

fmtFlags    EQU  10     ;numeric formatflags
fmtExponent EQU    0    ; 1=show exponent, 0=no exponent
fmtEng      EQU    1    ; 1=engineering notion, 0=scientific
fmtHex      EQU    2    ; 1=hexadecimal
fmtOct      EQU    3    ; 1=octal
fmtBin      EQU    4    ; 1=binary

numMode         EQU  10
fmtReal         EQU    5
fmtRect         EQU    6
fmtPolar        EQU    7
;???????????????????????
realMode        EQU    5
rectMode        EQU    6
polarMode       EQU    7

fmtBaseMask     EQU  00011100B      ; mask to base flags
fmtBaseShift    EQU  2          ; offset to base flags

fmtOverride EQU  11     ;copy of fmtFlags with conversion override
plotFlag2   .equ    11h
expr_param  .equ    3
expr_writing    .equ    4

curFlags    .equ    0Ch
fmtEditFlags    .equ    0Ch
fmtEdit .equ    0
curAble .equ    2
curOn   .equ    3
curLock .equ    4

appFlags    .equ    0Dh
appTextSave .equ    1
appAutoScroll   .equ    2
appCurGraphic   .equ    5
appCurWord  .equ    6
appWantIntrpt   EQU    0        ; 1=want ON key interrupts
appMenus    EQU    3        ; 1=process keys that bring up menus
                    ; 0=check Lock menu flag
appLockMenus    EQU    4        ; 1=ignore menu keys
                    ; 0=switch to home screen and bring up menu
appExit     EQU    7        ; 1=application handles [EXIT] key itself

appWantIntrptF  EQU 1<<appWantIntrpt
appTextSaveF    EQU 1<<appTextSave
appAutoScrollF  EQU 1<<appAutoScroll
appMenusF   EQU 1<<appMenus
appLockMenusF   EQU 1<<appLockMenus
appCurGraphicF  EQU 1<<appCurGraphic
appCurWordF EQU 1<<appCurWord
appExitF    EQU 1<<appExit

seqFlags    .equ    0Fh
webMode         EQU    0             ; 0 = NORMAL SEQ MODE, 1 = WEB MODE
webVert         EQU    1             ;
sequv           EQU    2             ; U vs V
seqvw           EQU    3             ; V vs W
sequw           EQU    4             ; U vs W

promptFlags EQU  17     ;prompt line flags
promptEdit  EQU    0    ; 1=editing in prompt buffer

shiftFlags  .equ    12h
indicOnly   .equ    2
shift2nd    .equ    3
shiftAlpha  .equ    4
shiftLwrAlph    .equ    5
shiftALock  .equ    6
indicRun    EQU    0    ; 1=run indicator ON
indicInUse  EQU    1    ; indicator save area in use=1, free=0
shiftKeepAlph   EQU    7    ; 1=cannot cancel alpha shift

tblFlags        EQU  19     ;table flags.
autoFill        EQU    4    ; 1=prompt, 0=fillAuto
autoCalc        EQU    5    ; 1=prompt, 0=CalcAuto
reTable         EQU    6    ; 0=table is ok, 1=must recompute table.

sGrFlags    .equ    14h
grfSplit    .equ    0
vertSplit   .equ    1
grfSplitOverride    .equ    3
write_on_graph  .equ    4
grfSChanged     EQU    2    ; 1=Graph just changed Split <-> normal
g_style_active  EQU    5    ; 1 = GRAPH STYLES ARE ENABLED, USE THEM
cmp_mod_box     EQU    6    ; 1 = DOING MOD BOX PLOT COMPUTATION
textWrite       EQU    7    ; 1 = Small font writes to buffer

smartFlags  .equ    17h
smartGraph  .equ    0
smartGraph_inv  .equ    1

gkKeyRepeating  .equ    0

remoteKeyPress  .equ    5

maybe_fullScrnTextDraw  .equ    4

appRetOffKey    .equ    7

appWantDiagonalKeys .equ    0

fontFlags   .equ    32h
fracDrawLFont   .equ    2
drawLFont   .equ    6

ignoreBPLink    .equ    3

getKeyHookActive    .equ    0
rawKeyHookActive    .equ    5
cursorHookActive    .equ    7

localizeHookActive  .equ    1
windowHookActive    .equ    2
fontHookActive  .equ    5
regraphHookActive   .equ    6
drawingHookActive   .equ    7

usbHookActive   .equ    0

DBKeyFlags  .equ    3Dh
haveDBKey   .equ    5
HWLinkErrF  .equ    7

textDrawColor   .equ    4


newIndicFlags   EQU  21
extraIndic      EQU    0
saIndic         EQU    1

newFlags2       EQU  22
noRestores      EQU  5

;
;----------------------------------------------------------------------
; Note: Fix these Equates if smartFlags are moved                            
;----------------------------------------------------------------------
;
smarter_mask    EQU  3
smarter_test    EQU  1
smartGraph      EQU  0

more_Flags  EQU 26

No_Del_Stat EQU 2
;----------------------------------------------------------------------
;           Available for assm programming
;----------------------------------------------------------------------

asm_Flag1       EQU  33          ; CODING
asm_Flag2       EQU  34          ; CODING
asm_Flag3       EQU  35          ; CODING

;----------------------------------------------------------------------
;
getSendFlg  EQU  36
comFailed   EQU  1        ; 1 = Get/Send Communication Failed
;
appLwrCaseFlag  EQU  36
lwrCaseActive   EQU  3
;
apiFlg3     EQU  42 
;
apiFlg4     EQU  43
fullScrnDraw    EQU    2    ; DRAW INTO LAST ROW/COL OF SCREEN

groupFlags  EQU  38
inGroup     EQU  1      ;1 = IN GROUP CONTEXT


xapFlag0        EQU  46      ; external app flags
xapFlag1        EQU  47
xapFlag2        EQU  48
xapFlag3        EQU  49

;fontFlags  EQU  50
;fracDrawLFont  EQU    2
fracTallLFont   EQU    3
customFont  EQU    7  

plotFlag3   EQU  60  
bufferOnly  EQU    0
useFastCirc EQU    4

;
varTypeMask EQU  1FH             
varGraphRef EQU    6

;
;====== Keypress Equates =======================================================
;           Keyboard key names
;
;
kRight          EQU       001h
kLeft           EQU       002h
kUp             EQU       003h
kDown           EQU       004h
kEnter          EQU       005h
kAlphaEnter     EQU       006h
kAlphaUp        EQU       007h
kAlphaDown      EQU       008h
kClear          EQU       009h
kDel            EQU       00Ah
kIns            EQU       00Bh
kRecall         EQU       00Ch
kLastEnt        EQU       00Dh
kBOL            EQU       00Eh
kEOL            EQU       00Fh
;
kSelAll         EQU       010h
kUnselAll       EQU       011h
kLtoTI82        EQU       012h
kBackup         EQU       013h
kRecieve        EQU       014h
kLnkQuit        EQU       015h
kTrans          EQU       016h
kRename         EQU       017h
kOverw          EQU       018h
kOmit           EQU       019h
kCont           EQU       01Ah
kSendID         EQU       01Bh
kSendSW         EQU       01Ch
kYes            EQU       01Dh
kNoWay          EQU       01Eh
kvSendType      EQU       01Fh
kOverWAll       EQU       020h
;
kNo             EQU       025h
kKReset         EQU       026h
kApp            EQU       027h
;
kDoug           EQU       028h
kListflag       EQU       029h
menuStart       EQU       02Bh
;
kAreYouSure     EQU       02Bh
kAppsMenu       EQU       02Ch
kPrgm           EQU       02Dh
kZoom           EQU       02Eh
kDraw           EQU       02Fh
kSPlot          EQU       030h
kStat           EQU       031h
kMath           EQU       032h
kTest           EQU       033h
kChar           EQU       034h
kVars           EQU       035h
kMem            EQU       036h
kMatrix         EQU       037h
kDist           EQU       038h
kAngle          EQU       039h
kList           EQU       03Ah
kCalc           EQU       03Bh
kFin            EQU       03Ch
;
menuEnd         EQU       kFin
;
kCatalog        EQU       03Eh
kInputDone      EQU       03Fh
kOff            EQU       kInputDone
;
kQuit           EQU       040h
appStart        EQU       kQuit

;
kLinkIO         EQU       041h
kMatrixEd       EQU       042h
kStatEd         EQU       043h
kGraph          EQU       044h
kMode           EQU       045h
kPrgmEd         EQU       046h        ; PROGRAM EDIT
kPrgmCr         EQU       047h        ; PROGRAM CREATE
kWindow         EQU       048h        ; RANGE EDITOR
kYequ           EQU       049h        ; EQUATION EDITOR
kTable          EQU       04Ah        ; TABLE EDITOR
kTblSet         EQU       04Bh        ; TABLE SET
kChkRAM         EQU       04Ch        ; CHECK RAM
kDelMem         EQU       04Dh        ; DELETE MEM
kResetMem       EQU       04Eh        ; RESET MEM
kResetDef   EQU   04Fh        ; RESET DEFAULT
kPrgmInput      EQU       050h        ; PROGRAM INPUT
kZFactEd        EQU       051h        ; ZOOM FACTOR EDITOR
kError          EQU       052h        ; ERROR
kSolveTVM       EQU       053h        ; TVM SOLVER
kSolveRoot  EQU   054h        ; SOLVE EDITOR
kStatP          EQU       055h        ; stat plot
kInfStat    EQU   056h        ; Inferential Statistic
kFormat         EQU       057h        ; FORMAT
kExtApps        EQU       058h        ; External Applications.     NEW
kNewApps        EQU       059h        ; New Apps for Cerberus.
;
append          EQU       kNewApps
;
echoStart1      EQU       05Ah
;
kTrace          EQU       05Ah
kZFit           EQU       05Bh
kZIn            EQU       05Ch
kZOut           EQU       05Dh
kZPrev          EQU       05Eh
kBox            EQU       05Fh
kDecml          EQU       060h
kSetZm          EQU       061h
kSquar          EQU       062h
kStd            EQU       063h
kTrig           EQU       064h
kUsrZm          EQU       065h
kZSto           EQU       066h
kZInt           EQU       067h
kZStat          EQU       068h
;
echoStart2      EQU       069h
;
kSelect         EQU       069h
kCircl          EQU       06Ah
kClDrw          EQU       06Bh
kLine           EQU       06Ch
kPen            EQU       06Dh
kPtChg          EQU       06Eh
kPtOff          EQU       06Fh
kPtOn           EQU       070h
kVert           EQU       071h
kHoriz          EQU       072h
kText           EQU       073h
kTanLn          EQU       074h
;
kEval           EQU       075h
kInters         EQU       076h
kDYDX           EQU       077h
kFnIntg         EQU       078h
kRootG          EQU       079h
kDYDT           EQU       07Ah
kDXDT           EQU       07Bh
kDRDo           EQU       07Ch
KGFMin          EQU       07Dh
KGFMax          EQU       07Eh
;
;
EchoStart       EQU       07Fh
;
kListName       EQU       07Fh
kAdd            EQU       080h
kSub            EQU       081h
kMul            EQU       082h
kDiv            EQU       083h
kExpon          EQU       084h
kLParen         EQU       085h
kRParen         EQU       086h
kLBrack         EQU       087h
kRBrack         EQU       088h
kShade          EQU       089h
kStore          EQU       08Ah
kComma          EQU       08Bh
kChs            EQU       08Ch
kDecPnt         EQU       08Dh
k0              EQU       08Eh
k1              EQU       08Fh
k2              EQU       090h
k3              EQU       091h
k4              EQU       092h
k5              EQU       093h
k6              EQU       094h
k7              EQU       095h
k8              EQU       096h
k9              EQU       097h
kEE             EQU       098h
kSpace          EQU       099h
kCapA           EQU       09Ah
kCapB           EQU       09Bh
kCapC           EQU       09Ch
kCapD           EQU       09Dh
kCapE           EQU       09Eh
kCapF           EQU       09Fh
kCapG           EQU       0A0h
kCapH           EQU       0A1h
kCapI           EQU       0A2h
kCapJ           EQU       0A3h
kCapK           EQU       0A4h
kCapL           EQU       0A5h
kCapM           EQU       0A6h
kCapN           EQU       0A7h
kCapO           EQU       0A8h
kCapP           EQU       0A9h
kCapQ           EQU       0AAh
kCapR           EQU       0ABh
kCapS           EQU       0ACh
kCapT           EQU       0ADh
kCapU           EQU       0AEh
kCapV           EQU       0AFh
kCapW           EQU       0B0h
kCapX           EQU       0B1h
kCapY           EQU       0B2h
kCapZ           EQU       0B3h
kVarx           EQU       0B4h
kPi             EQU       0B5h
kInv            EQU       0B6h
kSin            EQU       0B7h
kASin           EQU       0B8h
kCos            EQU       0B9h
kACos           EQU       0BAh
kTan            EQU       0BBh
kATan           EQU       0BCh
kSquare         EQU       0BDh
kSqrt           EQU       0BEh
kLn             EQU       0BFh
kExp            EQU       0C0h
kLog            EQU       0C1h
kALog           EQU       0C2h
kToABC          EQU       0C3h
;
kClrTbl         EQU       0C4h
;
kAns            EQU       0C5h
kColon          EQU       0C6h
;
kNDeriv         EQU       0C7h
kFnInt          EQU       0C8h
kRoot           EQU       0C9h
;
kQuest          EQU       0CAh
kQuote          EQU       0CBh
kTheta          EQU       0CCh
kIf             EQU       0CDh
kThen           EQU       0CEh
kElse           EQU       0CFh
kFor            EQU       0D0h
kWhile          EQU       0D1h
kRepeat         EQU       0D2h
kEnd            EQU       0D3h
kPause          EQU       0D4h
kLbl            EQU       0D5h
kGoto           EQU       0D6h
kISG            EQU       0D7h
kDSL            EQU       0D8h
kMenu           EQU       0D9h
kExec           EQU       0DAh
kReturn         EQU       0DBh
kStop           EQU       0DCh
kInput          EQU       0DDh
kPrompt         EQU       0DEh
kDisp           EQU       0DFh
kDispG          EQU       0E0h
kDispT          EQU       0E1h
kOutput         EQU       0E2h
kGetKey         EQU       0E3h
kClrHome        EQU       0E4h
kPrtScr         EQU       0E5h
kSinH           EQU       0E6h
kCosH           EQU       0E7h
kTanH           EQU       0E8h
kASinH          EQU       0E9h
kACosH          EQU       0EAh
kATanH          EQU       0EBh
kLBrace         EQU       0ECh
kRBrace         EQU       0EDh
kI              EQU       0EEh
kCONSTeA        EQU       0EFh
kPlot3          EQU       0F0h
kFMin           EQU       0F1h
kFMax           EQU       0F2h
kL1A            EQU       0F3h
kL2A            EQU       0F4h
kL3A            EQU       0F5h
kL4A            EQU       0F6h
kL5A            EQU       0F7h
kL6A            EQU       0F8h
kunA            EQU       0F9h
kvnA            EQU       0FAh
kwnA            EQU       0FBh
;
;-------------------------------------------------------------------------------
;       THIS KEY MEANS THAT IT IS A 2 BYTE KEYCODE
;       THERE ARE 2 OF THESE KEYS; BE CAREFUL WITH USAGE
;
kExtendEcho2    EQU       0FCh
;
;-------------------------------------------------------------------------------
;       THIS KEY MEANS THAT THE KEY PRESS IS ONE THAT ECHOS
;       INTO A BUFFER, AND IT IS A 2 BYTE KEY CODE, GO LOOK AT
;       (EXTECHO) FOR THE KEY VALUE 
;
kExtendEcho     EQU       0FEh
;

kE1BT           EQU       0

kDrawInv        EQU       kE1BT
kDrawF          EQU       kE1BT+1
kPixelOn        EQU       kE1BT+2
kPixelOff       EQU       kE1BT+3
kPxlTest        EQU       kE1BT+4
kRCGDB          EQU       kE1BT+5
kRCPic          EQU       kE1BT+6
kSTGDB          EQU       kE1BT+7
kSTPic          EQU       kE1BT+8
kAbs            EQU       kE1BT+9
kTEqu           EQU       kE1BT+10    ; ==
kTNoteQ         EQU       kE1BT+11    ; <>
kTGT            EQU       kE1BT+12    ; >
kTGTE           EQU       kE1BT+13    ; > =
kTLT            EQU       kE1BT+14    ; <
kTLTE           EQU       kE1BT+15    ; < =

kAnd            EQU       kE1BT+16
kOr             EQU       kE1BT+17
kXor            EQU       kE1BT+18
kNot            EQU       kE1BT+19

kLR1            EQU       kE1BT+20

kXRoot          EQU       kE1BT+21
kCube           EQU       kE1BT+22
kCbRt           EQU       kE1BT+23    ; Cube ROOT
kToDec          EQU       kE1BT+24
;
kCubicR         EQU       kE1BT+25
kQuartR         EQU       kE1BT+26
;
kPlot1          EQU       kE1BT+27
kPlot2          EQU       kE1BT+28
;

kRound          EQU       kE1BT+29
kIPart          EQU       kE1BT+30
kFPart          EQU       kE1BT+31
kInt            EQU       kE1BT+32

kRand           EQU       kE1BT+33
kNPR            EQU       kE1BT+34
kNCR            EQU       kE1BT+35
kXFactorial     EQU       kE1BT+36

kRad            EQU       kE1BT+37
kDegr           EQU       kE1BT+38    ; DEGREES CONV
kAPost          EQU       kE1BT+39
kToDMS          EQU       kE1BT+40
kRToPo          EQU       kE1BT+41    ; R
kRToPr          EQU       kE1BT+42
kPToRx          EQU       kE1BT+43
kPToRy          EQU       kE1BT+44

kRowSwap        EQU       kE1BT+45
kRowPlus        EQU       kE1BT+46
kTimRow         EQU       kE1BT+47
kTRowP          EQU       kE1BT+48

kSortA          EQU       kE1BT+49
kSortD          EQU       kE1BT+50
kSeq            EQU       kE1BT+51

kMin            EQU       kE1BT+52
kMax            EQU       kE1BT+53
kMean           EQU       kE1BT+54
kMedian         EQU       kE1BT+55
kSum            EQU       kE1BT+56
kProd           EQU       kE1BT+57

kDet            EQU       kE1BT+58
kTransp         EQU       kE1BT+59
kDim            EQU       kE1BT+60
kFill           EQU       kE1BT+61
kIdent          EQU       kE1BT+62
kRandm          EQU       kE1BT+63
kAug            EQU       kE1BT+64

kOneVar         EQU       kE1BT+65
kTwoVar         EQU       kE1BT+66
kLR             EQU       kE1BT+67
kLRExp          EQU       kE1BT+68
kLRLn           EQU       kE1BT+69
kLRPwr          EQU       kE1BT+70
kMedMed         EQU       kE1BT+71
kQuad           EQU       kE1BT+72
kClrLst         EQU       kE1BT+73
kHist           EQU       kE1BT+74
kxyLine         EQU       kE1BT+75
kScatter        EQU       kE1BT+76

kmRad           EQU       kE1BT+77
kmDeg           EQU       kE1BT+78
kmNormF         EQU       kE1BT+79
kmSci           EQU       kE1BT+80
kmEng           EQU       kE1BT+81
kmFloat         EQU       kE1BT+82

kFix            EQU       kE1BT+83
kSplitOn        EQU       kE1BT+84
kFullScreen     EQU       kE1BT+85
kStndrd         EQU       kE1BT+86
kParam          EQU       kE1BT+87
kPolar          EQU       kE1BT+88
kSeqG           EQU       kE1BT+89
kAFillOn        EQU       kE1BT+90
kAFillOff       EQU       kE1BT+91
kACalcOn        EQU       kE1BT+92
kACalcOff       EQU       kE1BT+93
kFNOn           EQU       kE1BT+94
kFNOff          EQU       kE1BT+95

kPlotsOn        EQU       kE1BT+96
kPlotsOff       EQU       kE1BT+97

kPixelChg       EQU       kE1BT+98

kSendMBL        EQU       kE1BT+99
kRecvMBL        EQU       kE1BT+100

kBoxPlot        EQU       kE1BT+101
kBoxIcon        EQU       kE1BT+102
kCrossIcon      EQU       kE1BT+103
kDotIcon        EQU       kE1BT+104

kE2BT           EQU       kE1BT+105

kSeqential      EQU       kE2BT
kSimulG         EQU       kE2BT+1
kPolarG         EQU       kE2BT+2
kRectG          EQU       kE2BT+3
kCoordOn        EQU       kE2BT+4
kCoordOff       EQU       kE2BT+5
kDrawLine       EQU       kE2BT+6
kDrawDot        EQU       kE2BT+7
kAxisOn         EQU       kE2BT+8
kAxisOff        EQU       kE2BT+9
kGridOn         EQU       kE2BT+10
kGridOff        EQU       kE2BT+11
kLblOn          EQU       kE2BT+12
kLblOff         EQU       kE2BT+13

kL1             EQU       kE2BT+14
kL2             EQU       kE2BT+15
kL3             EQU       kE2BT+16
kL4             EQU       kE2BT+17
kL5             EQU       kE2BT+18
kL6             EQU       kE2BT+19

;
;-------------------------------------------------------------------------------
;           These keys are layed on top of existing keys to
;           enable localization in the inferential stats editor
;
kinfData        EQU       kL1
kinfStats       EQU       kL1+1
kinfYes         EQU       kL1+2
kinfNo          EQU       kL1+3
kinfCalc        EQU       kL1+4
kinfDraw        EQU       kL1+5
kinfAlt1ne      EQU       kL1+6
kinfAlt1lt      EQU       kL1+7
kinfAlt1gt      EQU       kL1+8
kinfAlt2ne      EQU       kL1+9
kinfAlt2lt      EQU       kL1+10
kinfAlt2gt      EQU       kL1+11
kinfAlt3ne      EQU       kL1+12
kinfAlt3lt      EQU       kL1+13
kinfAlt3gt      EQU       kL1+14
kinfAlt4ne      EQU       kL1+15
kinfAlt4lt      EQU       kL1+16
kinfAlt4gt      EQU       kL1+17
kinfAlt5ne      EQU       kL1+18
kinfAlt5lt      EQU       kL1+19
kinfAlt5gt      EQU       kL1+20
kinfAlt6ne      EQU       kL1+21
kinfAlt6lt      EQU       kL1+22
kinfAlt6gt      EQU       kL1+23
;
;
kMatA           EQU       kE2BT+20
kMatB           EQU       kE2BT+21
kMatC           EQU       kE2BT+22
kMatD           EQU       kE2BT+23
kMatE           EQU       kE2BT+24

kXmin           EQU       kE2BT+25
kXmax           EQU       kE2BT+26
kXscl           EQU       kE2BT+27
kYmin           EQU       kE2BT+28
kYmax           EQU       kE2BT+29
kYscl           EQU       kE2BT+30
kTmin           EQU       kE2BT+31
kTmax           EQU       kE2BT+32
kTStep          EQU       kE2BT+33
kOmin           EQU       kE2BT+34
kOmax           EQU       kE2BT+35
kOStep          EQU       kE2BT+36
ku0             EQU       kE2BT+37
kv0             EQU       kE2BT+38
knMin           EQU       kE2BT+39
knMax           EQU       kE2BT+40
kDeltaY         EQU       kE2BT+41
kDeltaX         EQU       kE2BT+42

kZXmin          EQU       kE2BT+43
kZXmax          EQU       kE2BT+44
kZXscl          EQU       kE2BT+45
kZYmin          EQU       kE2BT+46
kZYmax          EQU       kE2BT+47
kZYscl          EQU       kE2BT+48
kZTmin          EQU       kE2BT+49
kZTmax          EQU       kE2BT+50
kZTStep         EQU       kE2BT+51
kZOmin          EQU       kE2BT+52
kZOmax          EQU       kE2BT+53
kZOStep         EQU       kE2BT+54
kZu0            EQU       kE2BT+55
kZv0            EQU       kE2BT+56
kZnMin          EQU       kE2BT+57
kZnMax          EQU       kE2BT+58

kDelLast        EQU       kE2BT+59
kSinReg         EQU       kE2BT+60
kConstE         EQU       kE2BT+61

kPic1           EQU       kE2BT+62
kPic2           EQU       kE2BT+63
kPic3           EQU       kE2BT+64

kDelVar         EQU       kE2BT+65
kGetCalc        EQU       kE2BT+66
kRealM          EQU       kE2BT+67
kPolarM         EQU       kE2BT+68
kRectM          EQU       kE2BT+69
kuv             EQU       kE2BT+70    ; U vs V
kvw             EQU       kE2BT+71    ; V vs W
kuw             EQU       kE2BT+72    ; U vs W
kFinPMTend      EQU       kE2BT+73
kFinPMTbeg      EQU       kE2BT+74
;
kGraphStyle     EQU       kE2BT+75
;
kExprOn         EQU       kE2BT+76
kExprOff        EQU       kE2BT+77
kStatA          EQU       kE2BT+78
kStatB          EQU       kE2BT+79
kStatC          EQU       kE2BT+80
kCorr           EQU       kE2BT+81
kStatD          EQU       kE2BT+82
kStatE          EQU       kE2BT+83
kRegEq          EQU       kE2BT+84
kMinX           EQU       kE2BT+85
kQ1             EQU       kE2BT+86
kMD             EQU       kE2BT+87
kQ3             EQU       kE2BT+88
kMaxX           EQU       kE2BT+89
kStatX1         EQU       kE2BT+90
kStatY1         EQU       kE2BT+91
kStatX2         EQU       kE2BT+92
kStatY2         EQU       kE2BT+93
kStatX3         EQU       kE2BT+94
kStatY3         EQU       kE2BT+95
kTblMin         EQU       kE2BT+96
kTblStep        EQU       kE2BT+97
kSetupLst       EQU       kE2BT+98
kClrAllLst      EQU       kE2BT+99
kLogistic       EQU       kE2BT+100
kZTest          EQU       kE2BT+101
kTTest          EQU       kE2BT+102
k2SampZTest     EQU       kE2BT+103
k2SampTTest     EQU       kE2BT+104
k1PropZTest     EQU       kE2BT+105
k2PropZTest     EQU       kE2BT+106
kChiTest        EQU       kE2BT+107
k2SampFTest     EQU       kE2BT+108
kZIntVal        EQU       kE2BT+109
kTIntVal        EQU       kE2BT+110
k2SampTInt      EQU       kE2BT+111
k2SampZInt      EQU       kE2BT+112
k1PropZInt      EQU       kE2BT+113
k2PropZInt      EQU       kE2BT+114
kDNormal        EQU       kE2BT+115
kInvNorm        EQU       kE2BT+116
kDT             EQU       kE2BT+117
kChi            EQU       kE2BT+118
kDF             EQU       kE2BT+119
kBinPDF         EQU       kE2BT+120
kBinCDF         EQU       kE2BT+121
kPoiPDF         EQU       kE2BT+122
kPoiCDF         EQU       kE2BT+123
kun             EQU       kE2BT+124
kvn             EQU       kE2BT+125
kwn             EQU       kE2BT+126
kRecn           EQU       kE2BT+127
kPlotStart      EQU       kE2BT+128
kZPlotStart     EQU       kE2BT+129   ; recursion n
kXFact          EQU       kE2BT+130   ; PlotStart
kYFact          EQU       kE2BT+131   ; ZPlotStart
kANOVA          EQU       kE2BT+132   ; XFact
kMaxY           EQU       kE2BT+133   ; YFact
kWebOn          EQU       kE2BT+134   ; MinY
kWebOff         EQU       kE2BT+135   ; MaxY
kTblInput       EQU       kE2BT+136   ; WEB ON
kGeoPDF         EQU       kE2BT+137   ; WEB OFF
kGeoCDF         EQU       kE2BT+138   ; WEB OFF
kShadeNorm      EQU       kE2BT+139
kShadeT         EQU       kE2BT+140
kShadeChi       EQU       kE2BT+141
kShadeF         EQU       kE2BT+142
kPlotStep       EQU       kE2BT+143
kZPlotStep      EQU       kE2BT+144
kLinRegtTest    EQU       kE2BT+145
KMGT        EQU   kE2BT+146   ; VERT SPLIT
kSelectA        EQU       kE2BT+147
kZFitA          EQU       kE2BT+148
kE2BT_End       EQU       kZFitA
;
;
;-------------------------------------------------------------------------------
;           More 2 Byte Keys 

kE2BT2      EQU       0

kGDB1       EQU       kE2BT2
kGDB2       EQU       kE2BT2+1
kGDB3       EQU       kE2BT2+2
kY1     EQU       kE2BT2+3
kY2     EQU       kE2BT2+4
kY3     EQU       kE2BT2+5
kY4     EQU       kE2BT2+6
kY5     EQU       kE2BT2+7
kY6     EQU       kE2BT2+8
kY7     EQU       kE2BT2+9
kY8     EQU       kE2BT2+10
kY9     EQU       kE2BT2+11
kY0     EQU       kE2BT2+12
kX1T        EQU       kE2BT2+13
kY1T        EQU       kE2BT2+14
kX2T        EQU       kE2BT2+15
kY2T        EQU       kE2BT2+16
kX3T        EQU       kE2BT2+17
kY3T        EQU       kE2BT2+18
kX4T        EQU       kE2BT2+19
kY4T        EQU       kE2BT2+20
kX5T        EQU       kE2BT2+21
kY5T        EQU       kE2BT2+22
kX6T        EQU       kE2BT2+23
kY6T        EQU       kE2BT2+24
kR1     EQU       kE2BT2+25
kR2     EQU       kE2BT2+26
kR3     EQU       kE2BT2+27
kR4     EQU       kE2BT2+28
kR5     EQU       kE2BT2+29
kR6     EQU       kE2BT2+30
kGDB4       EQU       kE2BT2+31
kGDB5       EQU       kE2BT2+32
kGDB6       EQU       kE2BT2+33
kPic4       EQU       kE2BT2+34
kPic5       EQU       kE2BT2+35
kPic6       EQU       kE2BT2+36
kGDB7       EQU       kE2BT2+37
kGDB8       EQU       kE2BT2+38
kGDB9       EQU       kE2BT2+39
kGDB0       EQU       kE2BT2+40
kPic7       EQU       kE2BT2+41
kPic8       EQU       kE2BT2+42
kPic9       EQU       kE2BT2+43
kPic0       EQU       kE2BT2+44
kStatN      EQU       kE2BT2+45
kXMean      EQU       kE2BT2+46
kConj       EQU       kE2BT2+47
kReal       EQU       kE2BT2+48
kFAngle     EQU       kE2BT2+49
kLCM        EQU       kE2BT2+50
kGCD        EQU       kE2BT2+51
kRandInt    EQU       kE2BT2+52
kRandNorm   EQU       kE2BT2+53
kToPolar    EQU       kE2BT2+54
kToRect     EQU       kE2BT2+55
kYMean      EQU       kE2BT2+56
kStdX       EQU       kE2BT2+57
kStdX1      EQU       kE2BT2+58
kw0     EQU       kE2BT2+59
kMatF       EQU       kE2BT2+60
kMatG       EQU       kE2BT2+61
kMatRH      EQU       kE2BT2+62
kMatI       EQU       kE2BT2+63
kMatJ       EQU       kE2BT2+64
kYMean1     EQU       kE2BT2+65
kStdY       EQU       kE2BT2+66
kStdY1      EQU       kE2BT2+67
kMatToLst   EQU       kE2BT2+68
kLstToMat   EQU       kE2BT2+69
kCumSum     EQU       kE2BT2+70
kDeltaLst   EQU       kE2BT2+71
kStdDev     EQU       kE2BT2+72
kVariance   EQU       kE2BT2+73
kLength     EQU       kE2BT2+74
kEquToStrng EQU       kE2BT2+75
kStrngToEqu EQU       kE2BT2+76
kExpr       EQU       kE2BT2+77
kSubStrng   EQU       kE2BT2+78
kInStrng    EQU       kE2BT2+79
kStr1       EQU       kE2BT2+80
kStr2       EQU       kE2BT2+81
kStr3       EQU       kE2BT2+82
kStr4           EQU       kE2BT2+83
kStr5           EQU       kE2BT2+84
kStr6           EQU       kE2BT2+85
kStr7           EQU       kE2BT2+86
kStr8           EQU       kE2BT2+87
kStr9           EQU       kE2BT2+88
kStr0           EQU       kE2BT2+89
kFinN           EQU       kE2BT2+90
kFinI           EQU       kE2BT2+91
kFinPV          EQU       kE2BT2+92
kFinPMT         EQU       kE2BT2+93
kFinFV          EQU       kE2BT2+94
kFinPY          EQU       kE2BT2+95
kFinCY          EQU       kE2BT2+96
kFinFPMT        EQU       kE2BT2+97
kFinFI          EQU       kE2BT2+98
kFinFPV         EQU       kE2BT2+99
kFinFN          EQU       kE2BT2+100
kFinFFV         EQU       kE2BT2+101
kFinNPV         EQU       kE2BT2+102
kFinIRR         EQU       kE2BT2+103
kFinBAL         EQU       kE2BT2+104
kFinPRN         EQU       kE2BT2+105
kFinINT         EQU       kE2BT2+106
kSumX           EQU       kE2BT2+107
kSumX2          EQU       kE2BT2+108
kFinToNom       EQU       kE2BT2+109
kFinToEff       EQU       kE2BT2+110
kFinDBD         EQU       kE2BT2+111
kStatVP         EQU       kE2BT2+112
kStatZ          EQU       kE2BT2+113
kStatT          EQU       kE2BT2+114
kStatChi        EQU       kE2BT2+115
kStatF          EQU       kE2BT2+116
kStatDF         EQU       kE2BT2+117
kStatPhat       EQU       kE2BT2+118
kStatPhat1      EQU       kE2BT2+119
kStatPhat2      EQU       kE2BT2+120
kStatMeanX1     EQU       kE2BT2+121
kStatMeanX2     EQU       kE2BT2+122
kStatStdX1      EQU       kE2BT2+123
kStatStdX2      EQU       kE2BT2+124
kStatStdXP      EQU       kE2BT2+125
kStatN1         EQU       kE2BT2+126
kStatN2         EQU       kE2BT2+127
kStatLower      EQU       kE2BT2+128
kStatUpper      EQU       kE2BT2+129
kuw0            EQU       kE2BT2+130
kImag           EQU       kE2BT2+131
kSumY           EQU       kE2BT2+132
kXres           EQU       kE2BT2+133
kStat_s         EQU       kE2BT2+134
kSumY2          EQU       kE2BT2+135
kSumXY          EQU       kE2BT2+136
kuXres          EQU       kE2BT2+137
kModBox     EQU       kE2BT2+138
kNormProb   EQU       kE2BT2+139
kNormalPDF      EQU       kE2BT2+140
kTPDF           EQU       kE2BT2+141
kChiPDF         EQU       kE2BT2+142
kFPDF           EQU       kE2BT2+143
kMinY           EQU       kE2BT2+144   ; MinY
kRandBin        EQU       kE2BT2+145
kRef            EQU       kE2BT2+146
kRRef           EQU       kE2BT2+147
kLRSqr          EQU       kE2BT2+148
kBRSqr          EQU       kE2BT2+149
kDiagOn         EQU       kE2BT2+150
kDiagOff        EQU       kE2BT2+151
kun1            EQU       kE2BT2+152   ; FOR RCL USE WHEN GOTTEN FROM 82
kvn1            EQU       kE2BT2+153   ; FOR RCL USE WHEN GOTTEN FROM 82
;
k83_00End       EQU kvn1            ;end of original keys...
kArchive        EQU     k83_00End + 1
kUnarchive      EQU     k83_00End + 2
kAsm            EQU     k83_00End + 3   ;_Asm(
kAsmPrgm        EQU     k83_00End + 4   ; AsmPrgm
kAsmComp        EQU     k83_00End + 5   ; AsmComp(
;
kcapAAcute  EQU       k83_00End + 6
kcapAGrave  EQU       k83_00End + 7
kcapACaret  EQU       k83_00End + 8
kcapADier   EQU       k83_00End + 9
kaAcute     EQU       k83_00End + 10
kaGrave     EQU       k83_00End + 11
kaCaret     EQU       k83_00End + 12
kaDier      EQU       k83_00End + 13
kcapEAcute  EQU       k83_00End + 14
kcapEGrave  EQU       k83_00End + 15
kcapECaret  EQU       k83_00End + 16
kcapEDier   EQU       k83_00End + 17
keAcute     EQU       k83_00End + 18
keGrave     EQU       k83_00End + 19
keCaret     EQU       k83_00End + 20
keDier      EQU       k83_00End + 21
kcapIAcute  EQU       k83_00End + 22
kcapIGrave  EQU       k83_00End + 23
kcapICaret  EQU       k83_00End + 24
kcapIDier   EQU       k83_00End + 25
kiAcute     EQU       k83_00End + 26
kiGrave     EQU       k83_00End + 27
kiCaret     EQU       k83_00End + 28
kiDier      EQU       k83_00End + 29
kcapOAcute  EQU       k83_00End + 30
kcapOGrave  EQU       k83_00End + 31
kcapOCaret  EQU       k83_00End + 32
kcapODier   EQU       k83_00End + 33
koAcute     EQU       k83_00End + 34
koGrave     EQU       k83_00End + 35
koCaret     EQU       k83_00End + 36
koDier      EQU       k83_00End + 37
kcapUAcute  EQU       k83_00End + 38
kcapUGrave  EQU       k83_00End + 39
kcapUCaret  EQU       k83_00End + 40
kcapUDier   EQU       k83_00End + 41
kuAcute     EQU       k83_00End + 42
kuGrave     EQU       k83_00End + 43
kuCaret     EQU       k83_00End + 44
kuDier      EQU       k83_00End + 45
kcapCCed    EQU       k83_00End + 46
kcCed       EQU       k83_00End + 47
kcapNTilde  EQU       k83_00End + 48
knTilde     EQU       k83_00End + 49
kaccent     EQU       k83_00End + 50
kgrave      EQU       k83_00End + 51
kdieresis   EQU       k83_00End + 52
kquesDown   EQU       k83_00End + 53
kexclamDown     EQU       k83_00End + 54
kalpha          EQU       k83_00End + 55
kbeta           EQU      k83_00End +  56
kgamma          EQU      k83_00End +  57
kcapDelta       EQU      k83_00End +  58
kdelta          EQU      k83_00End +  59
kepsilon        EQU      k83_00End +  60
klambda         EQU      k83_00End +  61
kmu             EQU      k83_00End +  62
kpi2            EQU      k83_00End +  63
krho            EQU      k83_00End +  64
kcapSigma       EQU      k83_00End +  65
ksigma          EQU      k83_00End +  66
ktau            EQU      k83_00End +  67
kphi            EQU      k83_00End +  68
kcapOmega       EQU      k83_00End +  69
kphat           EQU      k83_00End +  70
kchi2           EQU      k83_00End +  71
kstatF2         EQU      k83_00End +  72
kLa     EQU     k83_00End + 73
kLb     EQU     k83_00End + 74
kLc     EQU     k83_00End + 75
kLd     EQU     k83_00End + 76
kLe     EQU     k83_00End + 77
kLf     EQU     k83_00End + 78
kLg     EQU     k83_00End + 79
kLh     EQU     k83_00End + 80
kLi     EQU     k83_00End + 81
kLj     EQU     k83_00End + 82
kLk     EQU     k83_00End + 83
kLl     EQU     k83_00End + 84
kLm     EQU     k83_00End + 85
kLsmalln    EQU    k83_00End +  86
kLo     EQU    k83_00End +  87
kLp     EQU    k83_00End +  88
kLq     EQU    k83_00End +  89
kLsmallr    EQU    k83_00End +  90
kLs     EQU    k83_00End +  91
kLt     EQU    k83_00End +  92
kLu     EQU    k83_00End +  93
kLv     EQU    k83_00End +  94
kLw     EQU    k83_00End +  95
kLx     EQU    k83_00End +  96
kLy     EQU    k83_00End +  97
kLz     EQU    k83_00End +  98
kGarbageC   EQU  k83_00End +  99 ; GarbageCollect
;
kE2BT2_End  EQU     kGarbageC

;====== Scan Codes =============================================================
skDown          EQU 01h
skLeft          EQU 02h
skRight         EQU 03h
skUp            EQU 04h
skEnter         EQU 09h
skAdd           EQU 0Ah
skSub           EQU 0Bh
skMul           EQU 0Ch
skDiv           EQU 0Dh
skPower         EQU 0Eh
skClear         EQU 0Fh
skChs           EQU 11h
sk3             EQU 12h
sk6             EQU 13h
sk9             EQU 14h
skRParen        EQU 15h
skTan           EQU 16h
skVars          EQU 17h
skDecPnt        EQU 19h
sk2             EQU 1Ah
sk5             EQU 1Bh
sk8             EQU 1Ch
skLParen        EQU 1Dh
skCos           EQU 1Eh
skPrgm          EQU 1Fh
skStat          EQU 20h
sk0             EQU 21h
sk1             EQU 22h
sk4             EQU 23h
sk7             EQU 24h
skComma         EQU 25h
skSin           EQU 26h
skMatrix        EQU 27h
skGraphvar      EQU 28h
skStore         EQU 2Ah
skLn            EQU 2Bh
skLog           EQU 2Ch
skSquare        EQU 2Dh
skRecip         EQU 2Eh
skMath          EQU 2Fh
skAlpha         EQU 30h
skGraph         EQU 31h
skTrace         EQU 32h
skZoom          EQU 33h
skWindow        EQU 34h
skYEqu          EQU 35h
sk2nd           EQU 36h
skMode          EQU 37h
skDel           EQU 38h


;====== Tokens =================================================================
EOSSTART        EQU       0
;
;
;   DISPLAY CONVERSIONS COME IMMEDIATELY BEFORE 'TSTORE'
;
DCONV   EQU     01h
;
tToDMS          EQU       DCONV       ; 01h
tToDEC          EQU       DCONV+1     ; 02h
tToAbc          EQU       DCONV+2     ; 03h    > A b/c
;
tStore          EQU       DCONV+3     ; 04h Lstore                01
;
tBoxPlot        EQU       05h
;
BRACKS          EQU       06h
;
tLBrack         EQU       BRACKS      ; 06h '['
tRBrack         EQU       BRACKS+1    ; 07h ']'
tLBrace         EQU       BRACKS+2    ; 08h '{'
tRBrace         EQU       BRACKS+3    ; 09h '}'
;
tPOST1          EQU       BRACKS+4
;
tFromRad        EQU       tPOST1       ; 0Ah Lradian
tFromDeg        EQU       tPOST1+1     ; 0Bh Ldegree
tRecip          EQU       tPOST1+2     ; 0Ch Linverse
tSqr            EQU       tPOST1+3     ; 0Dh Lsquare
tTrnspos        EQU       tPOST1+4     ; 0Eh Ltranspose
tCube           EQU       tPOST1+5     ; 0Fh '^3'
;
tLParen         EQU       10h         ; 10h '('
tRParen         EQU       11h         ; 11h ')'
;
;
IMUN            EQU       12h
;
tRound          EQU       IMUN        ; 12h 'round'
tPxTst          EQU       IMUN+1      ; 13h 'PXL-TEST'
tAug            EQU       IMUN+2      ; 14h 'aug'
tRowSwap        EQU       IMUN+3      ; 15h 'rSwap'
tRowPlus        EQU       IMUN+4      ; 16h 'rAdd'
tmRow           EQU       IMUN+5      ; 17h 'multR'
tmRowPlus       EQU       IMUN+6      ; 18h 'mRAdd'
tMax            EQU       IMUN+7      ; 19h 'max'
tMin            EQU       IMUN+8      ; 1Ah 'min'
tRToPr          EQU       IMUN+9      ; 1Bh 'R>Pr
tRToPo          EQU       IMUN+10     ; 1Ch 'R>Po
tPToRx          EQU       IMUN+11     ; 1Dh 'P>Rx
tPToRy          EQU       IMUN+12     ; 1Eh 'P>Ry
tMedian         EQU       IMUN+13     ; 1Fh 'MEDIAN
tRandM          EQU       IMUN+14     ; 20h 'randM'
tMean           EQU       IMUN+15     ; 21h
tRoot           EQU       IMUN+16     ; 22h 'ROOT'
tSeries         EQU       IMUN+17     ; 23h 'seq'
tFnInt          EQU       IMUN+18     ; 24h 'fnInt'
tNDeriv         EQU       IMUN+19     ; 25h 'fnIr'
tEvalF          EQU       IMUN+20     ; 26h
tFmin           EQU       IMUN+21     ; 27h
tFmax           EQU       IMUN+22     ; 28h
;
tEOSEL          EQU       IMUN+23
;
tSpace          EQU       tEOSEL       ; 29h   ' '
tString         EQU       tEOSEL+1     ; 2Ah  '"'
tComma          EQU       tEOSEL+2     ; 2Bh  ','
;
;
tii             EQU       2Ch         ; i
;
;-------------------------------------------------------------------------------
;           Postfix Functions
tPost           EQU       2Dh
;
tFact           EQU       tPost       ; 2Dh '!'
;
tCubicR         EQU       2Eh
tQuartR         EQU       2Fh

;-------------------------------------------------------------------------------
;           Number Tokens 
NUMS            EQU       30h
;
t0              EQU       NUMS        ; 30h
t1              EQU       NUMS+1      ; 31h
t2              EQU       NUMS+2      ; 32h
t3              EQU       NUMS+3      ; 33h
t4              EQU       NUMS+4      ; 34h
t5              EQU       NUMS+5      ; 35h
t6              EQU       NUMS+6      ; 36h
t7              EQU       NUMS+7      ; 37h
t8              EQU       NUMS+8      ; 38h
t9              EQU       NUMS+9      ; 39h
tDecPt          EQU       NUMS+10     ; 3Ah
tee             EQU       NUMS+11     ; 3Bh
;
;-------------------------------------------------------------------------------
;            BINARY OP
tOr             EQU       3Ch         ; 3Ch  '_or_'
tXor            EQU       3Dh         ; 3Dh
;
tColon          EQU       3Eh         ; 3Eh ':'
tEnter          EQU       3Fh         ; 3Fh  Lenter
;
tAnd            EQU       40h         ; 40h  '_and_'       
;-------------------------------------------------------------------------------
;            LETTER TOKENS   
;
LET             EQU       41h
;
tA              EQU       LET         ; 41h
tB              EQU       LET+1       ; 42h
tC              EQU       LET+2       ; 43h
tD              EQU       LET+3       ; 44h
tE              EQU       LET+4       ; 45h
tF              EQU       LET+5       ; 46h
tG              EQU       LET+6       ; 47h
tH              EQU       LET+7       ; 48h
tI              EQU       LET+8       ; 49h
tJ              EQU       LET+9       ; 4Ah
tK              EQU       LET+10      ; 4Bh
tL              EQU       LET+11      ; 4Ch
tM              EQU       LET+12      ; 4Dh
tN              EQU       LET+13      ; 4Eh
tO              EQU       LET+14      ; 4Fh
tP              EQU       LET+15      ; 50h
tQ              EQU       LET+16      ; 51h
tR              EQU       LET+17      ; 52h
tS              EQU       LET+18      ; 53h
tT              EQU       LET+19      ; 54h
tU              EQU       LET+20      ; 55h
tV              EQU       LET+21      ; 56h
tW              EQU       LET+22      ; 57h
tX              EQU       LET+23      ; 58h
tY              EQU       LET+24      ; 59h
tZ              EQU       LET+25      ; 5Ah
tTheta          EQU       LET+26      ; 5Bh
;
;-------------------------------------------------------------------------------
;   THESE VAR TOKENS ARE 1ST OF A DOUBLE TOKEN 
;
vToks   EQU     LET+27    ;
;
;       USER MATRIX TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarMat         EQU       vToks       ; 5Ch
;
;       USER LIST TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarLst         EQU       vToks+1     ; 5Dh
;
;       USER EQUATION TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarEqu         EQU       vToks+2     ; 5Eh
tProg           EQU       vToks+3     ; 5Fh
;
;       USER PICT TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarPict        EQU       vToks+4     ; 60h
;
;       USER GDB TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarGDB         EQU       vToks+5     ; 61h
tVarOut         EQU       vToks+6     ; 62h
tVarSys         EQU       vToks+7     ; 63h
;
;
;-------------------------------------------------------------------------------
;            Mode Setting Commands
;
MODESA          EQU       vToks+8     ; 64h
;
tRad            EQU       MODESA      ; 64h 'Radian'
tDeg            EQU       MODESA+1    ; 65h 'Degree'
tNormF          EQU       MODESA+2    ; 66h 'Normal'
tSci            EQU       MODESA+3    ; 67h 'Sci'
tEng            EQU       MODESA+4    ; 68h 'Eng'
tFloat          EQU       MODESA+5    ; 69h 'Float'
;
CMPS            EQU       6Ah
;
tEQ             EQU       CMPS        ; 6Ah '=='
tLT             EQU       CMPS+1      ; 6Bh '<'
tGT             EQU       CMPS+2      ; 6Ch '>'
tLE             EQU       CMPS+3      ; 6Dh LLE
tGE             EQU       CMPS+4      ; 6Eh LGE
tNE             EQU       CMPS+5      ; 6Fh LNE
;
;-------------------------------------------------------------------------------
;            BINARY OP
;
tAdd            EQU       70h         ; 70h '+'
tSub            EQU       71h         ; 71h '-'
tAns            EQU       72h         ; 72h
;
;-------------------------------------------------------------------------------
;            Mode Setting Commands
MODES           EQU       73h
;
tFix            EQU       MODES       ; 73h 'Fix_'
tSplitOn        EQU       MODES+1     ; 74h
tFullScreen     EQU       MODES+2     ; 75h
tStndrd         EQU       MODES+3     ; 76h 'Func'
tParam          EQU       MODES+4     ; 77h 'Param'
tPolar          EQU       MODES+5     ; 78h 'Pol'
tSeqG           EQU       MODES+6     ; 79h
tAFillOn        EQU       MODES+7     ; 7Ah 'AUTO FILL ON
tAFillOff       EQU       MODES+8     ; 7Bh
tACalcOn        EQU       MODES+9     ; 7Ch
tACalcOff       EQU       MODES+10    ; 7Dh 'AutoFill OFF
;
;       GRAPH FORMAT TOKENS ARE 2 BYTE TOKENS
;
tGFormat        EQU       MODES+11    ; 7Eh
;
tBoxIcon        EQU       7Fh
tCrossIcon      EQU       80h
tDotIcon        EQU       81h
;
;-------------------------------------------------------------------------------
;            (More) BINARY OP
tMul            EQU       82h         ; 82h  '*'
tDiv            EQU       83h         ; 83h  '/'
;
;-------------------------------------------------------------------------------
;            SOME GRAPH COMMANDS
GRCMDS          EQU       84h
;
tTrace          EQU       GRCMDS      ; 84h 'Trace'
tClDrw          EQU       GRCMDS+1    ; 85h 'ClDrw'
tZoomStd        EQU       GRCMDS+2    ; 86h 'ZStd'
tZoomtrg        EQU       GRCMDS+3    ; 87h 'Ztrg'
tZoomBox        EQU       GRCMDS+4    ; 88h 'ZBOX'
tZoomIn         EQU       GRCMDS+5    ; 89h 'ZIn'
tZoomOut        EQU       GRCMDS+6    ; 8Ah 'ZOut'
tZoomSqr        EQU       GRCMDS+7    ; 8Bh 'ZSqr'
tZoomInt        EQU       GRCMDS+8    ; 8Ch 'ZInt'
tZoomPrev       EQU       GRCMDS+9    ; 8Dh 'ZPrev'
tZoomDec        EQU       GRCMDS+10   ; 8Eh 'ZDecm'
tZoomStat       EQU       GRCMDS+11   ; 8Fh 'ZStat
tUsrZm          EQU       GRCMDS+12   ; 90h 'ZRcl'
tPrtScrn        EQU       GRCMDS+13   ; 91h 'PrtScrn'
tZoomSto        EQU       GRCMDS+14   ; 92h  ZOOM STORE
tText           EQU       GRCMDS+15   ; 93h
;
;-------------------------------------------------------------------------------
;           BINARY OP  (Combination & Permutation)
tnPr            EQU       GRCMDS+16   ; 94h '_nPr_'
tnCr            EQU       GRCMDS+17   ; 95h '_nCr_'
;
;-------------------------------------------------------------------------------
;           MORE GRAPH COMMANDS
tYOn            EQU       GRCMDS+18   ; 96h 'FnOn_'
tYOff           EQU       GRCMDS+19   ; 97h 'FnOff_'
tStPic          EQU       GRCMDS+20   ; 98h 'StPic_'
tRcPic          EQU       GRCMDS+21   ; 99h 'RcPic_'
tStoDB          EQU       GRCMDS+22   ; 9Ah 'StGDB_'
tRclDB          EQU       GRCMDS+23   ; 9Bh 'RcGDB_'
tLine           EQU       GRCMDS+24   ; 9Ch 'Line'
tVert           EQU       GRCMDS+25   ; 9Dh 'Vert_'
tPtOn           EQU       GRCMDS+26   ; 9Eh 'PtOn'
tPtOff          EQU       GRCMDS+27   ; 9Fh 'PtOff'
;
;-------------------------------------------------------------------------------
;           TOKEN A0 CANNOT BE AN EOS FUNCTION
;           SINCE LOW MULT=A0 ALREADY
tPtChg          EQU       GRCMDS+28   ; A0h 'PtChg'
tPXOn           EQU       GRCMDS+29   ; A1h
tPXOff          EQU       GRCMDS+30   ; A2h
tPXChg          EQU       GRCMDS+31   ; A3h
tShade          EQU       GRCMDS+32   ; A4h 'Shade'
tCircl          EQU       GRCMDS+33   ; A5h 'Circl'
tHorz           EQU       GRCMDS+34   ; A6h 'HORIZONTAL'
tTanLn          EQU       GRCMDS+35   ; A7h 'TanLn'
tDrInv          EQU       GRCMDS+36   ; A8h 'DrInv_'
tDrawF          EQU       GRCMDS+37   ; A9h 'DrawF_'
;
tVarStrng       EQU       0AAh
;
;-------------------------------------------------------------------------------
;           Functions with No Arguments                                    
NOARG   EQU     0ABh
;
tRand           EQU       NOARG       ; ABh 'rand'
tPi             EQU       NOARG+1     ; ACh  Lpi
tGetKey         EQU       NOARG+2     ; ADh 'getKy'
;
;
tAPost          EQU       tGetKey+1   ; APOSTROPHY
tQuest          EQU       tAPost+1    ; QUESTION MARK
;
;
UNARY   EQU     tQuest+1              ; B0h
;
tChs            EQU       UNARY       ; B0h
tInt            EQU       UNARY+1     ; B1h
tAbs            EQU       UNARY+2     ; B2h
tDet            EQU       UNARY+3     ; B3h
tIdent          EQU       UNARY+4     ; B4h
tDim            EQU       UNARY+5     ; B5h
tSum            EQU       UNARY+6     ; B6h
tProd           EQU       UNARY+7     ; B7h
tNot            EQU       UNARY+8     ; B8h
tIPart          EQU       UNARY+9     ; B9h
tFPart          EQU       UNARY+10    ; BAh
;
;
;-------------------------------------------------------------------------------
;           NEW 2 BYTE TOKENS
t2ByteTok       EQU       0BBh         ;
;
;
UNARYLR EQU     UNARY+12
;
tSqrt           EQU       UNARYLR     ; BCh
tCubRt          EQU       UNARYLR+1   ; BDh
tLn             EQU       UNARYLR+2   ; BEh
tExp            EQU       UNARYLR+3   ; BFh
tLog            EQU       UNARYLR+4   ; C0h
tALog           EQU       UNARYLR+5   ; C1h
tSin            EQU       UNARYLR+6   ; C2h
tASin           EQU       UNARYLR+7   ; C3h
tCos            EQU       UNARYLR+8   ; C4h
tACos           EQU       UNARYLR+9   ; C5h
tTan            EQU       UNARYLR+10  ; C6h
tATan           EQU       UNARYLR+11  ; C7h
tSinH           EQU       UNARYLR+12  ; C8h
tASinH          EQU       UNARYLR+13  ; C9h
tCosH           EQU       UNARYLR+14  ; CAh
tACosH          EQU       UNARYLR+15  ; CBh
tTanH           EQU       UNARYLR+16  ; CCh
tATanH          EQU       UNARYLR+17  ; CDh
; 
;-------------------------------------------------------------------------------
;           SOME PROGRAMMING COMMANDS                                      
PROGTOK EQU     UNARYLR+18
;
tIf             EQU       PROGTOK     ; CEh
tThen           EQU       PROGTOK+1   ; CFh
tElse           EQU       PROGTOK+2   ; D0h
tWhile          EQU       PROGTOK+3   ; D1h
tRepeat         EQU       PROGTOK+4   ; D2h
tFor            EQU       PROGTOK+5   ; D3h
tEnd            EQU       PROGTOK+6   ; D4h
tReturn         EQU       PROGTOK+7   ; D5h
tLbl            EQU       PROGTOK+8   ; D6h   'Lbl_'
tGoto           EQU       PROGTOK+9   ; D7h   'Goto_'
tPause          EQU       PROGTOK+10  ; D8h   'Pause_'
tStop           EQU       PROGTOK+11  ; D9h   'Stop'
tISG            EQU       PROGTOK+12  ; DAh   'IS>'
tDSL            EQU       PROGTOK+13  ; DBh   'DS<'
tInput          EQU       PROGTOK+14  ; DCh   'Input_'
tPrompt         EQU       PROGTOK+15  ; DDh   'Prompt_'
tDisp           EQU       PROGTOK+16  ; DEh   'Disp_'
tDispG          EQU       PROGTOK+17  ; DFh   'DispG'
;
tOutput         EQU       PROGTOK+18  ; E0h    'Outpt'
tClLCD          EQU       PROGTOK+19  ; E1h    'ClLCD'
tConst          EQU       PROGTOK+20  ; E2h    'Fill'
tSortA          EQU       PROGTOK+21  ; E3h    'sortA_'
tSortD          EQU       PROGTOK+22  ; E4h    'sortD_'
tDispTab        EQU       PROGTOK+23  ; E5h    'Disp Table
tMenu           EQU       PROGTOK+24  ; E6h    'Menu'
tSendMBL        EQU       PROGTOK+25  ; E7h    'SEND'
tGetMBL         EQU       PROGTOK+26  ; E8h    'GET'
;-------------------------------------------------------------------------------
;           STAT PLOT COMMANDS                                             
statPCmd        EQU       PROGTOK+27
;
tPlotOn         EQU       statPCmd    ; E9h     ' PLOTSON'
tPlotOff        EQU       statPCmd+1   ; EAh     ' PLOTSOFF
;
tListName       EQU       0EBh        ; LIST DESIGNATOR
;
tPlot1          EQU       0ECh
tPlot2          EQU       0EDh
tPlot3          EQU       0EEh
;
tUnused01       EQU       0EFh        ;        available?
;
tPower          EQU       0F0h        ;        '^'
tXRoot          EQU       0F1h        ;        LsupX,Lroot

STATCMD         EQU       0F2h
;
tOneVar         EQU       STATCMD     ; F2h    'OneVar_'
tTwoVar         EQU       STATCMD+1   ; F3h
tLR             EQU       STATCMD+2   ; F4h    'LinR(A+BX
tLRExp          EQU       STATCMD+3   ; F5h    'ExpR_'
tLRLn           EQU       STATCMD+4   ; F6h    'LnR_'
tLRPwr          EQU       STATCMD+5   ; F7h    'PwrR_'
tMedMed         EQU       STATCMD+6   ; F8h
tQuad           EQU       STATCMD+7   ; F9h
tClrLst         EQU       STATCMD+8   ; FAh    'CLEAR LIST
tClrTbl         EQU       STATCMD+9   ; FBh    CLEAR TABLE
tHist           EQU       STATCMD+10  ; FCh    'Hist_'
txyLine         EQU       STATCMD+11  ; FDh    'xyline_'
tScatter        EQU       STATCMD+12  ; FEh    'Scatter_'
tLR1            EQU       STATCMD+13  ; FFh    'LINR(AX+B

;       
;-------------------------------------------------------------------------------
;           2ND HALF OF GRAPH FORMAT TOKENS
;-------------------------------------------------------------------------------
;           Format settings commands
;
GFMT            EQU       0
;
tSeq            EQU       GFMT        ; 'SeqG'
tSimulG         EQU       GFMT+1      ; 'SimulG'
tPolarG         EQU       GFMT+2      ; 'PolarGC'
tRectG          EQU       GFMT+3      ; 'RectGC'
tCoordOn        EQU       GFMT+4      ; 'CoordOn'
tCoordOff       EQU       GFMT+5      ; 'CoordOff'
tDrawLine       EQU       GFMT+6      ; 'DrawLine'
tDrawDot        EQU       GFMT+7      ; 'DrawDot'
tAxisOn         EQU       GFMT+8      ; 'AxesOn'
tAxisOff        EQU       GFMT+9      ; 'AxesOff'
tGridOn         EQU       GFMT+10     ; 'GridOn'
tGridOff        EQU       GFMT+11     ; 'GridOff'
tLblOn          EQU       GFMT+12     ; 'LabelOn'
tLblOff         EQU       GFMT+13     ; 'LabelOff'
tWebOn          EQU       GFMT+14     ; 'WebOn'
tWebOff         EQU       GFMT+15     ; 'WebOFF'
tuv             EQU       GFMT+16     ; U vs V
tvw             EQU       GFMT+17     ; V vs W
tuw             EQU       GFMT+18     ; U vs W
;
;-------------------------------------------------------------------------------
;           2ND HALF OF USER MATRIX TOKENS                                 
tMatA           EQU       00h         ; MAT A
tMatB           EQU       01h         ; MAT B
tMatC           EQU       02h         ; MAT C
tMatD           EQU       03h         ; MAT D
tMatE           EQU       04h         ; MAT E
tMatF           EQU       05h         ; MAT F
tMatG           EQU       06h         ; MAT G
tMatH           EQU       07h         ; MAT H
tMatI           EQU       08h         ; MAT I
tMatJ           EQU       09h         ; MAT J
;      
;-------------------------------------------------------------------------------
;           2ND HALF OF USER LIST TOKENS
tL1             EQU       00h         ; LIST 1
tL2             EQU       01h         ; LIST 2
tL3             EQU       02h         ; LIST 3
tL4             EQU       03h         ; LIST 4
tL5             EQU       04h         ; LIST 5
tL6             EQU       05h         ; LIST 6

;
;-------------------------------------------------------------------------------
;           2ND HALF OF USER EQUATION TOKENS
;
;       "Y" EQUATIONS HAVE BIT 4 SET
;
tY1             EQU       10h         ; Y1
tY2             EQU       11h         ; Y2
tY3             EQU       12h         ; Y3
tY4             EQU       13h         ; Y4
tY5             EQU       14h         ; Y5
tY6             EQU       15h         ; Y6
tY7             EQU       16h         ; Y7
tY8             EQU       17h         ; Y8
tY9             EQU       18h         ; Y9
tY0             EQU       19h         ; Y0
;
;       PARAM EQUATIONS HAVE BIT 5 SET
;
tX1T            EQU       20h         ; X1t
tY1T            EQU       21h         ; Y1t
tX2T            EQU       22h         ; X2t
tY2T            EQU       23h         ; Y2t
tX3T            EQU       24h         ; X3t
tY3T            EQU       25h         ; Y3t
tX4T            EQU       26h         ; X4t
tY4T            EQU       27h         ; Y4t
tX5T            EQU       28h         ; X5t
tY5T            EQU       29h         ; Y5t
tX6T            EQU       2Ah         ; X6t
tY6T            EQU       2Bh         ; Y6t
;
;       POLAR EQUATIONS HAVE BIT 6 SET
;
tR1             EQU       40h         ; R1
tR2             EQU       41h         ; R2
tR3             EQU       42h         ; R3
tR4             EQU       43h         ; R4
tR5             EQU       44h         ; R5
tR6             EQU       45h         ; R6
;
;       RECURSION EQUATIONS HAVE BIT 7 SET
;
tun             EQU       80h         ; Un
tvn             EQU       81h         ; Vn
twn             EQU       82h         ; Wn
;
;-------------------------------------------------------------------------------
;           2ND HALF OF USER PICTURE TOKENS                                
tPic1           EQU       00h         ; PIC1
tPic2           EQU       01h         ; PIC2
tPic3           EQU       02h         ; PIC3
tPic4           EQU       03h         ; PIC4
tPic5           EQU       04h         ; PIC5
tPic6           EQU       05h         ; PIC6
tPic7           EQU       06h         ; PIC7
tPic8           EQU       07h         ; PIC8
tPic9           EQU       08h         ; PIC9
tPic0           EQU       09h         ; PIC0
; 
;-------------------------------------------------------------------------------
;           2ND HALF OF USER GRAPH DATABASE TOKENS                         
tGDB1           EQU       00h         ; GDB1
tGDB2           EQU       01h         ; GDB2
tGDB3           EQU       02h         ; GDB3
tGDB4           EQU       03h         ; GDB4
tGDB5           EQU       04h         ; GDB5
tGDB6           EQU       05h         ; GDB6
tGDB7           EQU       06h         ; GDB7
tGDB8           EQU       07h         ; GDB8
tGDB9           EQU       08h         ; GDB9
tGDB0           EQU       09h         ; GDB0
;  
;-------------------------------------------------------------------------------
;           2ND HALF OF STRING VARS                                        
tStr1           EQU       00h
tStr2           EQU       01h
tStr3           EQU       02h
tStr4           EQU       03h
tStr5           EQU       04h
tStr6           EQU       05h
tStr7           EQU       06h
tStr8           EQU       07h
tStr9           EQU       08h
tStr0           EQU       09h

; 
;-------------------------------------------------------------------------------
;           2ND HALF OF SYSTEM OUTPUT ONLY VARIABLES                       
;
;   OPEN        EQU       00h
;
tRegEq          EQU       01h         ; REGRESSION EQUATION
;
tStatN          EQU       02h         ; STATISTICS N
;
tXMean          EQU       03h         ; X MEAN
tSumX           EQU       04h         ; SUM(X)
tSumXSqr        EQU       05h         ; SUM(X^2)
tStdX           EQU       06h         ; STANDARD DEV X
tStdPX          EQU       07h         ; STANDARD DEV POP X
tMinX           EQU       08h         ; Min X VALUE
tMaxX           EQU       09h         ; Max X VALUE
;
tMinY           EQU       0Ah         ; Min Y VALUE
tMaxY           EQU       0Bh         ; Max Y VALUE
tYmean          EQU       0Ch         ; Y MEAN
tSumY           EQU       0Dh         ; SUM(Y)
tSumYSqr        EQU       0Eh         ; SUM(Y^2)
tStdY           EQU       0Fh         ; STANDARD DEV Y
tStdPY          EQU       10h         ; STANDARD DEV POP Y
;
tSumXY          EQU       11h         ; SUM(XY)
tCorr           EQU       12h         ; CORRELATION
tMedX           EQU       13h         ; MED(X)
tQ1             EQU       14h         ; 1ST QUADRANT OF X
tQ3             EQU       15h         ; 3RD QUADRANT OF X
tQuadA          EQU       16h         ; 1ST TERM OF QUAD POLY REG/ Y-INT
tQuadB          EQU       17h         ; 2ND TERM OF QUAD POLY REG/ SLOPE
tQuadC          EQU       18h         ; 3RD TERM OF QUAD POLY REG
tCubeD          EQU       19h         ; 4TH TERM OF CUBIC POLY REG
tQuartE         EQU       1Ah         ; 5TH TERM OF QUART POLY REG
tMedX1          EQU       1Bh         ; x1 FOR MED-MED
tMedX2          EQU       1Ch         ; x2 FOR MED-MED
tMedX3          EQU       1Dh         ; x3 FOR MED-MED
tMedY1          EQU       1Eh         ; y1 FOR MED-MED
tMedY2          EQU       1Fh         ; y2 FOR MED-MED
tMedY3          EQU       20h         ; y3 FOR MED-MED
;
tRecurn     EQU       21h       ; RECURSION N
tStatP          EQU       22h
tStatZ          EQU       23h
tStatT          EQU       24h
tStatChi        EQU       25h
tStatF          EQU       26h
tStatDF         EQU       27h
tStatPhat       EQU       28h
tStatPhat1      EQU       29h
tStatPhat2      EQU       2Ah
tStatMeanX1     EQU       2Bh
tStatStdX1      EQU       2Ch
tStatN1         EQU       2Dh
tStatMeanX2     EQU       2Eh
tStatStdX2      EQU       2Fh
tStatN2         EQU       30h
tStatStdXP      EQU       31h
tStatLower      EQU       32h
tStatUpper      EQU       33h
tStat_s         EQU       34h
tLRSqr          EQU       35h         ; r^2
tBRSqr          EQU       36h         ; R^2
;
;-------------------------------------------------------------------------------
;       These next tokens are only used to access the data
;       they are display only and the user cannot access them at all
tF_DF    EQU     37h       ; ANOFAV FACTOR DF
tF_SS    EQU     38h       ; ANOFAV FACTOR SS
tF_MS    EQU     39h       ; ANOFAV FACTOR MS
tE_DF    EQU     3Ah       ; ANOFAV ERROR DF
tE_SS    EQU     3Bh       ; ANOFAV ERROR SS
tE_MS    EQU     3Ch       ; ANOFAV ERROR MS
;
;
;-------------------------------------------------------------------------------
;           2ND HALF OF SYSTEM INPUT/OUTPUT VARIABLES
;      SYSTEM VARIABLE EQUATES
;
tuXscl       EQU  0
tuYscl       EQU  1
tXscl        EQU  2
tYscl        EQU  3
tRecuru0     EQU  4        ; U 1ST INITIAL COND
tRecurv0     EQU  5        ; V 1ST INITIAL COND
tun1         EQU  6        ; U(N-1)    ; NOT USED
tvn1         EQU  7        ; V(N-1)    ; NOT USED
tuRecuru0    EQU  8        ;
tuRecurv0    EQU  9        ;
;
tXmin        EQU  0Ah
tXmax        EQU  0Bh
tYmin        EQU  0Ch
tYmax        EQU  0Dh
tTmin        EQU  0Eh
tTmax        EQU  0Fh
tThetaMin    EQU  10h
tThetaMax    EQU  11h
tuXmin       EQU  12h
tuXmax       EQU  13h
tuYmin       EQU  14h
tuYmax       EQU  15h
tuThetMin    EQU  16h
tuThetMax    EQU  17h
tuTmin       EQU  18h
tuTmax       EQU  19h
tTblMin      EQU  1Ah
tPlotStart   EQU  1Bh
tuPlotStart  EQU  1Ch
tnMax        EQU  1Dh
tunMax       EQU  1Eh
tnMin        EQU  1Fh
tunMin       EQU  20h
;
tTblStep    EQU  21h
tTStep      EQU  22h
tThetaStep  EQU  23h
tuTStep     EQU  24h
tuThetStep  EQU  25h
;
tDeltaX     EQU  26h
tDeltaY     EQU  27h
;
tXFact      EQU  28h
tYFact      EQU  29h
;
tTblInput       EQU       2Ah
;
tFinN           EQU       2Bh
tFinI           EQU       2Ch
tFinPV          EQU       2Dh
tFinPMT         EQU       2Eh
tFinFV          EQU       2Fh
tFinPY          EQU       30h
tFinCY          EQU       31h
;
tRecurw0        EQU       32h         ; w0(1)
tuRecurw0       EQU       33h
;
tPlotStep       EQU       34h
tuPlotStep      EQU       35h
;
tXres           EQU       36h
tuXres          EQU       37h
;
tRecuru02       EQU       38h         ; u0(2)
tuRecuru02      EQU       39h
tRecurv02       EQU       3Ch         ; v0(2)
tuRecurv02      EQU       3Dh
tRecurw02       EQU       3Eh         ; w0(2)
tuRecurw02      EQU       3Fh
;
;-------------------------------------------------------------------------------
;           2nd byte of t2ByteTok tokens
;
tFinNPV         EQU       00h         ;
tFinIRR         EQU       01h         ;
tFinBAL         EQU       02h         ;
tFinPRN         EQU       03h         ;
tFinINT         EQU       04h         ;
tFinToNom       EQU       05h         ;
tFinToEff       EQU       06h         ;
tFinDBD         EQU       07h         ;
tLCM            EQU       08h         ;
tGCD            EQU       09h         ;
tRandInt        EQU       0Ah         ;
tRandBin        EQU       0Bh         ;
tSubStrng       EQU       0Ch         ;
tStdDev         EQU       0Dh         ;
tVariance       EQU       0Eh         ;
tInStrng        EQU       0Fh         ;
tDNormal        EQU       10h         ;
tInvNorm        EQU       11h         ;
tDT             EQU       12h         ;
tChI            EQU       13h         ;
tDF             EQU       14h         ;
tBINPDF         EQU       15h         ;
tBINCDF         EQU       16h         ;
tPOIPDF         EQU       17h         ;
tPOICDF         EQU       18h         ;
tGEOPDF         EQU       19h         ;
tGEOCDF         EQU       1Ah         ;
tNormalPDF      EQU       1Bh         ;
tTPDF           EQU       1Ch         ;
tChiPDF         EQU       1Dh         ;
tFPDF           EQU       1Eh         ;
tRandNorm       EQU       1Fh         ;
tFinFPMT        EQU       20h         ;
tFinFI          EQU       21h         ;
tFinFPV         EQU       22h         ;
tFinFN          EQU       23h         ;
tFinFFV         EQU       24h         ;
tConj           EQU       25h         ;
tReal           EQU       26h         ;
tImag           EQU       27h         ;
tAngle          EQU       28h         ;
tCumSum         EQU       29h         ;
tExpr           EQU       2Ah         ;
tLength         EQU       2Bh         ;
tDeltaLst       EQU       2Ch         ;
tRef            EQU       2Dh         ;
tRRef           EQU       2Eh         ;
tToRect         EQU       2Fh         ;
tToPolar        EQU       30h         ;
tConste         EQU       31h         ;
tSinReg         EQU       32h         ;
tLogistic       EQU       33h         ;
tLinRegTTest    EQU       34h         ;
tShadeNorm      EQU       35h         ;
tShadeT         EQU       36h         ;
tShadeChi       EQU       37h         ;
tShadeF         EQU       38h         ;
tMatToLst       EQU       39h         ;
tLstToMat       EQU       3Ah         ;
tZTest          EQU       3Bh         ;
tTTest          EQU       3Ch         ;
t2SampZTest     EQU       3Dh         ;
t1PropZTest     EQU       3Eh         ;
t2PropZTest     EQU       3Fh         ;
tChiTest        EQU       40h         ;
tZIntVal        EQU       41h         ;
t2SampZInt      EQU       42h         ;
t1PropZInt      EQU       43h         ;
t2PropZInt      EQU       44h         ;
tGraphStyle     EQU       45h         ;
t2SampTTest     EQU       46h         ;
t2SampFTest     EQU       47h         ;
tTIntVal        EQU       48h         ;
t2SampTInt      EQU       49h         ;
tSetupLst       EQU       4Ah         ;
tFinPMTend      EQU       4Bh         ;
tFinPMTbeg      EQU       4Ch         ;
tRealM          EQU       4Dh         ;
tPolarM         EQU       4Eh         ;
tRectM          EQU       4Fh         ;
tExprOn         EQU       50h         ;
tExprOff        EQU       51h         ;
tClrAllLst      EQU       52h         ;
tGetCalc        EQU       53h         ;
tDelVar         EQU       54h         ;
tEquToStrng     EQU       55h         ;
tStrngToEqu     EQU       56h         ;
tDelLast        EQU       57h         ;
tSelect         EQU       58h         ;
tANOVA          EQU       59h         ;
tModBox         EQU       5Ah         ;
tNormProb       EQU       5Bh         ;
;
;
tMGT            EQU       64h         ; VERTICAL SPLIT
tZFit           EQU       65h         ; ZOOM FIT
tDiag_on        EQU       66h         ; DIANOSTIC DISPLAY ON
tDiag_off       EQU       67h         ; DIANOSTIC DISPLAY OFF
tOkEnd2v0       EQU       67h         ;end of 2byte tokens for version 0.
tArchive        EQU       68h         ;archive
tUnarchive      EQU       69h         ;unarchive
tasm        EQU   6Ah
tasmComp    EQU   6Bh         ; assm compile
tasmPrgm    EQU   6Ch         ; signifies a program is assm
tasmCmp     EQU   6Dh         ; assm program is compiled
;
tLcapAAcute     EQU       6Eh
tLcapAGrave     EQU       6Fh
tLcapACaret     EQU       70h
tLcapADier      EQU       71h
tLaAcute        EQU       72h
tLaGrave        EQU       73h
tLaCaret        EQU       74h
tLaDier         EQU       75h
tLcapEAcute     EQU       76h
tLcapEGrave     EQU       77h
tLcapECaret     EQU       78h
tLcapEDier      EQU       79h
tLeAcute        EQU       7Ah
tLeGrave        EQU       7Bh
tLeCaret        EQU       7Ch
tLeDier         EQU       7Dh
;
tLcapIGrave     EQU       7Fh
tLcapICaret     EQU       80h
tLcapIDier      EQU       81h
tLiAcute        EQU       82h
tLiGrave        EQU       83h
tLiCaret        EQU       84h
tLiDier         EQU       85h
tLcapOAcute     EQU       86h
tLcapOGrave     EQU       87h
tLcapOCaret     EQU       88h
tLcapODier      EQU       89h
tLoAcute        EQU       8Ah
tLoGrave        EQU       8Bh
tLoCaret        EQU       8Ch
tLoDier         EQU       8Dh
tLcapUAcute     EQU       8Eh
tLcapUGrave     EQU       8Fh
tLcapUCaret     EQU       90h
tLcapUDier      EQU       91h
tLuAcute        EQU       92h
tLuGrave        EQU       93h
tLuCaret        EQU       94h
tLuDier         EQU       95h
tLcapCCed       EQU       96h
tLcCed          EQU       97h
tLcapNTilde     EQU       98h
tLnTilde        EQU       99h
tLaccent        EQU       9Ah
tLgrave         EQU       9Bh
tLdieresis      EQU       9Ch
tLquesDown      EQU       9Dh
tLexclamDown    EQU       9Eh
tLalpha         EQU       9Fh
tLbeta          EQU      0a0h
tLgamma         EQU      0a1h
tLcapDelta      EQU      0a2h
tLdelta         EQU      0a3h
tLepsilon       EQU      0a4h
tLlambda        EQU      0a5h
tLmu            EQU      0a6h
tLpi            EQU      0a7h
tLrho           EQU      0a8h
tLcapSigma      EQU      0a9h
;
;
tLphi           EQU      0aBh
tLcapOmega      EQU      0aCh
tLphat          EQU      0aDh
tLchi           EQU      0aEh
tLstatF         EQU      0aFh
;

tLa      EQU     0b0h
tLb      EQU     0b1h
tLc      EQU     0b2h
tLd      EQU     0b3h
tLsmalle EQU     0b4h
tLf      EQU     0b5h
tLsmallg EQU     0b6h
tLh      EQU     0b7h
tLi      EQU     0b8h
tLj      EQU     0b9h
tLk      EQU     0bAh
;

tLl      EQU     0bCh
tLm      EQU     0bDh
;tLn      EQU     0bEh
tLo      EQU     0bFh
tLp      EQU     0c0h
tLq      EQU     0c1h
tLsmallr EQU     0c2h
tLs      EQU     0c3h
tLsmallt EQU     0c4h
tLu      EQU     0c5h
tLv      EQU     0c6h
tLw      EQU     0c7h
tLx      EQU     0c8h
tLy      EQU     0c9h
tLz      EQU     0cAh
tLsigma         EQU      0cbh
tLtau           EQU      0cch
tLcapIAcute     EQU      0cdh
;
;
tGarbagec EQU    0Ceh
LastToken       EQU      0Ceh         ;tLAST TOKEN IN THIS VERSION...

; EXTENDED TOKENS
;-----------------------------------------------
; These are new on OSes after 1.15 or whatever
tExtTok         equ 0EFh

; MathPrint
tZQuadrant      equ 17h
tZFracHalf      equ 18h
tZFracThird     equ 19h
tZFracFourth        equ 1Ah
tZFracFifth     equ 1Bh
tZFracEighth        equ 1Ch
tZFracTenth     equ 1Dh
tFracSlash      equ 2Eh
tFracMixedNum       equ 2Fh
tSwapImProper       equ 30h
tSwapFracDec        equ 31h
tRemainder      equ 32h
tSummationSigma     equ 33h
tLogBase        equ 34h
tRandIntNoRep       equ 35h
tMathPrint      equ 37h
tClassic        equ 38h
tAutoAnswer     equ 3Bh
tDecAnswer      equ 3Ch
tFracAnswer     equ 3Dh

; TI-84+CSE
tBlue           equ 41h
tRed            equ 42h
tBlack          equ 43h
tMagenta        equ 44h
tGreen          equ 45h
tOrange         equ 46h
tBrown          equ 47h
tNavy           equ 48h
tLtBlue         equ 49h
tYellow         equ 4Ah
tWhite          equ 4Bh
tLtGray         equ 4Ch
tMedGray        equ 4Dh
tGray           equ 4Eh
tDarkGray       equ 4Fh

tGraphColor     equ 65h
tTextColor      equ 67h
tBackgroundOn       equ 5Bh
tBackgroundOff      equ 64h

tThin           equ 74h
;tThick - 0x7E 0x06 (was "Connected"?)
tAsm84CPrgm     equ 68h
tAsm84CCmp      equ 69h
tBorderColor        equ 6Ch


;====== Data Types =============================================================
RealObj         EQU       0
ListObj         EQU       1
MatObj          EQU       2
EquObj          EQU       3
StrngObj        EQU       4
ProgObj         EQU       5
ProtProgObj     EQU       6
PictObj         EQU       7
GDBObj          EQU       8
UnknownObj      EQU       9
UnknownEquObj   EQU       0Ah
NewEquObj       EQU       0Bh
CplxObj         EQU       0Ch
CListObj        EQU       0Dh
UndefObj        EQU       0Eh
WindowObj       EQU       0Fh
ZStoObj         EQU       10h
TblRngObj       EQU       11h
LCDObj          EQU       12h
BackupObj       EQU       13h
AppObj      EQU   14h       ;application, only used in menus/link
AppVarObj   EQU   15h       ;application variable
TempProgObj     EQU       16h           ;program, home deletes when finished
GroupObj        EQU       17h           ;group.


;====== System Error Codes =====================================================
E_EDITF     EQU 7       ;allow re-entering application
E_EDIT      EQU 1<<E_EDITF
E_Mask      EQU 7Fh

E_Overflow  EQU 1+E_EDIT
E_DivBy0    EQU 2+E_EDIT
E_SingularMat   EQU 3+E_EDIT
E_Domain    EQU 4+E_EDIT
E_Increment EQU 5+E_EDIT
E_Break     EQU 6+E_EDIT
E_NonReal       EQU     7+E_EDIT
E_Syntax        EQU     8+E_EDIT
E_DataType      EQU     9+E_EDIT
E_Argument      EQU     10+E_EDIT
E_DimMismatch   EQU     11+E_EDIT
E_Dimension     EQU     12+E_EDIT
E_Undefined     EQU     13+E_EDIT
E_Memory        EQU     14+E_EDIT
E_Invalid       EQU     15+E_EDIT
E_IllegalNest   EQU     16+E_EDIT
E_Bound         EQU     17+E_EDIT
E_GraphRange    EQU     18+E_EDIT
E_Zoom          EQU     19+E_EDIT
E_Label         EQU     20
E_Stat          EQU     21
E_Solver        EQU     22+E_EDIT
E_Singularity   EQU     23+E_EDIT
E_SignChange    EQU     24+E_EDIT
E_Iterations    EQU     25+E_EDIT
E_BadGuess      EQU     26+E_EDIT
E_StatPlot      EQU     27
E_TolTooSmall   EQU     28+E_EDIT
E_Reserved      EQU     29+E_EDIT
E_Mode          EQU     30+E_EDIT
E_LnkErr        EQU     31+E_EDIT
E_LnkMemErr     EQU     32+E_EDIT
E_LnkTransErr   EQU     33+E_EDIT
E_LnkDupErr     EQU     34+E_EDIT
E_LnkMemFull    EQU     35+E_EDIT
E_Unknown       EQU     36+E_EDIT

E_Scale         EQU     37+E_EDIT
E_IdNotFound    EQU     38
E_NoMode        EQU     39+E_EDIT
E_Validation    EQU     40
E_Length        EQU     41+E_EDIT
E_Application   EQU     42+E_EDIT
E_AppErr1       EQU     43+E_EDIT
E_AppErr2       EQU     44+E_EDIT
E_ExpiredApp    EQU 45
E_BadAddr   EQU 46
E_Archived      EQU     47+E_EDIT
E_Version       EQU     48
E_ArchFull      EQU     49
E_Variable      EQU     50+E_EDIT
E_Duplicate     EQU     51+E_EDIT
HigErrNum       EQU     51


; obsolete error numbers34              ;first LINK error
E_LinkIOChkSum  EQU 34
E_LinkIOTimeOut EQU 35
E_LinkIOBusy    EQU 36
E_LinkIOVer EQU 37


;====== Stat Vars Equates ======================================================
FPLEN   EQU 9       ;Length of a floating-point number.
StatN   EQU     statVars
XMean   EQU     StatN      + FPLEN
SumX    EQU     XMean      + FPLEN
SumXSqr EQU     SumX       + FPLEN
StdX    EQU     SumXSqr    + FPLEN
StdPX   EQU     StdX       + FPLEN
MinX    EQU     StdPX      + FPLEN
MaxX    EQU     MinX       + FPLEN
MinY    EQU     MaxX       + FPLEN
MaxY    EQU     MinY       + FPLEN
YMean   EQU     MaxY       + FPLEN
SumY    EQU     YMean      + FPLEN
SumYSqr EQU     SumY       + FPLEN
StdY    EQU     SumYSqr    + FPLEN
StdPY   EQU     StdY       + FPLEN
SumXY   EQU     StdPY      + FPLEN
Corr    EQU     SumXY      + FPLEN
MedX    EQU     Corr       + FPLEN
Q1      EQU     MedX       + FPLEN
Q3      EQU     Q1         + FPLEN
QuadA   EQU     Q3         + FPLEN
QuadB   EQU     QuadA      + FPLEN
QuadC   EQU     QuadB      + FPLEN
CubeD   EQU     QuadC      + FPLEN
QuartE  EQU     CubeD      + FPLEN
MedX1   EQU     QuartE     + FPLEN
MedX2   EQU     MedX1      + FPLEN
MedX3   EQU     MedX2      + FPLEN
MedY1   EQU     MedX3      + FPLEN
MedY2   EQU     MedY1      + FPLEN
MedY3   EQU     MedY2      + FPLEN
PStat   EQU     MedY3      + 2*FPLEN
ZStat   EQU     PStat      + FPLEN
TStat   EQU     ZStat      + FPLEN
ChiStat EQU     TStat      + FPLEN
FStat   EQU     ChiStat    + FPLEN
DF      EQU     FStat      + FPLEN
Phat    EQU     DF         + FPLEN
Phat1   EQU     Phat       + FPLEN
Phat2   EQU     Phat1      + FPLEN
MeanX1  EQU     Phat2      + FPLEN
StdX1   EQU     MeanX1     + FPLEN
StatN1  EQU     StdX1      + FPLEN
MeanX2  EQU     StatN1     + FPLEN
StdX2   EQU     MeanX2     + FPLEN
StatN2  EQU     StdX2      + FPLEN
StdXP2  EQU     StatN2     + FPLEN
SLower  EQU     StdXP2     + FPLEN
SUpper  EQU     SLower     + FPLEN
SStat   EQU     SUpper     + FPLEN

;F_DF   EQU     anovaf_vars
;F_SS   EQU     F_DF + FPLEN
;F_MS   EQU     F_SS + FPLEN
;E_DF   EQU     F_MS + FPLEN
;E_SS   EQU     E_DF + FPLEN
;E_MS   EQU     E_SS + FPLEN

.list
