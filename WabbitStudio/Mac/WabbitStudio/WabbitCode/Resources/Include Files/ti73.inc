;======================================================================
; Include File for the TI-73
; Last Updated 8/14/2002 
;
; Copyright (c) 2002 Texas Instruments: The Licensed Materials are
; copyrighted by TI. LICENSEE agrees that it will 
; not delete the copyright notice, trademarks or
; protective notices from any copy made by LICENSEE.
;
; Warranty: TI does not warrant that the Licensed Materials will
; be free from errors or will meet your specific requirements.
; The Licensed Materials are made available "AS IS" to LICENSEE.
;
; Limitations: TI MAKES NO WARRANTY OR CONDITION, EITHER EXPRESS
; OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
; REGARDING THE LICENSED MATERIALS.  IN NO EVENT SHALL
; TI OR ITS SUPPLIERS BE LIABLE FOR ANY INDIRECT, INCIDENTAL
; OR CONSEQUENTIAL DAMAGES, LOSS OF PROFITS, LOSS OF USE OR DATA, 
; OR INTERRUPTION OF BUSINESS, WHETHER THE ALLEGED DAMAGES ARE
; LABELED IN TORT, CONTRACT OR INDEMNITY.
;
;========================================================================


;SYMBOLIC_DEBUG_START
;=================================
;entry points
;=================================
_AbsO1O2Cp		equ 40f0h
_AbsO1PAbsO2		equ 405ah
_ACos		equ 40d2h
_ACosRad		equ 40c6h
_AdrLEle		equ 4ab6h
_AdrMRow		equ 4567h
_AdrMEle		equ 456ah
_AllEq		equ 4792h
_AllocFPS		equ 4369h
_AllocFPS1		equ 436ch
_AnsName		equ 4B67h
_ApdSetup		equ 4c06h
_AppGetCbl		equ 4b97h
_AppGetCalc		equ 4b9ah
_AppInit		equ 4048h
_ASin		equ 40d8h
_ASinRad		equ 40cfh
_ATanRad		equ 40c9h
_ATan2Rad		equ 40cch
_ATan		equ 40d5h
_ATan2		equ 40dbh

_BinOPExec		equ 459ah
_BufClear		equ 4840h

_CanAlphIns		equ 4ab0h
_ChkFindSym		equ 42d9h
_CircCmd		equ 46f6h
_CkInt		equ 423ah
_CkOdd		equ 423dh
_CkOP1C0		equ 422bh
_CkOP1FP0		equ 422eh
_CkOP1Pos		equ 4252h
_CkOP1Real		equ 40e7h
_CkOP2FP0		equ 4231h
_CkOP2Pos		equ 424fh
_CkOP2Real		equ 42cah
_CkPosInt		equ 4237h
_CkValidNum		equ 426ah
_CleanAll		equ 4927h
_ClearRect		equ 4cfch
_ClearRow		equ 4c8dh
_CLine		equ 46cch
_CLineS		equ 46cfh
_CloseEditBuf	equ 47E9h
_CloseEditBufNoR		equ 46a2h
_CloseEditEqu	equ 4867h
_CloseProg		equ 490ch
_ClrGraphRef		equ 490fh
_ClrLCD		equ 44aah
_ClrLCDFull		equ 44a7h
_ClrLp		equ 41E3h
_ClrOP1S		equ 4258h
_ClrOP2S		equ 4255h
_ClrScrn		equ 44b0h
_ClrScrnFull		equ 44adh
_ClrTxtShd		equ 44b3h
_CmpSyms		equ 4921h
_ConvDim		equ 4B58h
_ConvDim00		equ 4B5Bh
_ConvOP1		equ 49abh
_Cos		equ 40c0h
_CpHLDE		equ 4003h
_CPoint		equ 4d89h
_CPointS		equ 4717h
_CpOP1OP2		equ 40f3h
_CpOP4OP3		equ 40eah
_CpyO1toFPS1		equ 4411h
_CpyO1toFPS2		equ 4420h
_CpyO1toFPS3		equ 442ch
_CpyO1toFPS4		equ 443eh
_CpyO1toFPS5		equ 4438h
_CpyO1toFPS6		equ 4432h
_CpyO1toFPS7		equ 4435h
_CpyO1toFPST		equ 43ffh
_CpyO2toFPS1		equ 440eh
_CpyO2toFPS2		equ 4417h
_CpyO2toFPS3		equ 4429h
_CpyO2toFPS4		equ 443bh
_CpyO2toFPST		equ 43f9h
_CpyO3toFPS1		equ 4408h
_CpyO3toFPS2		equ 441ah
_CpyO3toFPST		equ 43f6h
_CpyO5toFPS1		equ 440bh
_CpyO5toFPS3		equ 4426h
_CpyO6toFPS2		equ 441dh
_CpyO6toFPST		equ 43fch
_CpyStack		equ 43deh
_Cpyto1FPS1		equ 43e7h
_Cpyto1FPS10		equ 43a8h
_Cpyto1FPS11		equ 438dh
_Cpyto1FPS2		equ 43f0h
_Cpyto1FPS3		equ 43bdh
_Cpyto1FPS4		equ 43c3h
_Cpyto1FPS5		equ 4393h
_Cpyto1FPS6		equ 4399h
_Cpyto1FPS7		equ 439fh
_Cpyto1FPS8		equ 43a2h
_Cpyto1FPS9		equ 43abh
_Cpyto1FPST		equ 43d8h
_CpyTo2FPS1		equ 43e4h
_CpyTo2FPS2		equ 43edh
_CpyTo2FPS3		equ 43b7h
_CpyTo2FPS4		equ 43aeh
_CpyTo2FPS5		equ 4390h
_CpyTo2FPS6		equ 4396h
_CpyTo2FPS7		equ 439ch
_CpyTo2FPS8		equ 43a5h
_CpyTo2FPST		equ 43D5h
_CpyTo3FPS1		equ 43e1h
_CpyTo3FPS2		equ 43c6h
_CpyTo3FPST		equ 43d2h
_CpyTo4FPST		equ 43cfh
_CpyTo5FPST		equ 43c9h
_CpyTo6FPST		equ 43cch
_CpyTo6FPS3		equ 43b1h
_CpyTo6FPS2		equ 43b4h
_CpyToFPS1		equ 4414h
_CpyToFPS2		equ 4423h
_CpyToFPS3		equ 442fh
_CpyToFPST		equ 4402h
_CpyToStack		equ 4405h
_Create0Equ		equ 4303h
_CreateAppVar		equ 4312h
_CreateEqu		equ 4309h
_CreatePict		equ 430ch
_CreateProg		equ 430fh
_CreateReal		equ 42f4h
_CreateRList	equ 42fah
_CreateStrng	equ 4300h
_Cube  		equ 407bh
_CursorOff		equ 4522h
_CursorOn		equ 4528h

_DarkLine		equ 46ffh
_DarkPnt		equ 4714h
_DataSize		equ 433ch
_DataSizeA		equ 4339h
_DeallocFPS		equ 4363h
_DeallocFPS1		equ 4366h
_DecO1Exp		equ 4261h
_DelListEl		equ 4906h
_DelMem		equ 4330h
_DelRes		equ 48fah
_DelVar		equ 432ah
_DisableApd		equ 4be8h
_DisableAPI		equ 4bdfh
_DispDone		equ 4519h
_DispEOL		equ 450ah
_DispHL		equ 446eh
_DisplayImage		equ 4d5ch
_DispOP1A		equ 4AFBh
_DivHLBy10		equ 4006h
_DivHLByA		equ 4009h
_DrawCirc2		equ 4aadh
_DrawCmd		equ 47ddh
_DrawRectBorder		equ 4d2ch
_DrawRectBorderClear		equ 4d3bh
_DToR		equ 4075h

_EditProg		equ 4909h
_EnableApd		equ 4bebh
_EnoughMem		equ 42e5h
_EOP1NotReal		equ 4273h
_Equ_or_NewEqu		equ 42afh
_EraseEOL		equ 44b9h
_EraseRectBorder		equ 4d35h
_ErrArgument		equ 49ffh
_ErrBadGuess		equ 4a2fh
_ErrBreak		equ 4a20h
_ErrD_OP1_0		equ 42beh
_ErrD_OP1_LE_0		equ 42bbh
_ErrD_OP1Not_R		equ 42b5h
_ErrD_OP1NotPos		equ 42b2h
_ErrD_OP1NotPosInt		equ 42b8h
_ErrDataType		equ 49f9h
_ErrDimMismatch		equ 4a02h
_ErrDimension		equ 4a05h
_ErrDivBy0		equ 49e4h
_ErrDomain		equ 49e7h
_ErrIncrement		equ 49eah
_ErrInvalid		equ 4a11h
_ErrIterations		equ 4a2ch
_ErrLinkXmit		equ 4a38h
_ErrMemory		equ 4a0bh
_ErrNon_Real		equ 49edh
_ErrNonReal		equ 495ah
_ErrNotEnoughMem		equ 4441h
_ErrOverflow		equ 49e1h
_ErrSignChange		equ 4a29h
_ErrStat		equ 4a23h
_ErrStatPlot		equ 4a35h
_ErrSyntax		equ 49f0h
_ErrTolTooSmall		equ 4a32h
_ErrUndefined		equ 4a08h
_EToX		equ 40b4h
_ExLp		equ 4228h
_ExpToHex		equ 4249h

_FillRect		equ 4d02h
_FillRectPattern		equ 4d38h
_Find_Parse_Formula		equ 49b1h
_FindAlphaDn		equ 491eh
_FindAlphaUp		equ 491bh
_FindSym		equ 42dch
_FiveExec		equ 45b5h
_FixTempCnt		equ 4912h
_FormEReal		equ 4885h
_FormReal		equ 488Bh
_FourExec		equ 45b2h
_FPAdd		equ 4072h
_FPDiv		equ 4099h
_FPMult		equ 4084h
_FPRecip		equ 4096h
_FPSquare		equ 4081h
_FPSub		equ 406fh
_Frac		equ 4093h

_Get_Tok_Strng		equ 44F8h
_GetBaseVer		equ 4b8eh
_GetCSC		equ 400fh
_GetKey		equ 486dh
_GetLToOP1		equ 4573h
_GetTokLen		equ 44f5h
_GrBufClr         equ 4AD4h
_GrBufCpy		equ 4789h
_GrphCirc		equ 46f9h

_HLTimes9		equ 40e4h
_HorizCmd		equ 47c5h
_HTimesL		equ 4270h

_IBounds		equ 4aa4h
_IBoundsFull		equ 4d4dh
_ILine		equ 4702h
_IncLstSize		equ 4900h
_InsertList		equ 4903h
_InsertMem		equ 42dfh
_Int		equ 40a5h
_Intgr		equ 405dh
_InvCmd		equ 47e3h
_InvertRect		equ 4cffh
_InvOP1S		equ 408dh
_InvOP1SC		equ 408ah
_InvOP2S		equ 4090h
_InvSub		equ 4063h
_IOffset		equ 4aa7h
_IPoint		equ 4705h
_IsA2ByteTok	equ 4291h
_IsEditEmpty		equ 483Ah

_JError		equ 4a4dh
_JErrorNo		equ 4c03h
_JForceCmdNoChar		equ 401eh

_KeyToString		equ 452eh

_LCD_BUSY		equ 4051h
_LCD_DRIVERON		equ 4873h
_LdHLind		equ 4000h
_LineCmd		equ 47cbh
_LnX		equ 40abh
_Load_SFont		equ 46b7h
_LoadPattern		equ 4c51h
_LogX		equ 40aeh

_Max		equ 4057h
_MemChk		equ 42cdh
_MemClear		equ 4a47h
_MemSet		equ 4a4ah
_Min		equ 4054h
_Minus1		equ 406ch
_Mov10B		equ 413eh
_Mov18B		equ 46fch
_Mov9B		equ 4141h
_Mov7B		equ 414ah
_Mov8B		equ 4147h
_Mov9ToOP1		equ 416bh
_Mov9OP1OP2		equ 416eh
_Mov9ToOP2		equ 4171h
_MovFrOP1  equ 4174h

_NewLine		equ 4495h

_OP1ExOP2		equ 4225h
_OP1ExOP3		equ 421fh
_OP1ExOP4		equ 4222h
_OP1ExOP5		equ 4219h
_OP1ExOP6		equ 421ch
_OP1ExpToDec		equ 424ch
_OP1Set0		equ 41d1h
_OP1Set1		equ 4192h
_OP1Set2		equ 419eh
_OP1Set3		equ 4198h
_OP1Set4		equ 4195h
_OP1ToOP2		equ 4111h
_OP1ToOP3		equ 4105h
_OP1ToOP4		equ 40f9h
_OP1ToOP5		equ 4135h
_OP1ToOP6		equ 4132h
_OP2Set0		equ 41ceh
_OP2Set1		equ 41aah
_OP2Set2		equ 41a1h
_OP2Set3		equ 418fh
_OP2Set4		equ 418ch
_OP2Set5		equ 4186h
_OP2Set60		equ 497bh
_OP2Set8		equ 4183h
_OP2SetA		equ 4189h
_OP2ToOP1		equ 4138h
_OP2ToOP3		equ 415fh
_OP2ToOP4		equ 40fch
_OP2ToOP5		equ 412ch
_OP2ToOP6		equ 412fh
_OP3Set0		equ 41cbh
_OP3Set1		equ 4180h
_OP3Set2		equ 419bh
_OP3ToOP1		equ 4120h
_OP3ToOP2		equ 4102h
_OP3ToOP4		equ 40f6h
_OP3ToOP5		equ 4129h
_OP4Set0		equ 41c8h
_OP4Set1		equ 417dh
_OP4ToOP1		equ 411ah
_OP4ToOP2		equ 40ffh
_OP4ToOP3		equ 4162h
_OP4ToOP5		equ 4126h
_OP4ToOP6		equ 4168h
_OP5Set0		equ 41c5h
_OP5ToOP1		equ 411dh
_OP5ToOP2		equ 4108h
_OP5ToOP3		equ 4165h
_OP5ToOP4		equ 410eh
_OP5ToOP6		equ 410bh
_OP6ToOP1		equ 4117h
_OP6ToOP2		equ 4114h
_OP6ToOP5		equ 4123h

_ParseInp		equ 4966h
_PDspGrph		equ 47c2h
_PixelTest		equ 47d4h
_Plus1		equ 4069h
_PointCmd		equ 47d1h
_PointOn		equ 4a56h
_PopOP1		equ 4345h
_PopOP3		equ 4342h
_PopOP5		equ 433fh
_PopReal		equ 435ah
_PopRealO1		equ 4357h
_PopRealO2		equ 4354h
_PopRealO3		equ 4351h
_PopRealO4		equ 434eh
_PopRealO5		equ 434bh
_PopRealO6		equ 4348h
_PosNo0Int		equ 4234h
_PToR		equ 40deh
_PushOP1		equ 438ah
_PushOP3		equ 4387h
_PushOP5		equ 4384h
_PushReal		equ 4381h
_PushRealO1		equ 437eh
_PushRealO2		equ 437bh
_PushRealO3		equ 4378h
_PushRealO4		equ 4375h
_PushRealO5		equ 4372h
_PushRealO6		equ 436fh
_PutAway		equ 4039h
_PutC		equ 446bh
_PutMap		equ 4468h
_PutPS		equ 4477h
_PutPSB		equ 4474h
_PutS		equ 4471h
_PutTokString		equ 485eh
_PutToL		equ 4582h

_RandInit		equ 4c21h
_Random		equ 4b9dh
_Rcl_StatVar		equ 42c7h
_RclAns		equ 4996h
_RclGDB2		equ 46f3h
_RclSysTok		equ 49a2h
_RclVarSym		equ 499fh
_RclX		equ 499ch
_RclY		equ 4999h
_RedimMat		equ 48fdh
_Regraph		equ 47aah
_ReleaseBuffer		equ 46A5h
_ReloadAppEntryVecs		equ 4a50h
_RestoreDisp		equ 478ch
_RndGuard		equ 409fh
_RnFx		equ 40a2h
_Round		equ 40a8h
_RToD		equ 4078h
_RToP		equ 40e1h
_RunIndicOff		equ 44d7h
_RunIndicOn		equ 44d4h

_SaveDisp		equ 4ba3h
_SendVarCmd		equ 48F1h
_SetFuncM		equ 4762h
_SetXXOP1		equ 46c0h
_SetXXOP2		equ 46c3h
_SetXXXXOP2		equ 46c6h
_SFont_Len		equ 46bah
_ShRAcc		equ 41E6h
_Sin		equ 40bdh
_SinCosRad		equ 40bah
_SqRoot		equ 409ch
_SrchVLstDn		equ 4c36h
_SrchVLstUp		equ 4c33h
_SStringLength		equ 4c54h
_StMatEl		equ 49a5h
_StoAns		equ 498ah
_StoGDB2		equ 46f0h
_StoOther		equ 4993h
_StoRand		equ 4e5eh
_StoSysTok		equ 4987h
_StoX		equ 4990h
_StoY		equ 498dh
_StrCopy		equ 444ah
_StrLength		equ 4a5ch

_Tan		equ 40c3h
_TenX		equ 40b7h
_ThreeExec		equ 45ach
_Times2		equ 4066h
_TimesPt5		equ 407eh
_ToFrac		equ 4594h
_Trunc		equ 4060h

_UCLines		equ 46c9h
_UnLineCmd		equ 47ceh
_UnOpExec		equ 45a9h

_VertCmd		equ 47c8h
_VPutMap		equ 44c5h
_VPutS		equ 44c8h
_VPutSN		equ 44cbh
_VtoWHLDE		equ 471dh

_XftoI		equ 4726h
_XitoF		equ 4720h
_XName		equ 4279h
_XRootY		equ 46d2h

_YftoI		equ 4723h
_YName		equ 427ch
_YtoX		equ 46d5h

_Zero16D		equ 41b9h
_ZeroOP		equ 41e0h
_ZeroOP1		equ 41d7h
_ZeroOP2		equ 41dah
_ZeroOP3		equ 41ddh
_ZmDecml		equ 476eh
_ZmFit		equ 477ah
_ZmInt		equ 476bh
_ZmPrev		equ 4771h
_ZmSquare		equ 477dh
_ZmStats		equ 46d8h
_ZmTrig		equ 4780h
_ZmUsr		equ 4774h
_ZooDefault		equ 4786h

;======================================================================

;           system-defined ram variable address equates

;======================================================================

;


ramStart             equ  8000h
appData              equ  8000h
ramCode              equ  8100h
ramCodeEnd           equ  822fh
baseAppBrTab         equ  8230h
bootTemp             equ  8241h
appSearchPage        equ  8293h
tempSwapArea         equ  8295h
appID                equ  837dh
ramReturnData        equ  8529h
appInfo              equ  8998h
appBank_jump         equ  85c9h
appPage              equ  85cbh
kbdScanCode          equ  83d8h
kbdKey               equ  83ddh
kbdGetKy             equ  83deh
keyExtend            equ  83dfh
contrast             equ  83e0h
apdSubTimer          equ  84e0h
apdTimer             equ  84e1h
curTime              equ  83e1h
curRow               equ  83e2h
curCol               equ  83e3h
curOffset            equ  83e4h
curUnder             equ  83e5h
curY                 equ  8498h
curType              equ  8499h
curXRow              equ  849ah
prevDData            equ  849bh
lFont_record         equ  84a3h
sFont_record         equ  84abh
tokVarPtr            equ  85a1h
tokLen               equ  85a3h
indicMem             equ  84d6h
indicCounter         equ  84deh
indicBusy            equ  84dfh
OP1                  equ  842bh
OP1M                 equ  842dh
OP2                  equ  8436h
OP2M                 equ  8485h
OP2EXT               equ  843fh
OP3                  equ  8441h
OP3M                 equ  8443h
OP4                  equ  844ch
OP4M                 equ  844eh
OP5                  equ  8457h
OP5M                 equ  8459h
OP6                  equ  8462h
OP6M                 equ  8464h
OP6EXT               equ  846bh
progToEdit           equ  8472h
nameBuff             equ  847ah
equ_edit_save        equ  8485h
iMathPtr1            equ  8486h
iMathPtr2            equ  8488h
iMathPtr3            equ  848ah
iMathPtr4            equ  848ch
iMathPtr5            equ  848eh
chkDelPtr1           equ  8490h
chkDelPtr2           equ  8492h
chkDelPtr3           equ  9baah
chkDelPtr4           equ  9bach
insDelPtr            equ  8494h
upDownPtr            equ  8496h
fOutDat              equ  85a5h
asm_data_ptr1        equ  85afh
asm_data_ptr2        equ  85b1h
asm_sym_ptr1         equ  85b3h
asm_sym_ptr2         equ  85b5h
asm_ind_call         equ  85b7h
textShadow           equ  8913h
textShadCur          equ  8993h
textShadTop          equ  8995h
textShadAlph         equ  8996h
textShadIns          equ  8997h
cxMain               equ  84e2h
cxPPutAway           equ  84e4h
cxPutAway            equ  84e6h
cxRedisp             equ  84e8h
cxErrorEP            equ  84eah
cxSizeWind           equ  84ech
cxPage               equ  84eeh
cxCurApp             equ  84efh
cxPrev               equ  84f0h
monQH                equ  850eh
monQT                equ  850fh
monQueue             equ  8510h
onSP                 equ  8520h
promptRow            equ  89a2h
promptCol            equ  89a3h
promptIns            equ  89a4h
promptShift          equ  89a5h
promptRet            equ  89a6h
promptValid          equ  89a8h
promptTop            equ  89aah
promptCursor         equ  89ach
promptTail           equ  89aeh
promptBtm            equ  89b0h
varType              equ  89b2h
varCurrent           equ  89b3h
varClass             equ  89bbh
menuActive           equ  84b3h
menuAppDepth         equ  84b4h
MenuCurrent          equ  84b5h
ProgCurrent          equ  84c1h
userMenuSA           equ  89e1h
ioPrompt             equ  8a41h
dImageWidth          equ  8a42h
RectFillPHeight		 equ  8A42h
RectFillPWidth		 equ  8a43h
RectFillPattern		 equ  8a44h
ioFlag               equ  83e6h
sndRecState          equ  83e8h
ioErrState           equ  83e9h
header               equ  83eah
ioData               equ  83f3h
bakHeader            equ  83ffh
penCol               equ  89dfh
penRow               equ  89e0h
rclQueue             equ  84d2h
rclQueueEnd          equ  84d4h
errNo                equ  854ah
errSP                equ  854bh
errOffset            equ  854dh
saveSScreen          equ  8a52h
bstCounter           equ  8559h
flags                equ  855bh
statVars             equ  8d58h
curGStyle            equ  8e94h
curGY                equ  8e95h
curGX                equ  8e96h
curGY2               equ  8e97h
curGX2               equ  8e98h
freeSaveY            equ  8e99h
freeSaveX            equ  8e9ah
XOffset              equ  8ec8h
YOffset              equ  8ec9h
lcdTallP             equ  8ecah
pixWideP             equ  8ecbh
pixWide_m_1          equ  8ecch
pixWide_m_2          equ  8ecdh
lastEntryPTR         equ  8eceh
lastEntryStk         equ  8ed0h
numLastEntries       equ  90d0h
currLastEntry        equ  90d1h
curInc               equ  92deh
uXmin                equ  92f5h
uXmax                equ  92feh
uXscl                equ  9307h
uYmin                equ  9310h
uYmax                equ  9319h
uYscl                equ  9322h
uThetMin             equ  932bh
uThetMax             equ  9334h
uThetStep            equ  933dh
uTmin                equ  9346h
uTmax                equ  934fh
uTStep               equ  9358h
uPlotStart           equ  9361h
unMax                equ  936ah
uu0                  equ  9373h
uv0                  equ  937ch
unMin                equ  9385h
uu02                 equ  938eh
uv02                 equ  9397h
uw0                  equ  93a0h
uPlotStep            equ  93a9h
uXres                equ  93b2h
uw02                 equ  93bbh
Xmin                 equ  93c7h
Xmax                 equ  93d0h
Xscl                 equ  93d9h
Ymin                 equ  93e2h
Ymax                 equ  93ebh
Yscl                 equ  93f4h
ThetaMin             equ  93fdh
ThetaMax             equ  9406h
ThetaStep            equ  940fh
TminPar              equ  9418h
TmaxPar              equ  9421h
Tstep                equ  942ah
PlotStart            equ  9433h
nMax                 equ  943ch
u0                   equ  9445h
v0                   equ  944eh
nMin                 equ  9457h
u02                  equ  9460h
v02                  equ  9469h
w0                   equ  9472h
PlotStep             equ  947bh
XresO                equ  9484h
w02                  equ  948dh
smallEditRAM         equ  949fh
XFact                equ  950bh
YFact                equ  9514h
Xres_int             equ  951dh
deltaX               equ  951eh
deltaY               equ  9527h
shortX               equ  9530h
shortY               equ  9539h
lower                equ  9542h
upper                equ  954bh
XOutSym              equ  9558h
XOutDat              equ  955ah
YOutSym              equ  955ch
YOutDat              equ  955eh
inputSym             equ  9560h
inputDat             equ  9562h
prevData             equ  9564h
TblMin               equ  9676h
TblStep              equ  967fh
P1Type		     EQU  968ch
SavX1List	     EQU  968dh
SavY1List	     EQU  9692h
SavF1List	     EQU  9697h
P1FrqOnOff	     EQU  96abh
P2Type		     EQU  96aeh
SavX2List	     EQU  96afh
SavY2List	     EQU  96b4h
SavF2List	     EQU  96b9h
P2FrqOnOff	     EQU  96cdh
P3Type		     EQU  96d0h	
SavX3List	     EQU  96d1h
SavY3List	     EQU  96d6h
SavF3List	     EQU  96d8h
P3FrqOnOff	     EQU  96efh
plotSScreen          equ  9737h
seed1                equ  9a37h
seed2                equ  9a40h
cmdCursor            equ  9a7eh
editTop              equ  9a82h
editCursor           equ  9a84h
editTail             equ  9a86h
editBtm              equ  9a88h
editSym              equ  9a94h
editDat              equ  9a96h
winTop               equ  9b33h
winBtm               equ  9b34h
winLeftEdge          equ  9b35h
winLeft              equ  9b36h
winAbove             equ  9b38h
winRow               equ  9b3ah
winCol               equ  9b3ch
fmtDigits            equ  9b3eh
fmtString            equ  9b3fh
fmtConv              equ  9b80h
fmtLeft              equ  9b92h
fmtIndex             equ  9b94h
fmtMatSym            equ  9b96h
fmtMatMem            equ  9b98h
EQS                  equ  9b9ah
tSymPtr1             equ  9ba6h
tSymPtr2             equ  9ba8h
chkDelPtr3           EQU  9baah
chkDelPtr4           EQU  9bach
tempMem              equ  9baeh
fpBase               equ  9bb0h
FPS                  equ  9bb2h
OPBase               equ  9bb4h
OPS                  equ  9bb6h
pTempCnt             equ  9bb8h
cleanTmp             equ  9bbah
pTemp                equ  9bbch
progPtr              equ  9bbeh
newDataPtr           equ  9bc0h
appErr1              equ  9bc2h
appErr2              equ  9bcfh
appRawKeyHandle      equ  85b8h
appBackUpScreen      equ  8613h
customHeight         equ  8612h
userMem              equ  9c37h
symTable             equ  0fe66h

;SYMBOLIC_DEBUG_END
;====================================
; equates for fraction entry routines
;====================================
fieldMax    equ    OP5
fieldLen    equ    fieldMax+1
fieldCol    equ    fieldLen+1     ; pencol of start
fieldRow    equ    fieldCol+1     ; penrow of start  3 rows above char
fieldInput  equ    fieldRow+1     ; 9 bytes,
wholeMax    equ    tempSwapArea
wholeLen    equ    wholeMax+1
wholeCol    equ    wholeLen+1     ; pencol of start
wholeRow    equ    wholeCol+1     ; penrow of start  3 rows above char
wholeInput  equ    wholeRow+1     ; 9 bytes,
;
numerMax    equ    wholeInput+9
numerLen    equ    numerMax+1
numerCol    equ    numerLen+1     ; pencol of start
numerRow    equ    numerCol+1     ; penrow of start
numerInput  equ    numerRow+1     ; 9 bytes,
;
denomMax    equ    numerInput+9
denomLen    equ    denomMax+1
denomCol    equ    denomLen+1     ; pencol of start
denomRow    equ    denomCol+1     ; penrow of start
denomInput  equ    denomRow+1     ; 9 bytes,
;
fracLineLen equ   denomInput+9
fracLineCol equ   fracLineLen+1
;
fracRightEdge  equ fracLineCol+1
;
curPosition equ   fracRightEdge+1
;
numerSpaces equ   curPosition+1
denomSpaces equ   numerSpaces+1
haveFrac    equ   3
haveUnit    equ   4
incCurPos   equ   5




;====================================================================
; macro to call base code from an app
;====================================================================
rBR_CALL	equ	28h		; needed for apps
BRT_JUMP0	equ	50h
;
B_CALL macro   label
	     rst     rBR_CALL
	     dw      _&label
       .endm
;
B_JUMP macro   label
	     call    BRT_JUMP0	     ; fixed location on page 0
	     dw      _&label
       .endm
;
;======================================================================
;           system variable equates
;======================================================================
;       entry points : rclsystok, stosystok
;
;       vars with no restriction
;
XSCLt       equ  3           ; 'xscl'           4
YSCLt       equ  4           ; 'yscl'           4
;
;       vars rounded to 10 digits
;
XMINt       EQU  0AH         ; 'xMin'           4
XMAXt       EQU  0BH         ; 'xMax'           4
YMINt       EQU  0CH         ; 'yMin'           4
YMAXt       EQU  0DH         ; 'yMax'           4
TBLMINt     EQU  1AH         ; 'TBLMIN'
;
;       rouned to 10 digits, and positve integers
;
PLOTSTARTt   equ  1bh         ; 'nstart'         5
UPLOTSTARTt  equ  1ch         ; 'znstart'        6

;       vars cannot be 0
;
TBLSTEPt    EQU  21H         ; 'TBLSTEP'
TSTEPt      EQU  22H         ; 'tStep'          5
THETSTEPt   EQU  23H         ; Ltheta,'Step'    5
UTSTEPt     EQU  24H         ; 'ztStep'         6
UTHSTEPt    EQU  25H         ; 'z',Ltheta,'Step' 6
;
;       special limits on these
;
DELTAXt     EQU  26H         ; LcapDelta,'x'    2
DELTAYt     EQU  27H         ; LcapDelta,'y'    2
;
;       special limits on these
;
XFACTt      EQU  28H         ; 'xFact'          5
YFACTt      EQU  29H         ; 'yFact'          5
;
;       sys var list
;
TBLINPUTt       equ       2ah
;
;
;
PLOTSTEPt       equ       34h
UPLOTSTEPt      equ       35h
;
XRESt           equ       36h
UXRESt          equ       37h
;
; special token used by input/prompt for storing value to variable

INPUTt      equ  40h         ; set up for store for input/prompt

; special token used by graph math intersection routine to indicate
; list data

ISECTt      equ  41h         ; list data follows this
;
;========== automatic power down time out value (<= 7 minutes)
apdTimeOut	equ	5		;automatic power down time (minutes)

;========== interrupt equate
iAll 	equ 1011b

;========== i/o ports
intrptEnPort	equ 3 ; interrupt enable (in/out) 
memPageAPort	equ	6		; memory page a	(i/o)
LCDInstPort     equ     10h
LCDDataPort     equ     11h 


;========== common subroutine rst numbers
rOP1TOOP2	EQU  08h
rFINDSYM	EQU  10h
rPUSHREALO1	EQU  18h
rMOV9TOOP1	EQU  20h
rFPADD		EQU  30h

;============error handler equates/macros
APP_PUSH_ERRORH EQU	59h
APP_POP_ERRORH	EQU	5Ch

AppOnErr   macro   handaddr
	LD	HL,handaddr		;hl -> stack handler address
	CALL	APP_PUSH_ERRORH 	;establish error handler context
	.endm

AppOffErr  macro
	CALL	APP_POP_ERRORH		;remove error handler context
	.endm
;
;========== run indicators

busyPause	equ	10101010b
busynNormal	equ	11110000b

;======================================================================
;           system and state flags
;======================================================================
;

trigFlags	equ  0		;trigonometry mode settings
trigDeg 	equ    2	; 1=degrees, 0=radians

kbdFlags	equ  0		;keyboard scan
kbdScr		equ    3	; 1=scan code ready
kbdKeyPress	equ    4	; 1=key has been pressed

doneFlags	equ  0		;display "done"
donePrgm	equ    5	; 1=display "done" after prgm

ioDelFlag	equ  0
inDelete	equ    0	;1 = delete screen 

;----------------------------------------------------------------------
editFlags	equ  1
editOpen	equ    2	; 1=edit buffer is open

monFlags	equ  1		;monitor flags
monAbandon	equ    4	; 1=don't start any long process
				; in put away (#715)
;----------------------------------------------------------------------
plotFlags	equ  2		;plot generation flags
plotTrace	equ  0
plotLoc 	equ    1	; 0=bkup & display, 1=display only
plotDisp	equ    2	; 1=plot is in display, 0=text in display


grfModeFlags	equ  2		;graph mode settings
grfFuncM	equ    4	; 1=function graph
grfPolarM	equ    5	; 1=polar graph
grfParamM	equ    6	; 1=parametric graph
grfRecurM       equ    7        ; 1=recursion graph

graphFlags	equ  3
graphProg	equ  1     ; 1=graph in progress
graphDraw	equ    0	; 0=graph is valid, 1=redraw graph

grfDBFlags	equ  4
grfDot		equ    0	; 0=line, 1=dot
grfSimul	equ    1	; 0=sequential, 1=simultaneous
grfGrid 	equ    2	; 0=no grid, 1=grid
grfPolar	equ    3	; 0=rectangular, 1=polar coordinates
grfNoCoord	equ    4	; 0=display coordinates, 1=off
grfNoAxis	equ    5	; 0=axis, 1=no axis
grfLabel	equ    6	; 0=off, 1=axis label

textFlags	equ  5		;text output flags
textEraseBelow	equ    1	; 1=erase line below small char
textScrolled	equ    2	; 1=screen scrolled
textInverse	equ    3	; 1=display inverse bit-map
textInsMode	equ    4	; 0=overstrike, 1=insert mode

parsFlag2	equ  7		;parser flags
numOP1		equ    0	; 1=result in op1, 0=no result

newDispF        equ  8		;derivative mode flags
preClrForMode   equ    0	; 1=help blink on mode screen

apdFlags	equ  8		;automatic power-down
apdAble 	equ    2	; 1=apd enabled
apdRunning	equ    3	; 1=apd clock running


web_err_mask    equ  60h

onFlags 	equ  9		;on key flags
onRunning	equ	 3
onInterrupt	equ    4	; 1=on key interrupt request

statFlags	equ  9		;statistics flags
statsValid	equ    6	; 1=stats are valid

fmtFlags	equ  10		;numeric format flags
fmtExponent	equ    0	; 1=show exponent, 0=no exponent
fmtEng		equ    1	; 1=engineering notion, 0=scientific
fmtHex		equ    2	; 1=hexadecimal
fmtOct		equ    3	; 1=octal
fmtBin		equ    4	; 1=binary
;
numMode         equ  10
fmtReal         equ    5
fmtRect         equ    6
fmtPolar        equ    7

realMode        equ    5
rectMode        equ    6
polarMmode       equ    7
;					;   if hex and oct both = 1
;					;   then bin=0 means >frac
;					;	 bin=1 means >dms
fmtBaseMask     equ  00011100b		; mask to base flags
fmtBaseShift    equ  2			; offset to base flags
;
;       check if these are used by numform,
;
;               equ  6
;               equ  7

fmtOverride	equ  11		;copy of fmtflags with conversion override

fmtEditFlags	equ  12		;numeric editing flags
fmtEdit 	equ    0	; 1=format number for editing

curFlags	equ  12		;cursor
curAble 	equ    2	; 1=cursor flash is enabled
curOn		equ    3	; 1=cursor is showing
curLock 	equ    4	; 1=cursor is locked off

appFlags	equ  13		;application flags
appWantIntrpt	equ    0		; 1=want on key interrupts
appTextSave	equ    1		; 1=save characters in textshadow
appAutoScroll	equ    2		; 1=auto-scroll text on last line
appMenus	equ    3		; 1=process keys that bring up menus
					; 0=check lock menu flag
appLockMenus	equ    4		; 1=ignore menu keys
					; 0=switch to home screen and bring up menu
appCurGraphic	equ    5		; 1=graphic cursor
appCurWord	equ    6		; 1=text cursor covers entire word
appExit 	equ    7		; 1=application handles [exit] key itself

appWantIntrptF	equ	1<<appWantIntrpt
appTextSaveF	equ	1<<appTextSave
appAutoScrollF	equ	1<<appAutoScroll
appMenusF	equ	1<<appMenus
appLockMenusF	equ	1<<appLockMenus
appCurGraphicF	equ	1<<appCurGraphic
appCurWordF	equ	1<<appCurWord
appExitF	equ	1<<appExit


seqFlags        equ  15              ; sequential graph flags
webMode         equ    0             ; 0 = normal seq mode, 1 = web mode
webVert         equ    1             ;
sequv           equ    2             ; u vs v
seqvw           equ    3             ; v vs w
sequw           equ    4             ; u vs w


promptFlags	equ  17		;prompt line flags
promptEdit	equ    0	; 1=editing in prompt buffer

indicFlags	equ  18		;indicator flags
indicRun	equ    0	; 1=run indicator on
indicInUse	equ    1	; indicator save area in use=1, free=0
indicOnly	equ    2	; interrupt handler only checks run indicator

shiftFlags	equ  18		;[2nd] and [alpha] flags
shift2nd	equ    3	; 1=[2nd] has been pressed
shiftAlpha	equ    4	; 1=[alpha] has been pressed
shiftLwrAlph	equ    5	; 1=lower case, 0=upper case
shiftALock	equ    6	; 1=alpha lock has been pressed
shiftKeepAlph	equ    7	; 1=cannot cancel alpha shift


tblFlags        equ  19		;table flags.
autoFill        equ    4	; 1=prompt, 0=fillauto
autoCalc        equ    5	; 1=prompt, 0=calcauto
reTable         equ    6	; 0=table is ok, 1=must recompute table.


fracMode        equ      20             ;FRAC FLAGS
ABCF            equ       0             ; a b/c if 1, b/c if 0.
AutoSimpF       equ       1             ; autosimp if 0, mansimp if 1.
ManSimpF        equ       1             ; autosimp if 0, mansimp if 1.

ConstMode       equ     20
Hide            equ       2            ;0 if show, 1 if hide.
Multi           equ       3            ;0 if single, 1 if multiple.

sGrFlags	equ  20
write_on_graph  equ    4	; 1 = text or equ writing to graph screen
g_style_active  equ    5	; 1 = graph styles are enabled, use them
cmp_mod_box     equ    6	; 1 = doing mod box plot computation
textWrite       equ    7
;

newIndicFlags   equ  21
extraIndic      equ    0
saIndic         equ    1

smartFlags      equ  23
;
;----------------------------------------------------------------------
; note: fix these equates if smartflags are moved                            
;----------------------------------------------------------------------
;
smarter_mask    equ  3
smarter_test    equ  1
smartGraph      equ  0
smartGraph_inv  equ  1                                                 

;----------------------------------------------------------------------
;           available for asm programming
;----------------------------------------------------------------------

asm_Flag1       equ  33          ; asm coding
asm_Flag2       equ  34          ; asm coding
asm_Flag3       equ  35          ; asm coding

;----------------------------------------------------------------------

fracFlags	equ	  36		; fraction display/edit
fracStopEOS	equ	   0
fracInside	equ	   1
fracDontDraw	equ	   2
fracEditMode	equ	   3
fracInNum	equ	   4
fracScrScrolled equ	   5
fracFlag1	equ	   6
fracClearEOS	equ	   7	   ; tell drawExpr code to clear to end of screen when done
;
fontFlags	equ	   37
fracDrawNum	equ	   0
fracDrawDenom	equ	   1
fracDrawLFont	equ	   2
fracTallLFont	equ	   3
fracLastChar	equ	   4
fracCursor	equ	   5
DrawLfont       equ       6
CustomFont      equ       7
;
;flags for fraction math routines:
FracFlags2      equ     38
DtoF10F         equ      0      ;10digit flag for Dec ->Frac
SimpKeyF        equ      1      ;Used for Simp K.
RatD1K          equ      1      ;also used to show denominator=1000
FinalType       equ      2      ;1=final type is improper 0=type is simple
FinalFrac       equ      3      ;set if final fraction result expected.
ManDtoF         equ      4      ;manual simp=1, Dec->frac bit.
ManForceS       equ      5      ;manual simp=1, Abc=0; force simplify anyhow.
ManAdjW         equ      6      ;manual simp=1, Abc=0; Adjust whole flag.
ManBin          equ      7      ;manual simp=1, Binop set.


;flags for fraction math routines:
FracFlags3      equ     39
FNormOver       equ      0      ;Fraction Normalization Override if set.
DivSmallNum     equ      1      ;set if Frac Numerator < denominator.
IntDivDisp      equ      2      ;integer divide display flag.
StrictInt       equ      3      ;strict integer check by intdiv if set.
yxRatF          equ      4      ;y^x rational approximation flag.
nid             equ      5      ;1 if non integer decimal
SimpDisp        equ      6      ;1 if need to display frac simplify factor.
HadTrueMixed    equ      7      ;at least one true fraction in an expression.
;(HadTrueMixed is needed by >Ab/c<>d/e so that if any mixed fraction, then
; if convert, will stay b/c if b/c mode & not convert back to mixed.)

;
getSendFlg	equ  47
comFailed	equ  3	      ; 1 = get/send communication failed
;
;
apiFlg3		equ  52 
;
apiFlg4		equ  53
fullScrnDraw	equ    2	; draw into last row/col of screen

xapFlag0        equ  43      ; external app flags
xapFlag1        equ  44
xapFlag2        equ  45
xapFlag3        equ  46

plotFlag3	equ  47  
bufferOnly	equ    6
useFastCirc	equ    4

;
varTypeMask	equ  1fh             
varGraphRef	equ    6

;======================================================================
;           character font equates  
;======================================================================
;
LrecurN         equ     001h
LrecurU 	equ	002h
LrecurV 	equ	003h
LrecurW		equ	004h
Lconvert	equ	005h
LsqUp		equ	006h
LsqDown		equ	007h
Lintegral	equ	008h
Lcross		equ	009h
LboxIcon 	equ     00ah
LcrossIcon 	equ     00bh
LdotIcon 	equ     00ch
LsubT   	equ     00dh		;small capital t for parametric mode.
LcubeR  	equ     00eh		;slightly different 3 for cubed root.
LhexF		equ	00fh
Lroot		equ	010h
Linverse	equ	011h
Lsquare		equ	012h
Langle		equ	013h
Ldegree		equ	014h
Lradian		equ	015h
Ltranspose	equ	016h
LLE		equ	017h
LNE		equ	018h
LGE		equ	019h
Lneg		equ	01ah
Lexponent	equ	01bh
Lstore		equ	01ch
Lten		equ	01dh
LupArrow	equ	01eh
LdownArrow	equ	01fh
Lspace		equ	020h
Lexclam		equ	021h
Lquote		equ	022h
Lpound		equ	023h
Lfourth		equ     024h
Lpercent	equ	025h
Lampersand	equ	026h
Lapostrophe	equ	027h
LlParen		equ	028h
LrParen		equ	029h
Lasterisk	equ	02ah
LplusSign	equ	02bh
Lcomma		equ	02ch
Ldash		equ	02dh
Lperiod		equ	02eh
Lslash		equ	02fh
L0		equ	030h
L1		equ	031h
L2		equ	032h
L3		equ	033h
L4		equ	034h
L5		equ	035h
L6		equ	036h
L7		equ	037h
L8		equ	038h
L9		equ	039h
Lcolon		equ	03ah
Lsemicolon	equ	03bh
LLT		equ	03ch
LEQ		equ	03dh
LGT		equ	03eh
Lquestion	equ	03fh
LatSign		equ	040h
LcapA	equ	041h
LcapB	equ	042h
LcapC	equ	043h
LcapD	equ	044h
LcapE	equ	045h
LcapF	equ	046h
LcapG	equ	047h
LcapH	equ	048h
LcapI	equ	049h
LcapJ	equ	04ah
LcapK	equ	04bh
LcapL	equ	04ch
LcapM	equ	04dh
LcapN	equ	04eh
LcapO	equ	04fh
LcapP	equ	050h
LcapQ	equ	051h
LcapR	equ	052h
LcapS	equ	053h
LcapT	equ	054h
LcapU	equ	055h
LcapV	equ	056h
LcapW	equ	057h
LcapX	equ	058h
LcapY	equ	059h
LcapZ	equ	05ah
Ltheta  equ     05bh
Lbackslash	equ	05ch
LrBrack		equ	05dh
Lcaret		equ	05eh
Lunderscore	equ	05fh
Lbackquote	equ	060h
La	equ	061h
Lb	equ	062h
Lc	equ	063h
Ld	equ	064h
Le	equ	065h
Lf	equ	066h
Lg	equ	067h
Lh	equ	068h
Li	equ	069h
Lj	equ	06ah
Lk	equ	06bh
Ll	equ	06ch
Lm	equ	06dh
Ln	equ	06eh
Lo	equ	06fh
Lp	equ	070h
Lq	equ	071h
Lr	equ	072h
Ls	equ	073h
Lt	equ	074h
Lu	equ	075h
Lv	equ	076h
Lw	equ	077h
Lx	equ	078h
Ly	equ	079h
Lz	equ	07ah
LlBrace	equ	07bh
Lbar	equ	07ch
LrBrace	equ	07dh
Ltilde	equ	07eh
LinvEQ	equ	07fh
Lsub0	equ	080h
Lsub1	equ	081h
Lsub2	equ	082h
Lsub3	equ	083h
Lsub4	equ	084h
Lsub5	equ	085h
Lsub6	equ	086h
Lsub7	equ	087h
Lsub8	equ	088h
Lsub9	equ	089h
LcapAAcute	equ	08ah
LcapAGrave	equ	08bh
LcapACaret	equ	08ch
LcapADier	equ	08dh
LaAcute		equ	08eh
LaGrave		equ	08fh
LaCaret		equ	090h
LaDier		equ	091h
LcapEAcute	equ	092h
LcapEGrave	equ	093h
LcapECaret	equ	094h
LcapEDier	equ	095h
LeAcute		equ	096h
LeGrave		equ	097h
LeCaret		equ	098h
LeDier		equ	099h
LcapIAcute	equ	09ah
LcapIGrave	equ	09bh
LcapICaret	equ	09ch
LcapIDier	equ	09dh
LiAcute		equ	09eh
LiGrave		equ	09fh
LiCaret		equ	0a0h
LiDier		equ	0a1h
LcapOAcute	equ	0a2h
LcapOGrave	equ	0a3h
LcapOCaret	equ	0a4h
LcapODier	equ	0a5h
LoAcute		equ	0a6h
LoGrave		equ	0a7h
LoCaret		equ	0a8h
LoDier		equ	0a9h
LcapUAcute	equ	0aah
LcapUGrave	equ	0abh
LcapUCaret	equ	0ach
LcapUDier	equ	0adh
LuAcute		equ	0aeh
LuGrave		equ	0afh
LuCaret		equ	0b0h
LuDier		equ	0b1h
LcapCCed	equ	0b2h
LcCed		equ	0b3h
LcapNTilde	equ	0b4h
LnTilde		equ	0b5h
Laccent		equ	0b6h
Lgrave		equ	0b7h
Ldieresis	equ	0b8h
LquesDown	equ	0b9h
LexclamDown	equ	0bah
Lalpha		equ	0bbh
Lbeta		equ	0bch
Lgamma		equ	0bdh
LcapDelta	equ	0beh
Ldelta		equ	0bfh
Lepsilon	equ	0c0h
LlBrack 	equ     0c1h
Llambda		equ	0c2h
Lmu		equ	0c3h
Lpi		equ	0c4h
Lrho		equ	0c5h
LcapSigma	equ	0c6h
Lsigma		equ	0c7h
Ltau		equ	0c8h
Lphi		equ	0c9h
LcapOmega	equ	0cah
LxMean		equ	0cbh
LyMean		equ	0cch
LsupX		equ	0cdh
Lellipsis	equ	0ceh
Lleft		equ	0cfh
Lblock		equ	0d0h
Lper		equ	0d1h
Lhyphen		equ	0d2h
Larea		equ	0d3h
Ltemp		equ	0d4h
Lcube		equ	0d5h
Lenter		equ	0d6h
LimagI		equ	0d7h
Lphat		equ	0d8h
Lchi		equ	0d9h
LstatF		equ     0dah
Llne		equ	0dbh
LlistL		equ	0dch
LfinanN 	equ	0ddh
L2_r_paren 	equ	0deh
LblockArrow     equ     0dfh
LcurO   	equ     0e0h
LcurO2  	equ     0e1h
LcurOcapA       equ     0e2h
LcurOa  	equ     0e3h
LcurI   	equ     0e4h
LcurI2  	equ     0e5h
LcurIcapA       equ     0e6h
LcurIa  	equ     0e7h
LGline          equ     0e8h    ; = 0
LGthick         equ     0e9h    ; = 1
LGabove         equ     0eah    ; = 2
LGbelow         equ     0ebh    ; = 3
LGpath          equ     0ech    ; = 4
LGanimate       equ     0edh    ; = 5
LGdot           equ     0eeh    ; = 6
LGpBlk          equ     0efh    ;up arrow and block in solver
LDnBlk          equ     0f0h    ;down arrow and block in solver
LcurFull        equ     0f1h    ;note: must be last char (putmap checks)

;========= floating point

FPLEN           equ     9               ;length of a floating-point number

;========= constant table equates

drConst 	equ	0
pi2Const	equ	1
pi4Const	equ	2
logeConst	equ	3
piConst 	equ	4
DtoRConst	equ	5
ln10		equ	6

;========== text screen
textRows	equ	8
textCols        equ     16

;========== alphatext screen
alphabuffersize equ	16
;
;========== msc fraction equates
mantSignBit     equ  7       ;msbit of data type byte is set if negative.
redF            equ  1       ;reduced if 0, unreduced if 1.
unredF          equ  1       ;reduced if 0, unreduced if 1.
simpF           equ  0       ;simple if 0, mixed if 1.
mixedF          equ  0       ;simple if 0, mixed if 1.
;

;
;

;====== keyboard key names
;
;       key presses that do not echo into the display
;       and are not application or menu loading key presses
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
kCont           equ       01Ah
kLtoTI83        equ       01Bh
kSendApp        equ       01Ch
kvApps          equ       01Ch
kSendId         equ       01Dh
kSendSW         equ       01Eh
kvALL           equ       01Fh
kvSendType      equ       020h  ;send a particular data type (in kextend)
kNoWay          EQU       021h
kYes            EQU       022h
kAbout          equ       023h  ;about screen
kApp            equ       024h
;
kProgExec       equ       025h
kListflag       equ       026h
;
;open for expansion       028h..02bh
;

menustart       equ       02ch
;
;       key presses that do not echo into the display
;       and are menu switching key presses
;
kAreYouSure     equ       02Ch
kPrgm           equ       02Dh
kZoom           equ       02Eh        ; PULL DOWN
kDraw           equ       02Fh
kSPlot          equ       030h
kStat           equ       031h
kMath           equ       032h
kTest           equ       033h
kAppsMenu       equ       034h
kVars           equ       035h
kMem            equ       036h
kConstSetmenu   equ       037h
kConvMenu       equ       038h
kTrigmenu       equ       039h
kLIST           equ       03Ah
kConstMenu      equ       03Bh
;
menuend         equ       kConstMenu
;
kConst          EQU       03Ch
;
;
kAlpha          equ       03dh
kCatalog        equ       03eh        ; catalog
kInputDone      equ       3fh

;
;       key presses that do not echo into the display
;       and are context switching key presses
;
kQuit           equ       040h
kLinkIO         equ       041h
kChkApps        equ       042h        ; Check Apps
kStatEd         equ       043h
kGraph          equ       044h
kMode           equ       045h
kPrgmEd         equ       046h        ; PROGRAM EDIT
kPrgmCr         equ       047h        ; PROGRAM CREATE
kWindow         equ       048h        ; RANGE EDITOR
kYequ           equ       049h        ; EQUATION EDITOR
kTable          equ       04Ah        ; TABLE EDITOR
kTblSet         equ       04Bh        ; TABLE SET
kChkRAM         equ       04CH        ; CHECK RAM
kDelMem         equ       04DH        ; DELETE MEM
kResetMem       equ       04EH        ; RESET MEM
kResetDef	equ	  04FH        ; RESET DEFAULT
kPrgmInput      EQU       050H        ; PROGRAM INPUT
kZFactEd        EQU       051H        ; ZOOM FACTOR EDITOR
kError          EQU       052H        ; ERROR
kExtApps        EQU       053h        ; Extra Applications.     NEW
kSolveRoot	equ	  054h        ; SOLVE EDITOR
kStatP          equ       055h        ; stat plot
kConstEd        equ       056h        ; Constant equ Editor.
kPrgmSetVar     equ       057h        ; SetMenu in a Program.
kFormat         equ       058h        ; FORMAT
;
;
append          equ       kFormat
;
;       58h - 59h are empty for growth
;
echoStart1      equ       05ah
;
;       conditional keys that if in prog edit will echo
;       or if not in prog edit go to the graph screen
;       'trace' , 'zooms'
;
;
kTrace          equ       05ah
kZFit           equ       05bh
kZIn            equ       05ch
kZOut           equ       05dh
kZPrev          equ       05eh
kZBox           equ       05fh
kZDecml         equ       060h
kSetZm          equ       061h
kZSquar         equ       062h
kZStd           equ       063h
kZTrig          equ       064h
kZQuad1         equ       065h
;kusrzm          equ       065h
;kzsto           equ       066h
kZInt           equ       067h
kZStat          equ       068h
;
;       conditional keys that if in prog edit or if the
;       graph screen is not displayed will echo, else will
;       go to the graph screen. 'draw' menu, eval
echoStart2      equ       069h
;
;
kSelect         equ       069h
kCircl          equ       06ah
kClDrw          equ       06bh
kLine           equ       06ch
kPen            equ       06dh
kPtChg          equ       06eh
kPtOff          equ       06fh
kPtOn           equ       070h
kVert           equ       071h
kHoriz          equ       072h
kText           equ       073h
kTanLn          equ       074h
kManFit         equ       075h
;
;   these keys will always force to graph mode
;
kEval           equ       076h
;
echoStart       equ       07dh
;
;       key presses that echo into a buffer
;
kRemain         equ       07dh          ;only needed to display 'r'
kPlot3          equ       07eh
kListName       equ       07fh
kAdd            equ       080h
kSub            equ       081h
kMul            equ       082h
kDiv            equ       083h
kExpon          equ       084h
kLParen         equ       085h
kRParen         equ       086h
kLBrack         equ       087h
kRBrack         equ       088h
kShade          equ       089h
kStore          equ       08ah
kComma          equ       08bh
kChs            equ       08ch
kDecPnt         equ       08dh
k0              equ       08eh
k1              equ       08fh
k2              equ       090h
k3              equ       091h
k4              equ       092h
k5              equ       093h
k6              equ       094h
k7              equ       095h
k8              equ       096h
k9              equ       097h
kEE             equ       098h
kSpace          equ       099h
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
;kclrtbl         equ       0c4h
;
kAns            equ       0c5h
kColon          equ       0c6h
;

kUnit           equ       0c7h          ;                       new
kFracSlash      equ       0c8h          ;                       new
kRoot           equ       0c9h
;
kQuest          equ       0cah        ; question mark
kQuote          equ       0cbh        ; "
kTheta          equ       0cch
kIf             equ       0cdh
kThen           equ       0ceh
kElse           equ       0cfh
kFor            equ       0d0h
kWhile          equ       0d1h
kRepeat         equ       0d2h
kEnd            equ       0d3h
kPause          equ       0d4h
kLbl            equ       0d5h
kGoto           equ       0d6h
kISG            equ       0d7h
kDSL            equ       0d8h
kMenu           equ       0d9h
kExec           equ       0dah
kReturn         equ       0dbh
kStop           equ       0dch
kInput          equ       0ddh
kPrompt         equ       0deh
kDisp           equ       0dfh
kDispG          equ       0e0h
kDispT          equ       0e1h
kOutput         equ       0e2h
kGetKey         equ       0e3h
kClrHome        equ       0e4h
;
;kprtscr         equ       0e5h
;
kPercent        equ        0e6h         ;%                      new
kMixSimp        equ        0e7h         ;                       new
kFracDec        equ        0e8h         ;                       new
kSimp           equ        0e9h         ;                       new
kUnredF         equ        0eah         ;                       new
kIDiv           equ        0ebh         ;                       new
kConvert        equ       0ech          ;                       new
kLBrace         equ       0edh
kRBrace         equ       0eeh
kL1A            equ       0efh
kL2A            equ       0f0h
kL3A            equ       0f1h
kL4A            equ       0f2h
kL5A            equ       0f3h
kL6A            equ       0f4h
kConvLength     equ       0f5h        ;1st byte of conversion key.
kConvArea       equ       0f6h        ;1st byte of conversion key.
kConvVolume     equ       0f7h        ;1st byte of conversion key.
kConvTime       equ       0f8h        ;1st byte of conversion key.
kConvTemp       equ       0f9h        ;1st byte of conversion key.
kConvMass       equ       0fah        ;1st byte of conversion key.
kConvSpeed      equ       0fbh        ;1st byte of conversion key.
;
;       this key means that it is a 2 byte keycode
;       there are 2 of these keys be careful
;
;
kExtendEcho2    equ       0fch
;
;
;       this key means that the key press is one that echos
;       into a buffer, and it is a 2 byte key code, go look at
;       (extecho) for the key value
;
kExtendEcho     equ       0feh
;
kE1BT           equ       0

kDrawInv        equ       kE1BT
kDrawF          equ       kE1BT+1
kPixelOn        equ       kE1BT+2
kPixelOff       equ       kE1BT+3
kPxlTest        equ       kE1BT+4
kRCGDB          equ       kE1BT+5
kRCPic          equ       kE1BT+6
kSTGDB          equ       kE1BT+7
kSTPic          equ       kE1BT+8
kAbs            equ       kE1BT+9
kTEqu           equ       kE1BT+10    ; ==
kTNoteQ         equ       kE1BT+11    ; <>
kTGT            equ       kE1BT+12    ; >
kTGTE           equ       kE1BT+13    ; > =
kTLT            equ       kE1BT+14    ; <
kTLTE           equ       kE1BT+15    ; < =

kAnd            equ       kE1BT+16
kOr             equ       kE1BT+17
kXor            equ       kE1BT+18
kNot            equ       kE1BT+19

;klr1            equ       kE1BT+20

kXRoot          equ       kE1BT+21
kCube           equ       kE1BT+22
kCbRt           equ       kE1BT+23    ; cube root
kToDec          equ       kE1BT+24
;
kCubicR         equ       kE1BT+25
kQuartR         equ       kE1BT+26
;
kPlot1          equ       kE1BT+27
kPlot2          equ       kE1BT+28
;

kRound          equ       kE1BT+29
kIPart          equ       kE1BT+30
kFPart          equ       kE1BT+31
kInt            equ       kE1BT+32

kRand           equ       kE1BT+33
kNPR            equ       kE1BT+34
kNCR            equ       kE1BT+35
kXFactorial     equ       kE1BT+36

kRad            equ       kE1BT+37
kDegr           equ       kE1BT+38    ; degrees conv
kApost          equ       kE1BT+39
kToDMS          equ       kE1BT+40
kRtoPo          equ       kE1BT+41    ; r
kRtoPr          equ       kE1BT+42
kPtoRx          equ       kE1BT+43
kPtoRy          equ       kE1BT+44

kRowSwap        equ       kE1BT+45
kRowPlus        equ       kE1BT+46
kTimRow         equ       kE1BT+47
kTRowP          equ       kE1BT+48

kSortA          equ       kE1BT+49
kSortD          equ       kE1BT+50
kSeq            equ       kE1BT+51

kMin            equ       kE1BT+52
kMax            equ       kE1BT+53
kMean           equ       kE1BT+54
kMedian         equ       kE1BT+55
kSum            equ       kE1BT+56
kProd           equ       kE1BT+57

kDet            equ       kE1BT+58
kTransp         equ       kE1BT+59
kDim            equ       kE1BT+60    ; same as one in list
kFill           equ       kE1BT+61    ; same as one in list
kIdent          equ       kE1BT+62
kRandm          equ       kE1BT+63
kAug            equ       kE1BT+64

kLstMode        equ       kE1BT+65
;available      equ       kE1BT+66

kmRad           equ       kE1BT+67
kmDeg           equ       kE1BT+68
kmNormF         equ       kE1BT+69
kmSci           equ       kE1BT+70
kFix            equ       kE1BT+71
kmFloat         equ       kE1BT+72
kSetMenu        equ       kE1BT+73
kEOFrac         equ       kE1BT+74

kBoyIcon        equ     kE1BT+75
kTreeIcon       equ     kE1BT+76
kFaceIcon       equ     kE1BT+77
kCircleIcon     equ     kE1BT+78
kDiamondIcon    equ     kE1BT+79
kStarIcon       equ     kE1BT+80
kDollarIcon      equ    kE1BT+81
;available      equ     kE1BT+82
;available      equ     kE1BT+83

kFNOn           equ       kE1BT+84
kFNOff          equ       kE1BT+85

kPlotsOn        equ       kE1BT+86
kPlotsPff       equ       kE1BT+87

kPixelChg       equ       kE1BT+88

kSendMBL        equ       kE1BT+89
kRecvMBL        equ       kE1BT+90

kBoxPlot        equ       kE1BT+ 91
kBoxIcon        equ       kE1BT+ 92

kCrossIcon      equ       kE1BT+ 93
kDotIcon        equ       kE1BT+ 94

kE2BT           equ       kE1BT+95

kSeqential      equ       kE2BT         ;105
kSimulG         equ       kE2BT+1       ;106
kCoordOn        equ       kE2BT+2       ;107
kCoordOff       equ       kE2BT+3       ;108
kDrawLine       equ       kE2BT+4       ;109
kDrawDot        equ       kE2BT+5       ;110
kAxisOn         equ       kE2BT+6       ;111
kAxisOff        equ       kE2BT+7       ;112
kGridOn         equ       kE2BT+8       ;113
kGridOff        equ       kE2BT+9       ;114
kLblOn          equ       kE2BT+10      ;115
kOblOff         equ       kE2BT+11      ;116
kPolarG         equ       kE2BT+12      ;117
kRectG          equ       kE2BT+13      ;118

kL1             equ       kE2BT+14      ;119 = 77h
kL2             equ       kE2BT+15
kL3             equ       kE2BT+16
kL4             equ       kE2BT+17
kL5             equ       kE2BT+18
kL6             equ       kE2BT+19

kMatA          equ       kE2BT+20
kMatB           equ       kE2BT+21
kMatC           equ       kE2BT+22
kMatD           equ       kE2BT+23
kMatE           equ       kE2BT+24

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
;
kABC         equ       kE2BT+55
kBC           equ       kE2BT+56
kAutoSimp       equ       kE2BT+57
kManSimp        equ       kE2BT+58

kDelLast        equ       kE2BT+59
ksingleC        equ       kE2BT+60
kMultiC         equ       kE2BT+61

kPic1           equ       kE2BT+62
kPic2           equ       kE2BT+63
kPic3           equ       kE2BT+64

kDelVar         equ       kE2BT+65
kGetCalc        equ       kE2BT+66

kStndrd         equ       kE2BT+67
kParam          equ       kE2BT+68
kPolar          equ       kE2BT+69
kAFillOn        equ       kE2BT+70
kAFillOff       equ       kE2BT+71
kACalcOn        equ       kE2BT+72
kACalcOff       equ       kE2BT+73

kSetConst       equ       kE2BT+74
;
kGraphStyle     equ       kE2BT+75
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
kc1             equ       kE2BT+101   ;constant c1
kc2             equ       kE2BT+102   ;constant c2
kc3             equ       kE2BT+103   ;constant c3
kc4             equ       kE2BT+104   ;constant c4
kPlotStart      equ       kE2BT+105   ; plotstart
kZPlotStart     equ       kE2BT+106   ; zplotstart
kXFact          equ       kE2BT+107   ; xfact
kYFact          equ       kE2BT+108   ; yfact
kMaxY           equ       kE2BT+109   ; maxy
kTblInput       equ       kE2BT+110   ; web off
kPlotStep       equ       kE2BT+111
kZPlotStep      equ       kE2BT+112
kSelectA        equ       kE2BT+113
kZFitA          equ       kE2BT+114
kOneVar         equ       kE2BT+115
kTwoVar         equ       kE2BT+116
kLR             equ       kE2BT+117
kLRExp          equ       kE2BT+118
kLRLn           equ       kE2BT+119
;klrpwr          equ       kE2BT+
;kmanfit         equ       kE2BT+
kMedMed         equ       kE2BT+120
kQuad           equ       kE2BT+121
kClrLst         equ       kE2BT+122
kHist           equ       kE2BT+123
kXYLine         equ       kE2BT+124
kScatter        equ       kE2BT+125
kLR1            equ       kE2BT+126
kClrTbl         equ       kE2BT+127
kE2BT_end       equ       kClrTbl
;
;

; new 2 byte keys

kE2BT2		EQU       0

kGDB1		EQU       kE2BT2
kGDB2		EQU       kE2BT2+1
kGDB3		EQU       kE2BT2+2
kY1		EQU       kE2BT2+3
kY2		EQU       kE2BT2+4
kY3		EQU       kE2BT2+5
kY4		EQU       kE2BT2+6
kY5		EQU       kE2BT2+7
kY6		EQU       kE2BT2+8
kY7		EQU       kE2BT2+9
kY8		EQU       kE2BT2+10
kY9		EQU       kE2BT2+11
kY0		EQU       kE2BT2+12
kX1T		EQU       kE2BT2+13
kY1T		EQU       kE2BT2+14
kX2T		EQU       kE2BT2+15
kY2T		EQU       kE2BT2+16
kX3T		EQU       kE2BT2+17
kY3T		EQU       kE2BT2+18
kX4T		EQU       kE2BT2+19
kY4T		EQU       kE2BT2+20
kX5T		EQU       kE2BT2+21
kY5T		EQU       kE2BT2+22
kX6T		EQU       kE2BT2+23
kY6T		EQU       kE2BT2+24
kR1		EQU       kE2BT2+25
kR2		EQU       kE2BT2+26
kR3		EQU       kE2BT2+27
kR4		EQU       kE2BT2+28
kR5		EQU       kE2BT2+29
kR6		EQU       kE2BT2+30
kGDB4		EQU       kE2BT2+31
kGDB5		EQU       kE2BT2+32
kGDB6		EQU       kE2BT2+33
kPic4		EQU       kE2BT2+34
kPic5		EQU       kE2BT2+35
kPic6		EQU       kE2BT2+36
kGDB7		EQU       kE2BT2+37
kGDB8		EQU       kE2BT2+38
kGDB9		EQU       kE2BT2+39
kGDB0		EQU       kE2BT2+40
kPic7		EQU       kE2BT2+41
kPic8		EQU       kE2BT2+42
kPic9		EQU       kE2BT2+43
kPic0		EQU       kE2BT2+44
kStatN		EQU       kE2BT2+45
kXMean		EQU       kE2BT2+46
;kconj           equ       kE2BT2+47
;kreal           equ       kE2BT2+48
;kfangle         equ       kE2BT2+49
kLCM            equ       kE2BT2+50
kGCD            equ       kE2BT2+51
kRandInt        equ       kE2BT2+52
kCoin           equ       kE2BT2+53
kDice           equ       kE2BT2+54
kRemainder      equ       kE2BT2+55
kYMean          equ       kE2BT2+56
kStdX           equ       kE2BT2+57
kStdX1          equ       kE2BT2+58
kw0             equ       kE2BT2+59
kMatf           equ       kE2BT2+60
kMatg           equ       kE2BT2+61
kMatrh          equ       kE2BT2+62
kMati           equ       kE2BT2+63
kMatj           equ       kE2BT2+64
kYMean1         equ       kE2BT2+65
kStdY           equ       kE2BT2+66
kStdY1          equ       kE2BT2+67
kMaTtoLst       equ       kE2BT2+68
kLstToMat       equ       kE2BT2+69
kCumSum         equ       kE2BT2+70
kDeltaLst       equ       kE2BT2+71
kStdDev         equ       kE2BT2+72
kVariance       equ       kE2BT2+73
kLength         equ       kE2BT2+74
kEquToStrng     equ       kE2BT2+75
kStrngToEqu     equ       kE2BT2+76
kExpr           equ       kE2BT2+77
kSubStrng       equ       kE2BT2+78
kInStrng        equ       kE2BT2+79
kStr1           equ       kE2BT2+80
kStr2           equ       kE2BT2+81
kStr3           equ       kE2BT2+82
kStr4           equ       kE2BT2+83
kStr5           equ       kE2BT2+84
kStr6           equ       kE2BT2+85
kStr7           equ       kE2BT2+86
kStr8           equ       kE2BT2+87
kStr9           equ       kE2BT2+88
kStr0           equ       kE2BT2+89
kSimpFact       equ       kE2BT2+90
kPictPlot       equ     kE2BT2+91
kCirclePlot     equ     kE2BT2+92
kStemPlot       equ     kE2BT2+93
kBarPlot        equ     kE2BT2+94
kSumX           equ       kE2BT2+102
kSumX2          equ       kE2BT2+103

kSumY           equ       kE2BT2+104
kXRes           equ       kE2BT2+105

kSumY2          equ       kE2BT2+106
kSumXY          equ       kE2BT2+107
kuXRes          equ       kE2BT2+108
kModBox         equ       kE2BT2+109
kNormProb       equ       kE2BT2+110

kMinY           equ       kE2BT2+111   ; miny
kRef            equ       kE2BT2+112
kRRef           equ       kE2BT2+113
kLRSqr          equ       kE2BT2+114
kBRSqr          equ       kE2BT2+115
kDiagOn         equ       kE2BT2+116
kDiagOff        equ       kE2BT2+117
;kun1            equ       kE2BT2+152   ; for rcl use when gotten from 82
;kvn1            equ       kE2BT2+153   ; for rcl use when gotten from 82
kE2BT2_end      equ       kDiagOff

;=================================================
;    context equates
;=================================================
cxCmd           equ     kQuit                   ;home screen
cxAppsApp       equ     kChkApps                ;apps usage screen
cxPrgmEdit      equ     kPrgmEd                 ;program editor
cxEquEdit       equ     kYequ                   ;equation editor
cxGrRange       equ     kWindow                 ;graph range editor
cxGrZfact       equ     kZFactEd                ;graph zoom factors editor
cxGraph         equ     kGraph                  ;graph mode
cxStatEdit      equ     kStatEd                 ;statistics list editor
cxPrgmInput     equ     kPrgmInput              ;programmed input
cxError         equ     kError                  ;error handler
cxLinkIO        equ     kLinkIO                 ;link i/o interface
cxMem           equ     kResetMem               ;reset memory
cxDefMem        equ     kResetDef               ;reset default
cxRAMApp        equ     kChkRAM                 ;ram usage screen
cxMode          equ     kMode                   ;mode settings screen
cxErase         equ     kDelMem                 ;memory erase
cxPrgmCreate    equ     kPrgmCr                 ;program create
cxTableEditor   equ     kTable                  ;table editor
cxTableSet      equ     kTblSet                 ;table set up
cxStatPlot      equ     kStatP                  ;stat plots
cxFormat        equ     kFormat                 ;format context
cxSolveRoot     equ     kSolveRoot              ;solve root
cxextapps       equ     kExtApps        ;053h   ; extra applications.     new
cxConstEd       equ     kConstEd        ;056h   ; constant equ editor.
cxPrgmSetVar    equ     kPrgmSetVar     ;057h   ; setmenu in a program.

;============================================
;	scan code equates
;============================================

skDown          EQU	01h
skLeft          EQU	02h
skRight         EQU	03h
skUp            EQU	04h
skEnter         EQU	09h
skAdd           EQU	0Ah
skSub           EQU	0Bh
skMul           EQU	0Ch
skDiv           EQU	0Dh
skConst         equ              0eh
skClear         equ              0fh
skChs           equ              11h
sk3             equ              12h
sk6             equ              13h
sk9             equ              14h
skRParen        equ              15h
skMixSimp       equ              16h
skAppsMenu      equ              17h
skDecPnt        equ              19h
sk2             equ              1ah
sk5             equ              1bh
sk8             equ              1ch
skLParen        equ              1dh
skFracDec       equ              1eh
skPrgm          equ              1fh
skStatEd        equ              20h
sk0             equ              21h
sk1             equ              22h
sk4             equ              23h
sk7             equ              24h
skPercent       equ              25h
skFracSlash     equ              26h
skExpon         equ              27h
skDraw          equ              28h
skStore         equ              2ah
skComma         equ              2bh
skVarX          equ              2ch
skSimp          equ              2dh
skUnit          equ              2eh
skSquare        equ              2fh
skMath          equ              30h
skGraph         equ              31h
skTrace         equ              32h
skZoom          equ              33h
skWindow        equ              34h
skYEqu          equ              35h
sk2nd           equ              36h
skMode          equ              37h
skDel           equ              38h

;==========================================
;	tokens
;==========================================

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
tUnredF         equ     03h     ;down arrow for unreduced fraction token

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

;   immediate unary tokens go from here to 'eosel-1'
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
tLstMode        equ       IMUN+16     ; 22h 'mode('
;the below tokens are immediate, but require an expression as 1st arg & so
;are grouped together...
tRoot           equ       IMUN+17     ; 23h 'solve('
tSeries         equ       IMUN+18     ; 24h 'seq'
tEvalF          equ       IMUN+20     ; 26h             ;keep until sure.
IMUNEND         equ     IMUN+20         ;end of immediate unaries.

tSolveEqual     equ     IMUN+21         ;teq ('=') in solver -> tsolveequal.
tEOFrac         equ     IMUN+22         ; 28h end of fraction in home screen.
;
;
;  token 29h cannot be an eos function since el(=29h already
;
tEOSEL          EQU       IMUN+23
;
tSpace          EQU       tEOSEL       ; 29h   ' '
tString         EQU       tEOSEL+1     ; 2Ah  '"'
tComma          EQU       tEOSEL+2     ; 2Bh  ','
tRemain         equ     2ch     ;'r' for remainder display.
; tremain is needed in home screen where turn display chars into tokens.
; (there is no token for small 'r').
; ===== postfix functions
;
tPost           equ       2eh
;
tFact           equ       tPost       ; 2eh '!'
tPercent        equ     tPost+1       ; 2fh '%'                 new

;=============number tokens

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

;=======================
;   binary op
;=======================
tOr             EQU       3Ch         ; 3Ch  '_or_'
tXor            EQU       3Dh         ; 3Dh
;
tColon          EQU       3Eh         ; 3Eh ':'
tEnter          EQU       3Fh         ; 3Fh  Lenter
;
tAnd            EQU       40h         ; 40h  '_and_'       

;========================
;       letter tokens
;========================
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
tEOF            equ       LET+26      ; 5bh
;
;===============================================
;   these var tokens are 1st of a double token
;===============================================
;
;======================================================================
;   THESE VAR TOKENS ARE 1ST OF A DOUBLE TOKEN 
;======================================================================

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

;================================
; ===== mode setting commands
;================================
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

;=======================
;   binary op
;=======================
tAdd            equ       70h         ; 70h '+'
tSub            equ       71h         ; 71h '-'
;
tAns            equ       72h         ; 72h
;
;============================
;===== mode setting commands
;============================
;       graph format tokens are 2 byte tokens
;
tGFormat        equ     73h
;
;
tBoyIcon        equ     78h           ; for stat plots in programs
tTreeIcon       equ     79h           ; for stat plots in programs
tDollarIcon     equ     7ah           ; for stat plots in programs
tFaceIcon       equ     7bh           ; for stat plots in programs
tCircleIcon     equ     7ch           ; for stat plots in programs
tDiamondIcon    equ     7dh           ; for stat plots in programs
tStarIcon       equ     7eh           ; for stat plots in programs
tBoxIcon        equ       7fh
tCrossIcon      equ       80h
tDotIcon        equ       81h
;
;===========================
;       binary op cont.
;===========================
tMul            equ       82h         ; 82h  '*'
tDiv            equ       83h         ; 83h  '/'
tIDiv           equ       84h         ; 84h  'int/'
;
;============================
;       some graph commands
;============================
;
GRCMDS          equ       85h
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
tZoomQuad1      equ       GRCMDS+12   ; 91h 'quadrant 1'
tZoomSto        equ       GRCMDS+14   ; 93h  zoom store
tText           equ       GRCMDS+15   ; 94h
;
;=======================
;       binary op
;=======================
tnPr            EQU       GRCMDS+16   ; 94h '_nPr_'
tnCr            EQU       GRCMDS+17   ; 95h '_nCr_'

;=========================================================================
;       more graph commands (start of graph commands requiring arguments)
;=========================================================================

tYOn            EQU       GRCMDS+18   ; 96h 'FnOn_'
tYOff           EQU       GRCMDS+19   ; 97h 'FnOff_'
tStPic          EQU       GRCMDS+20   ; 98h 'StPic_'
tRcPic          EQU       GRCMDS+21   ; 99h 'RcPic_'
tLine           EQU       GRCMDS+22   ; 9bh 'line'
tVert           EQU       GRCMDS+23   ; 9ch 'vert_'
tPtOn           EQU       GRCMDS+24   ; 9dh 'pton'
tPtOff          EQU       GRCMDS+25   ; 9eh 'ptoff'
;
;===============================================================
;  token a0 cannot be an eos function since low mult=a0 already
;===============================================================
tPtChg          EQU       GRCMDS+26   ; 9fh 'ptchg'
tPXOn           EQU       GRCMDS+27   ; a0h
tPXOff          EQU       GRCMDS+28   ; a1h
tPXChg          EQU       GRCMDS+29   ; a2h
tShade          EQU       GRCMDS+30   ; a3h 'shade'
tCircl          EQU       GRCMDS+31   ; a4h 'circl'
tHorz           EQU       GRCMDS+32   ; a5h 'horizontal'
tTanLn          EQU       GRCMDS+33   ; a6h 'tanln'
;tManFit         equ       GRCMDS+34   ; a7h 'manual fit'

;
;available      equ       GRCMDS+35   ; a8h
;
tDrInv          equ       GRCMDS+36   ; a9h 'drinv_'
tDrawF          equ       GRCMDS+36   ; a9h 'drawf_'
;
tVarStrng       equ       0aah
;
;===================================
; ===== functions with no arguments
;===================================
;
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
;tUnit            EQU       UNARY+8     ; B8h
tIPart          EQU       UNARY+9     ; B9h
tFPart          EQU       UNARY+10    ; BAh
;
;================================
;       new 2 byte tokens
;================================
t2ByteTok       equ       0bbh         ;
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
LASTUNARY2       equ       UNARYLR+11  ; c7h
tMixSimp        equ     UNARYLR+12 ;c8convert between mixed(ab/c) & simple(b/c)
tFracDec        equ     UNARYLR+13 ;c9h convert between fraction & decimal.
tSimp           equ     UNARYLR+14 ;cah simplify a fraction.

;=================
;  binary op
;=================
tPower          equ       0cbh        ; cbh    '^'
tXRoot          equ       0cch        ; cch    lsupx,lroot

;===================================
;       some programming commands
;===================================
;
PROGTOK equ     UNARYLR+17
;
tSetMenu        equ       PROGTOK     ; cdh
tIf             equ       PROGTOK+1   ; ceh
tThen           equ       PROGTOK+2   ; cfh
tElse           equ       PROGTOK+3   ; d0h
tWhile          equ       PROGTOK+4   ; d1h
tRepeat         equ       PROGTOK+5   ; d2h
tFor            equ       PROGTOK+6   ; d3h
tEnd            equ       PROGTOK+7   ; d4h
tReturn         equ       PROGTOK+8   ; d5h
tLbl            equ       PROGTOK+9   ; d6h   'lbl_'
tGoto           equ       PROGTOK+10  ; d7h   'goto_'
tPause          equ       PROGTOK+11  ; d8h   'pause_'
tStop           equ       PROGTOK+12  ; d9h   'stop'
tISG            equ       PROGTOK+13  ; dah   'is>'
tDSL            equ       PROGTOK+14  ; dbh   'ds<'
tInput          equ       PROGTOK+15  ; dch   'input_'
tPrompt         equ       PROGTOK+16  ; ddh   'prompt_'
tDisp           equ       PROGTOK+17  ; deh   'disp_'
tDispG          equ       PROGTOK+18  ; dfh   'dispg'
;
tOutput         equ       PROGTOK+19  ; e0h    'outpt'
tClLCD          equ       PROGTOK+20  ; e1h    'cllcd'
tFill           equ       PROGTOK+21  ; e2h    'fill'
tSortA          equ       PROGTOK+22  ; e3h    'sorta_'
tSortD          equ       PROGTOK+23  ; e4h    'sortd_'
tDispTab        equ       PROGTOK+24  ; e5h    'disp table
tMenu           equ       PROGTOK+25  ; e6h    'menu'
tSendMBL        equ       PROGTOK+26  ; e7h    'send'
tGetMBL         equ       PROGTOK+27  ; e8h    'get'
;
;===================================
;       stat plot commands
;===================================
statPCmd        equ       PROGTOK+28
;
tPlotOn         equ       statPCmd    ; e9h     ' plotson'
tPlotOff        equ       statPCmd+1   ; eah     ' plotsoff
CmdLen0         equ     tPlotOff - PROGTOK+ 1
CmdOff1         equ     CmdLen0 - 1
;
tListName       equ       0ebh        ; list designator
;
tPlot1          equ       0ech
tPlot2          equ       0edh
tPlot3          equ       0eeh

tUnit           equ       0efh        ; efh unit for mixed fractions (a u b/c)
tFracSlash      equ       0f0h         ; f0h '/' (bc symbol)

tStatCmd         equ       0f2h
;
;==================================
; 2nd half of graph format tokens
;
; ===== format settings commands
;===================================

GFMT            equ       1
;
tSeq            equ       GFMT        ; 01  'seqg'
tSimulG         equ       GFMT+1      ; 02  'simulg'
tCoordOn        equ       GFMT+2      ; 03  'coordon'
tCoordOff       equ       GFMT+3      ; 04  'coordoff'
tDrawLine       equ       GFMT+4      ; 05  'drawline'
tDrawDot        equ       GFMT+5      ; 06  'drawdot'
tAxisOn         equ       GFMT+6      ; 07  'axeson'
tAxisOff        equ       GFMT+7      ; 08  'axesoff'
tGridOn         equ       GFMT+8      ; 09  'gridon'
tGridOff        equ       GFMT+9      ; 0a  'gridoff'
tLblOn          equ       GFMT+10     ; 0b  'labelon'
tLblOff         equ       GFMT+11     ; 0c  'labeloff'
tPolarG         equ       GFMT+12     ; 0d  'polargc'
tRectG          equ       GFMT+13     ; 0e  'rectgc'
;
;=====================================
; 2nd half of mode setting tokens
;=====================================
MODES2          equ       GFMT+14
;
tStndrd         equ       MODES2      ; 0fh 'func'
tParam          equ       MODES2+1    ; 10h 'param'
tPolar          equ       MODES2+2    ; 11h 'pol'
tAFillOn        equ       MODES2+3    ; 12h 'auto fill on
tAFillOff       equ       MODES2+4    ; 13h
tACalcOn        equ       MODES2+5    ; 14h
tACalcOff       equ       MODES2+6    ; 15h 'autofill off
;
tMABC           equ       MODES2+7    ; 16h 'aub/c mode'
tMBC            equ       MODES2+8    ; 17h 'b/c mode'
tMAutoSimp      equ       MODES2+9    ; 18h 'b/c mode'
tMManSimp       equ       MODES2+10   ; 19h 'b/c mode'
tMSingleC       equ       MODES2+11   ; 1ah single constant
tMMultiC        equ       MODES2+12   ; 1bh multi constant


STATREGS        equ     1
tOneVar         equ     STATREGS      ; 01h    'onevar_'
tTwoVar         equ     STATREGS+1    ; 02h
tLR1            equ     STATREGS+2    ; 03h    'linr(ax+b
tManFit         equ     STATREGS+5   ;07h     'manual fit'
tMedMed         equ     STATREGS+6    ; 08h
tQuad           equ     STATREGS+7    ; 09h
tClrLst         equ     STATREGS+8    ; 0ah    'clear list
tClrTbl         equ     STATREGS+9    ; 0bh    clear table
tHist           equ     STATREGS+10   ; 0ch    'hist_'
tXYLine         equ     STATREGS+11   ; 0dh    'xyline_'
tScatter        equ     STATREGS+12   ; 0eh    'scatter_'
tLR             equ     STATREGS+13   ; 0fh    'linr(a+bx

;======================================
;       2nd half of user list tokens
;======================================
tL1             equ       01h         ; list 1
tL2             equ       02h         ; list 2
tL3             equ       03h         ; list 3
tL4             equ       04h         ; list 4
tL5             equ       05h         ; list 5
tL6             equ       06h         ; list 6

;===========================================
;       2nd half of user equation tokens
;===========================================
;
tY1             equ       10h         ; y1
tY2             equ       11h         ; y2
tY3             equ       12h         ; y3
tY4             equ       13h         ; y4
;
;       constant equations have bit 7 set
tC1             equ       80h         ; const1
tC2             equ       81h         ; const2
tC3             equ       82h         ; const3
tC4             equ       83h         ; const4
;
;===========================================
;       2nd half of user picture tokens
;===========================================
tPic1           equ       01h         ; pic1
tPic2           equ       02h         ; pic2
tPic3           equ       03h         ; pic3

;==============================================
;       2nd half of user graph database tokens
;==============================================
tGDB1           equ       01h         ; gdb1
tGDB2           equ       02h         ; gdb2
tGDB3           equ       03h         ; gdb3
tGDB4           equ       04h         ; gdb4
tGDB5           equ       05h         ; gdb5
tGDB6           equ       06h         ; gdb6
tGDB7           equ       07h         ; gdb7
tGDB8           equ       08h         ; gdb8
tGDB9           equ       09h         ; gdb9
tGDB0           equ       0ah         ; gdb0
;
;====================================
;       2nd half of string vars
;====================================
tStr1           equ       01h
tStr2           equ       02h
tStr3           equ       03h
tStr4           equ       04h
tStr5           equ       05h
tStr6           equ       06h
tStr7           equ       07h
tStr8           equ       08h
tStr9           equ       09h
tStr0           equ       0ah

;==================================================
;       2nd half of system output only variables
;==================================================
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
tLRSqr          equ       21h         ; r^2
tBRSqr          equ       22h         ; r^2
;
tError    equ     23h       ; token in case of error in entry (ans only)
tDone     equ     24h
;
;====================================
; system variable equates
;       vars with no restriction
;====================================
tuXscl       equ  1
tuYscl       equ  2
tXscl        equ  3
tYscl        equ  4

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
;       vars cannot be 0
;
tTblStep     equ  21h
tTStep       equ  22h
tThetaStep    equ  23h
tuTStep      equ  24h
tuThetStep     equ  25h
;
;       special limits on these
;
tDeltaX      equ  26h
tDeltaY      equ  27h
;
;       special limits on these
;
tXFact       equ  28h
tYFact       equ  29h
;
tTblInput       equ       2ah
;
;       no restrictions
tSimpFact       equ     2bh     ;simplification factor.
;
tPlotStep       equ       34h
tuPlotStep      equ       35h
;
tXres           equ       36h
tuXres          equ       37h
;

;======================================
;2nd byte of 2byte tokens
;======================================

tLCM            equ       01h         ; immediate unary
tGCD            equ       02h         ; immediate unary
tRandInt        equ       03h         ; immediate unary
tSubStrng       equ       04h         ; immediate unary
tStdDev         equ       05h         ; immediate unary
tVariance       equ       06h         ; immediate unary
tInStrng        equ       07h         ; immediate unary
tCoin           equ       08h         ; immediate unary
tDice           equ       09h         ; immediate unary
tRemainder      equ       0ah         ; immediate unary
lastImUnary      equ       0ah         ; last immediate unary token.
firstUnary      equ       0bh         ; first unary operator.
tCumSum         equ       0bh         ; unary
tExpr           equ       0ch         ; unary                        do autop
tLength         equ       0dh         ; unary                        do autop
tDeltaLst       equ       0eh         ; unary                        do autop
tRef            equ       0fh         ; unary                        no autop
autoP10         equ       10h
tRRef           equ       10h         ; unary                        no autop
newTok12         equ    12h

autoP11         equ       12h
tMatToLst       equ       12h         ; command pushed               do autop
tLstToMat       equ       13h         ; command pushed               do autop
tSetConst       equ       14h         ;                                 autop
tGraphStyle     equ       15h         ; command pushed                  autop
autoP12          equ       15h
tSetUpLst       equ       16h         ; command opt pushed           no autop
tExprOn         equ       1ah         ; mode command
tExprOff        equ       1bh         ; mode command
tClrSllLst      equ       1ch         ; command not pushed
tGetCalc        equ       1dh         ; command not pushed
tDelVar         equ       1eh         ; command not pushed
tEquToStrng     equ       1fh         ; command not pushed
tStrngToEqu     equ       20h         ; command not pushed
tDelLast        equ       21h         ; command not pushed
tSelect        equ       22h         ; command not pushed
tModBox         equ       23h         ; icon
tNormProb       equ       24h         ; icon
tPictOlot       equ       25h
tCirclePlot     equ       26h
tStemPlot       equ       27h
tBarPlot        equ       28h
tZFit           equ       29h         ; zoom fit
tDiag_On        equ       2ah         ; dianostic display on
tDiag_Off       equ       2bh         ; dianostic display off

;==========================================
;	data type equates
;===========================================
RealObj         equ       0
ListObj         equ       1
MatObj          equ       2
EquObj          equ       3
StrngObj        equ       4
ProgObj         equ       5
ProtProgObj     equ       6
PictObj         equ       7
GDBObj          equ       8
UnknownObj      equ       9
UnknownEquObj   equ       0ah
NewEquObj       equ       0bh
CplxObj         equ       0ch
ClistObj        equ       0dh
SlistObj        equ       0dh   ;string list
UndefObj        equ       0eh
WindowObj       equ       0fh
ZStoObj         equ       10h
TblrngObj       equ       11h
LCDObj          equ       12h
BackupObj       equ       13h
FracObj         equ       14h
FracRSObj       equ       14h           ;reduced   simple fraction.
FracRMObj       equ       15h           ;reduced   mixed  fraction.
FracUSObj       equ       16h           ;unreduced simple fraction.
FracUMObj       equ       17h           ;unreduced mixed fraction.
NameObj         equ       18h           ;name string (will fit in fplen).
AppObj          equ       19h           ;application, only used in menus/link
AppVarObj       equ       1ah           ;application variable
TmpConstObj     equ       1bh           ;temporary type of constant.

;======================================================================
;           i/o equates                                                          
;======================================================================
;
D0D1_bits	EQU  03h
D0LD1L		EQU  03h
D0LD1H		EQU  01h
D0HD1L		EQU  02h
D0HD1H		EQU  00h
bport		EQU  0			; 4-BIT LINK PORT (I/O) 
;
;======================================================================
;           device codes                                                       
;======================================================================
;
;
TI82DEV		EQU  82h
PC82DEV		EQU  02h
MAC82DEV	EQU  12h
;
TI83FDEV	EQU  73h
LINK83FDEV	EQU  23h
;
TI83DEV		EQU  83h
PC83DEV		EQU  03h
MAC83DEV	EQU  13h
;
TI85DEV		EQU  95h      ; DIFF THEN REAL 85 SO ME TALK
PC85DEV		EQU  05h
MAC85DEV	EQU  15h
;
TI73DEV		EQU  74h      ; Device x3 is always an 83
PC73DEV		EQU  07h
MAC73DEV	EQU  17h
;
TI83FDEV	EQU  73h
LINK73FDEV	EQU  23h
PC83FDEV	EQU  23h

;=========================================
;	system error codes
;=========================================

E_EDITF		EQU	7		;allow re-entering application
E_EDIT		EQU	1<<E_EDITF
E_Mask		EQU	7Fh

E_Overflow	EQU	1+E_EDIT
E_DivBy0	EQU	2+E_EDIT
E_SingularMat	EQU	3+E_EDIT
E_Domain	EQU	4+E_EDIT
E_Increment	EQU	5+E_EDIT
E_Break		EQU	6+E_EDIT
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
;
E_Scale         equ     31+E_EDIT
;
E_LnkErr        equ     32+E_EDIT
E_LnkMemErr     equ     33+E_EDIT
E_LnkTransErr   equ     34+E_EDIT
E_LnkDupErr     equ     35+E_EDIT
E_LnkMemFull    equ     36+E_EDIT
E_Unknown       equ     37+E_EDIT
E_IdNotFound    equ     38+E_EDIT
E_NoMode        equ     39+E_EDIT
E_Validation    equ     40+E_EDIT
E_Length        equ     41+E_EDIT
E_Application   equ     42+E_EDIT
E_AppErr1       equ     43+E_EDIT
E_AppErr2       equ     44+E_EDIT
E_ExpiredApp	equ	45+E_EDIT
E_BadAddr	equ	46+E_EDIT
HigErrNum	equ	46+E_EDIT

; obsolete error numbers34              ;first link error
E_LinkIOChkSum	equ	34
E_LinkIOTimeOut	equ	35
E_LinkIOBusy	equ	36
E_LinkIOVer	equ	37

;==============================================
;       equates to ram locations for stat vars
;==============================================
;
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
SLower  EQU     MedY3      + 2*FPLEN
SUpper  EQU     MedY3      + FPLEN

L3rd        equ              OP6+4
R3rd        equ              OP6+6
RidX        equ              OP6
LidX        equ              OP6+2

GetCurrentPage macro
    in		a, (memPageAPort)
    .endm

;============================================================
;  The following is a duplicate of all of the above equates
;  in lower case.  
;
;  It may be deleted if you choose case insensitive development
;  tools or choose to follow the above conventions.
;
;
;======================================================================
;

;SYMBOLIC_DEBUG_START
;=================================
;entry points
;=================================
_abso1o2cp		equ 40f0h
_abso1pabso2		equ 405ah
_acos		equ 40d2h
_acosrad		equ 40c6h
_adrlele		equ 4ab6h
_adrmrow		equ 4567h
_adrmele		equ 456ah
_alleq		equ 4792h
_allocfps		equ 4369h
_allocfps1		equ 436ch
_ansname		equ 4b67h
_apdsetup		equ 4c06h
_appgetcbl		equ 4b97h
_appgetcalc		equ 4b9ah
_appinit		equ 4048h
_appputaway equ 41b6h
_asin		equ 40d8h
_asinrad		equ 40cfh
_atanrad		equ 40c9h
_atan2rad		equ 40cch
_atan		equ 40d5h
_atan2		equ 40dbh

_binopexec		equ 459ah
_bufclear		equ 4840h

_canalphins		equ 4ab0h
_chkfindsym		equ 42d9h
_circcmd		equ 46f6h
_ckint		equ 423ah
_ckodd		equ 423dh
_ckop1c0		equ 422bh
_ckop1fp0		equ 422eh
_ckop1pos		equ 4252h
_ckop1real		equ 40e7h
_ckop2fp0		equ 4231h
_ckop2pos		equ 424fh
_ckop2real		equ 42cah
_ckposint		equ 4237h
_ckvalidnum		equ 426ah
_cleanall		equ 4927h
_clearrect		equ 4cfch
_clearrow		equ 4c8dh
_cline		equ 46cch
_clines		equ 46cfh
_closeeditbuf	equ 47e9h
_closeeditbufnor		equ 46a2h
_closeeditequ	equ 4867h
_closeprog		equ 490ch
_clrgraphref		equ 490fh
_clrlcd		equ 44aah
_clrlcdfull		equ 44a7h
_clrlp		equ 41e3h
_clrop1s		equ 4258h
_clrop2s		equ 4255h
_clrscrn		equ 44b0h
_clrscrnfull		equ 44adh
_clrtxtshd		equ 44b3h
_cmpsyms		equ 4921h
_convdim		equ 4b58h
_convdim00		equ 4b5bh
_convop1		equ 49abh
_cos		equ 40c0h
_cphlde		equ 4003h
_cpoint		equ 4d89h
_cpoints		equ 4717h
_cpop1op2		equ 40f3h
_cpop4op3		equ 40eah
_cpyo1tofps1		equ 4411h
_cpyo1tofps2		equ 4420h
_cpyo1tofps3		equ 442ch
_cpyo1tofps4		equ 443eh
_cpyo1tofps5		equ 4438h
_cpyo1tofps6		equ 4432h
_cpyo1tofps7		equ 4435h
_cpyo1tofpst		equ 43ffh
_cpyo2tofps1		equ 440eh
_cpyo2tofps2		equ 4417h
_cpyo2tofps3		equ 4429h
_cpyo2tofps4		equ 443bh
_cpyo2tofpst		equ 43f9h
_cpyo3tofps1		equ 4408h
_cpyo3tofps2		equ 441ah
_cpyo3tofpst		equ 43f6h
_cpyo5tofps1		equ 440bh
_cpyo5tofps3		equ 4426h
_cpyo6tofps2		equ 441dh
_cpyo6tofpst		equ 43fch
_cpystack		equ 43deh
_cpyto1fps1		equ 43e7h
_cpyto1fps10		equ 43a8h
_cpyto1fps11		equ 438dh
_cpyto1fps2		equ 43f0h
_cpyto1fps3		equ 43bdh
_cpyto1fps4		equ 43c3h
_cpyto1fps5		equ 4393h
_cpyto1fps6		equ 4399h
_cpyto1fps7		equ 439fh
_cpyto1fps8		equ 43a2h
_cpyto1fps9		equ 43abh
_cpyto1fpst		equ 43d8h
_cpyto2fps1		equ 43e4h
_cpyto2fps2		equ 43edh
_cpyto2fps3		equ 43b7h
_cpyto2fps4		equ 43aeh
_cpyto2fps5		equ 4390h
_cpyto2fps6		equ 4396h
_cpyto2fps7		equ 439ch
_cpyto2fps8		equ 43a5h
_cpyto2fpst		equ 43D5h
_cpyto3fps1		equ 43e1h
_cpyto3fps2		equ 43c6h
_cpyto3fpst		equ 43d2h
_cpyto4fpst		equ 43cfh
_cpyto5fpst		equ 43c9h
_cpyto6fpst		equ 43cch
_cpyto6fps3		equ 43b1h
_cpyto6fps2		equ 43b4h
_cpytofps1		equ 4414h
_cpytofps2		equ 4423h
_cpytofps3		equ 442fh
_cpytofpst		equ 4402h
_cpytostack		equ 4405h
_create0equ		equ 4303h
_createappvar		equ 4312h
_createequ		equ 4309h
_createpict		equ 430ch
_createprog		equ 430fh
_createreal		equ 42f4h
_createrlist	equ 42fah
_createstrng	equ 4300h
_cube  		equ 407bh
_cursoroff		equ 4522h
_cursoron		equ 4528h

_darkline		equ 46ffh
_darkpnt		equ 4714h
_datasize		equ 433ch
_datasizea		equ 4339h
_deallocfps		equ 4363h
_deallocfps1		equ 4366h
_deco1exp		equ 4261h
_dellistel		equ 4906h
_delmem		equ 4330h
_delres		equ 48fah
_delvar		equ 432ah
_disableapd		equ 4be8h
_disableapi		equ 4bdfh
_dispdone		equ 4519h
_dispeol		equ 450ah
_disphl		equ 446eh
_displayimage		equ 4d5ch
_dispop1a		equ 4afbh
_divhlby10		equ 4006h
_divhlbya		equ 4009h
_drawcirc2		equ 4aadh
_drawcmd		equ 47ddh
_drawrectborder		equ 4d2ch
_drawrectborderclear		equ 4d3bh
_dtor		equ 4075h

_editprog		equ 4909h
_enableapd		equ 4bebh
_enoughmem		equ 42e5h
_eop1notreal		equ 4273h
_equ_or_newequ		equ 42afh
_eraseeol		equ 44b9h
_eraserectborder		equ 4d35h
_errargument		equ 49ffh
_errbadguess		equ 4a2fh
_errbreak		equ 4a20h
_errd_op1_0		equ 42beh
_errd_op1_le_0		equ 42bbh
_errd_op1not_r		equ 42b5h
_errd_op1notpos		equ 42b2h
_errd_op1notposint		equ 42b8h
_errdatatype		equ 49f9h
_errdimmismatch		equ 4a02h
_errdimension		equ 4a05h
_errdivby0		equ 49e4h
_errdomain		equ 49e7h
_errincrement		equ 49eah
_errinvalid		equ 4a11h
_erriterations		equ 4a2ch
_errlinkxmit		equ 4a38h
_errmemory		equ 4a0bh
_errnon_real		equ 49edh
_errnonreal		equ 495ah
_errnotenoughmem		equ 4441h
_erroverflow		equ 49e1h
_errsignchange		equ 4a29h
_errstat		equ 4a23h
_errstatplot		equ 4a35h
_errsyntax		equ 49f0h
_errtoltoosmall		equ 4a32h
_errundefined		equ 4a08h
_etox		equ 40b4h
_exlp		equ 4228h
_exptohex		equ 4249h

_fillrect		equ 4d02h
_fillrectpattern		equ 4d38h
_find_parse_formula		equ 49b1h
_findalphadn		equ 491eh
_findalphaup		equ 491bh
_findsym		equ 42dch
_fiveexec		equ 45b5h
_fixtempcnt		equ 4912h
_formereal		equ 4885h
_formreal		equ 488bh
_fourexec		equ 45b2h
_fpadd		equ 4072h
_fpdiv		equ 4099h
_fpmult		equ 4084h
_fprecip		equ 4096h
_fpsquare		equ 4081h
_fpsub		equ 406fh
_frac		equ 4093h

_get_tok_strng		equ 44f8h
_getbasever		equ 4b8eh
_getcsc		equ 400fh
_getkey		equ 486dh
_getltoop1		equ 4573h
_gettoklen		equ 44f5h
_grbufclr         equ 4ad4h
_grbufcpy		equ 4789h
_grphcirc		equ 46f9h

_hltimes9		equ 40e4h
_horizcmd		equ 47c5h
_htimesl		equ 4270h

_ibounds		equ 4aa4h
_iboundsfull		equ 4d4dh
_iline		equ 4702h
_inclstsize		equ 4900h
_insertlist		equ 4903h
_insertmem		equ 42dfh
_int		equ 40a5h
_intgr		equ 405dh
_invcmd		equ 47e3h
_invertrect		equ 4cffh
_invop1s		equ 408dh
_invop1sc		equ 408ah
_invop2s		equ 4090h
_invsub		equ 4063h
_ioffset		equ 4aa7h
_ipoint		equ 4705h
_isa2bytetok	equ 4291h
_iseditempty		equ 483ah

_jerror		equ 4a4dh
_jerrorno		equ 4c03h
_jforcecmdnochar		equ 401eh

_keytostring		equ 452eh

_lcd_busy		equ 4051h
_lcd_driveron		equ 4873h
_ldhlind		equ 4000h
_linecmd		equ 47cbh
_lnx		equ 40abh
_load_sfont		equ 46b7h
_loadpattern		equ 4c51h
_logx		equ 40aeh

_max		equ 4057h
_memchk		equ 42cdh
_memclear		equ 4a47h
_memset		equ 4a4ah
_min		equ 4054h
_minus1		equ 406ch
_mov10b		equ 413eh
_mov18b		equ 46fch
_mov9b		equ 4141h
_mov7b		equ 414ah
_mov8b		equ 4147h
_mov9toop1		equ 416bh
_mov9op1op2		equ 416eh
_mov9toop2		equ 4171h
_movfrop1  equ 4174h

_newline		equ 4495h

_op1exop2		equ 4225h
_op1exop3		equ 421fh
_op1exop4		equ 4222h
_op1exop5		equ 4219h
_op1exop6		equ 421ch
_op1exptodec		equ 424ch
_op1set0		equ 41d1h
_op1set1		equ 4192h
_op1set2		equ 419eh
_op1set3		equ 4198h
_op1set4		equ 4195h
_op1toop2		equ 4111h
_op1toop3		equ 4105h
_op1toop4		equ 40f9h
_op1toop5		equ 4135h
_op1toop6		equ 4132h
_op2set0		equ 41ceh
_op2set1		equ 41aah
_op2set2		equ 41a1h
_op2set3		equ 418fh
_op2set4		equ 418ch
_op2set5		equ 4186h
_op2set60		equ 497bh
_op2set8		equ 4183h
_op2seta		equ 4189h
_op2toop1		equ 4138h
_op2toop3		equ 415fh
_op2toop4		equ 40fch
_op2toop5		equ 412ch
_op2toop6		equ 412fh
_op3set0		equ 41cbh
_op3set1		equ 4180h
_op3set2		equ 419bh
_op3toop1		equ 4120h
_op3toop2		equ 4102h
_op3toop4		equ 40f6h
_op3toop5		equ 4129h
_op4set0		equ 41c8h
_op4set1		equ 417dh
_op4toop1		equ 411ah
_op4toop2		equ 40ffh
_op4toop3		equ 4162h
_op4toop5		equ 4126h
_op4toop6		equ 4168h
_op5set0		equ 41c5h
_op5toop1		equ 411dh
_op5toop2		equ 4108h
_op5toop3		equ 4165h
_op5toop4		equ 410eh
_op5toop6		equ 410bh
_op6toop1		equ 4117h
_op6toop2		equ 4114h
_op6toop5		equ 4123h

_parseinp		equ 4966h
_pdspgrph		equ 47c2h
_pixeltest		equ 47d4h
_plus1		equ 4069h
_pointcmd		equ 47d1h
_pointon		equ 4a56h
_popop1		equ 4345h
_popop3		equ 4342h
_popop5		equ 433fh
_popreal		equ 435ah
_poprealo1		equ 4357h
_poprealo2		equ 4354h
_poprealo3		equ 4351h
_poprealo4		equ 434eh
_poprealo5		equ 434bh
_poprealo6		equ 4348h
_posno0int		equ 4234h
_ptor		equ 40deh
_pushop1		equ 438ah
_pushop3		equ 4387h
_pushop5		equ 4384h
_pushreal		equ 4381h
_pushrealo1		equ 437eh
_pushrealo2		equ 437bh
_pushrealo3		equ 4378h
_pushrealo4		equ 4375h
_pushrealo5		equ 4372h
_pushrealo6		equ 436fh
_putaway		equ 4039h
_putc		equ 446bh
_putmap		equ 4468h
_putps		equ 4477h
_putpsb		equ 4474h
_puts		equ 4471h
_puttokstring		equ 485eh
_puttol		equ 4582h

_randinit		equ 4c21h
_random		equ 4b9dh
_rcl_statvar		equ 42c7h
_rclans		equ 4996h
_rclgdb2		equ 46f3h
_rclsystok		equ 49a2h
_rclvarsym		equ 499fh
_rclx		equ 499ch
_rcly		equ 4999h
_redimmat		equ 48fdh
_regraph		equ 47aah
_releasebuffer		equ 46a5h
_reloadappentryvecs		equ 4a50h
_restoredisp		equ 478ch
_rndguard		equ 409fh
_rnfx		equ 40a2h
_round		equ 40a8h
_rtod		equ 4078h
_rtop		equ 40e1h
_runindicoff		equ 44d7h
_runindicon		equ 44d4h

_savedisp		equ 4ba3h
_sendvarcmd		equ 48f1h
_setfuncm		equ 4762h
_setxxop1		equ 46c0h
_setxxop2		equ 46c3h
_setxxxxop2		equ 46c6h
_sfont_len		equ 46bah
_shracc		equ 41E6h
_sin		equ 40bdh
_sincosrad		equ 40bah
_sqroot		equ 409ch
_srchvlstdn		equ 4c36h
_srchvlstup		equ 4c33h
_sstringlength		equ 4c54h
_stmatel		equ 49a5h
_stoans		equ 498ah
_stogdb2		equ 46f0h
_stoother		equ 4993h
_storand		equ 4e5eh
_stosystok		equ 4987h
_stox		equ 4990h
_stoy		equ 498dh
_strcopy		equ 444ah
_strlength		equ 4a5ch

_tan		equ 40c3h
_tenx		equ 40b7h
_threeexec		equ 45ach
_times2		equ 4066h
_timespt5		equ 407eh
_tofrac		equ 4594h
_trunc		equ 4060h

_uclines		equ 46c9h
_unlinecmd		equ 47ceh
_unopexec		equ 45a9h

_vertcmd		equ 47c8h
_vputmap		equ 44c5h
_vputs		equ 44c8h
_vputsn		equ 44cbh
_vtowhlde		equ 471dh

_xftoi		equ 4726h
_xitof		equ 4720h
_xname		equ 4279h
_xrooty		equ 46d2h

_yftoi		equ 4723h
_yname		equ 427ch
_ytox		equ 46d5h

_zero16d		equ 41b9h
_zeroop		equ 41e0h
_zeroop1		equ 41d7h
_zeroop2		equ 41dah
_zeroop3		equ 41ddh
_zmdecml		equ 476eh
_zmfit		equ 477ah
_zmint		equ 476bh
_zmprev		equ 4771h
_zmsquare		equ 477dh
_zmstats		equ 46d8h
_zmtrig		equ 4780h
_zmusr		equ 4774h
_zoodefault		equ 4786h
;======================================================================

;           system-defined ram variable address equates

;======================================================================

;




ramstart             equ  8000h
appdata              equ  8000h
ramcode              equ  8100h
ramcodeend           equ  822fh
baseappbrtab         equ  8230h
boottemp             equ  8241h
appsearchpage        equ  8293h
tempswaparea         equ  8295h
appid                equ  837dh
ramreturndata        equ  8529h
appinfo              equ  8998h
appbank_jump         equ  85c9h
apppage              equ  85cbh
kbdscancode          equ  83d8h
kbdkey               equ  83ddh
kbdgetky             equ  83deh
keyextend            equ  83dfh
contrast             equ  83e0h
apdsubtimer          equ  84e0h
apdtimer             equ  84e1h
curtime              equ  83e1h
currow               equ  83e2h
curcol               equ  83e3h
curoffset            equ  83e4h
curunder             equ  83e5h
cury                 equ  8498h
curtype              equ  8499h
curxrow              equ  849ah
prevddata            equ  849bh
lfont_record         equ  84a3h
sfont_record         equ  84abh
tokvarptr            equ  85a1h
toklen               equ  85a3h
indicmem             equ  84d6h
indiccounter         equ  84deh
indicbusy            equ  84dfh
op1                  equ  842bh
op1m                 equ  842dh
op2                  equ  8436h
op2m                 equ  8485h
op2ext               equ  843fh
op3                  equ  8441h
op3m                 equ  8443h
op4                  equ  844ch
op4m                 equ  844eh
op5                  equ  8457h
op5m                 equ  8459h
op6                  equ  8462h
op6m                 equ  8464h
op6ext               equ  846bh
progtoedit           equ  8472h
namebuff             equ  847ah
equ_edit_save        equ  8485h
imathptr1            equ  8486h
imathptr2            equ  8488h
imathptr3            equ  848ah
imathptr4            equ  848ch
imathptr5            equ  848eh
chkdelptr1           equ  8490h
chkdelptr2           equ  8492h
chkdelptr3           equ  9baah
chkdelptr4           equ  9bach
insdelptr            equ  8494h
updownptr            equ  8496h
foutdat              equ  85a5h
asm_data_ptr1        equ  85afh
asm_data_ptr2        equ  85b1h
asm_sym_ptr1         equ  85b3h
asm_sym_ptr2         equ  85b5h
asm_ind_call         equ  85b7h
textshadow           equ  8913h
textshadcur          equ  8993h
textshadtop          equ  8995h
textshadalph         equ  8996h
textshadins          equ  8997h
cxmain               equ  84e2h
cxpputaway           equ  84e4h
cxputaway            equ  84e6h
cxredisp             equ  84e8h
cxerrorep            equ  84eah
cxsizewind           equ  84ech
cxpage               equ  84eeh
cxcurapp             equ  84efh
cxprev               equ  84f0h
monqh                equ  850eh
monqt                equ  850fh
monqueue             equ  8510h
onsp                 equ  8520h
promptrow            equ  89a2h
promptcol            equ  89a3h
promptins            equ  89a4h
promptshift          equ  89a5h
promptret            equ  89a6h
promptvalid          equ  89a8h
prompttop            equ  89aah
promptcursor         equ  89ach
prompttail           equ  89aeh
promptbtm            equ  89b0h
vartype              equ  89b2h
varcurrent           equ  89b3h
varclass             equ  89bbh
menuactive           equ  84b3h
menuappdepth         equ  84b4h
menucurrent          equ  84b5h
progcurrent          equ  84c1h
usermenusa           equ  89e1h
ioprompt             equ  8a41h
dimagewidth          equ  8a42h
rectfillpheight		 equ  8a42h
rectfillpwidth		 equ  8a43h
rectfillpattern		 equ  8a44h
ioflag               equ  83e6h
sndrecstate          equ  83e8h
ioerrstate           equ  83e9h
header               equ  83eah
iodata               equ  83f3h
bakheader            equ  83ffh
pencol               equ  89dfh
penrow               equ  89e0h
rclqueue             equ  84d2h
rclqueueend          equ  84d4h
errno                equ  854ah
errsp                equ  854bh
erroffset            equ  854dh
savesscreen          equ  8a52h
bstcounter           equ  8559h
flags                equ  855bh
statvars             equ  8d58h
curgstyle            equ  8e94h
curgy                equ  8e95h
curgx                equ  8e96h
curgy2               equ  8e97h
curgx2               equ  8e98h
freesavey            equ  8e99h
freesavex            equ  8e9ah
xoffset              equ  8ec8h
yoffset              equ  8ec9h
lcdtallp             equ  8ecah
pixwidep             equ  8ecbh
pixwide_m_1          equ  8ecch
pixwide_m_2          equ  8ecdh
lastentryptr         equ  8eceh
lastentrystk         equ  8ed0h
numlastentries       equ  90d0h
currlastentry        equ  90d1h
curinc               equ  92deh
uxmin                equ  92f5h
uxmax                equ  92feh
uxscl                equ  9307h
uymin                equ  9310h
uymax                equ  9319h
uyscl                equ  9322h
uthetmin             equ  932bh
uthetmax             equ  9334h
uthetstep            equ  933dh
utmin                equ  9346h
utmax                equ  934fh
utstep               equ  9358h
uplotstart           equ  9361h
unmax                equ  936ah
uu0                  equ  9373h
uv0                  equ  937ch
unmin                equ  9385h
uu02                 equ  938eh
uv02                 equ  9397h
uw0                  equ  93a0h
uplotstep            equ  93a9h
uxres                equ  93b2h
uw02                 equ  93bbh
xmin                 equ  93c7h
xmax                 equ  93d0h
xscl                 equ  93d9h
ymin                 equ  93e2h
ymax                 equ  93ebh
yscl                 equ  93f4h
thetamin             equ  93fdh
thetamax             equ  9406h
thetastep            equ  940fh
tminpar              equ  9418h
tmaxpar              equ  9421h
tstep                equ  942ah
plotstart            equ  9433h
nmax                 equ  943ch
u0                   equ  9445h
v0                   equ  944eh
nmin                 equ  9457h
u02                  equ  9460h
v02                  equ  9469h
w0                   equ  9472h
plotstep             equ  947bh
xreso                equ  9484h
w02                  equ  948dh
smalleditram         equ  949fh
xfact                equ  950bh
yfact                equ  9514h
xres_int             equ  951dh
deltax               equ  951eh
deltay               equ  9527h
shortx               equ  9530h
shorty               equ  9539h
lower                equ  9542h
upper                equ  954bh
xoutsym              equ  9558h
xoutdat              equ  955ah
youtsym              equ  955ch
youtdat              equ  955eh
inputsym             equ  9560h
inputdat             equ  9562h
prevdata             equ  9564h
tblmin               equ  9676h
tblstep              equ  967fh
p1type		     equ  968ch
savx1list	     equ  968dh
savy1list	     equ  9692h
savf1list	     equ  9697h
p1frqonoff	     equ  96abh
p2type		     equ  96aeh
savx2list	     equ  96afh
savy2list	     equ  96b4h
savf2list	     equ  96b9h
p2frqonoff	     equ  96cdh
p3type		     equ  96d0h	
savx3list	     equ  96d1h
savy3list	     equ  96d6h
savf3list	     equ  96d8h
p3frqonoff	     equ  96efh
plotsscreen          equ  9737h
seed1                equ  9a37h
seed2                equ  9a40h
cmdcursor            equ  9a7eh
edittop              equ  9a82h
editcursor           equ  9a84h
edittail             equ  9a86h
editbtm              equ  9a88h
editsym              equ  9a94h
editdat              equ  9a96h
wintop               equ  9b33h
winbtm               equ  9b34h
winleftedge          equ  9b35h
winleft              equ  9b36h
winabove             equ  9b38h
winrow               equ  9b3ah
wincol               equ  9b3ch
fmtdigits            equ  9b3eh
fmtstring            equ  9b3fh
fmtconv              equ  9b80h
fmtleft              equ  9b92h
fmtindex             equ  9b94h
fmtmatsym            equ  9b96h
fmtmatmem            equ  9b98h
eqs                  equ  9b9ah
tsymptr1             equ  9ba6h
tsymptr2             equ  9ba8h
chkdelptr3           equ  9baah
chkdelptr4           equ  9bach
tempmem              equ  9baeh
fpbase               equ  9bb0h
fps                  equ  9bb2h
opbase               equ  9bb4h
ops                  equ  9bb6h
ptempcnt             equ  9bb8h
cleantmp             equ  9bbah
ptemp                equ  9bbch
progptr              equ  9bbeh
newdataptr           equ  9bc0h
apperr1              equ  9bc2h
apperr2              equ  9bcfh
apprawkeyhandle      equ  85b8h
appbackupscreen      equ  8613h
customheight         equ  8612h
usermem              equ  9c37h
symtable             equ  0fe66h


;====================================
; equates for fraction entry routines
;====================================
fieldmax    equ    op5
fieldlen    equ    fieldmax+1
fieldcol    equ    fieldlen+1     ; pencol of start
fieldrow    equ    fieldcol+1     ; penrow of start  3 rows above char
fieldinput  equ    fieldrow+1     ; 9 bytes,
wholemax    equ    tempswaparea
wholelen    equ    wholemax+1
wholecol    equ    wholelen+1     ; pencol of start
wholerow    equ    wholecol+1     ; penrow of start  3 rows above char
wholeinput  equ    wholerow+1     ; 9 bytes,
;
numermax    equ    wholeinput+9
numerlen    equ    numermax+1
numercol    equ    numerlen+1     ; pencol of start
numerrow    equ    numercol+1     ; penrow of start
numerinput  equ    numerrow+1     ; 9 bytes,
;
denommax    equ    numerinput+9
denomlen    equ    denommax+1
denomcol    equ    denomlen+1     ; pencol of start
denomrow    equ    denomcol+1     ; penrow of start
denominput  equ    denomrow+1     ; 9 bytes,
;
fraclinelen equ   denominput+9
fraclinecol equ   fraclinelen+1
;
fracrightedge  equ fraclinecol+1
;
curposition equ   fracrightedge+1
;
numerspaces equ   curposition+1
denomspaces equ   numerspaces+1
havefrac    equ   3
haveunit    equ   4
inccurpos   equ   5

;SYMBOLIC_DEBUG_END

;====================================================================
; macro to call base code from an app
;====================================================================
rbr_call	equ	28h		; needed for apps
brt_jump0	equ	50h
;
b_call macro   label
	     rst     rbr_call
	     dw      _&label
       .endm
;
b_jump macro   label
	     call    brt_jump0	     ; fixed location on page 0
	     dw      _&label
       .endm

;======================================================================
;           system variable equates
;======================================================================
;       entry points : rclsystok, stosystok
;
;       vars with no restriction
;
xsclt       equ  3           ; 'xscl'           4
ysclt       equ  4           ; 'yscl'           4
;
;       vars rounded to 10 digits
;
xmint       equ  0ah         ; 'xmin'           4
xmaxt       equ  0bh         ; 'xmax'           4
ymint       equ  0ch         ; 'ymin'           4
ymaxt       equ  0dh         ; 'ymax'           4
tblmint     equ  1ah         ; 'tblmin'
;
;       rouned to 10 digits, and positve integers
;
plotstartt   equ  1bh         ; 'nstart'         5
uplotstartt  equ  1ch         ; 'znstart'        6

;       vars cannot be 0
;
tblstept    equ  21h         ; 'tblstep'
tstept      equ  22h         ; 'tstep'          5
thetstept   equ  23h         ; ltheta,'step'    5
utstept     equ  24h         ; 'ztstep'         6
uthstept    equ  25h         ; 'z',ltheta,'step' 6
;
;       special limits on these
;
deltaxt     equ  26h         ; lcapdelta,'x'    2
deltayt     equ  27h         ; lcapdelta,'y'    2
;
;       special limits on these
;
xfactt      equ  28h         ; 'xfact'          5
yfactt      equ  29h         ; 'yfact'          5
;
;       sys var list
;
tblinputt       equ       2ah
;
;
;
plotstept       equ       34h
uplotstept      equ       35h
;
xrest           equ       36h
uxrest          equ       37h
;
; special token used by input/prompt for storing value to variable

inputt      equ  40h         ; set up for store for input/prompt

; special token used by graph math intersection routine to indicate
; list data

isectt      equ  41h         ; list data follows this
;
;========== ram area sizes
stacksize	equ	400
lastentrystklen equ     128

;========== automatic power down time out value (<= 7 minutes)
apdtimeout	equ	5		;automatic power down time (minutes)

;========== interrupt equate
iall	equ 1011b

;========== i/o ports
intrptenport	equ 3 ; interrupt enable (in/out) 
mempageaport	equ	6		; memory page a	(i/o)
lcdinstport     equ     10h
lcddataport     equ     11h 


;========== common subroutine rst numbers
rop1toop2	equ	08h
rfindsym	equ	10h
rpushrealo1     equ     18h
rmov9toop1	equ	20h
rbr_call	equ	28h
rfpadd		equ	30h

;========== fixed location entry points on page 0

brt_jump0	equ	50h
app_push_errorh equ	59h
app_pop_errorh	equ	5ch

apponerr   macro   handaddr
	ld	hl,handaddr		;hl -> stack handler address
	call	app_push_errorh 	;establish error handler context
	.endm


appofferr  macro
	call	app_pop_errorh		;remove error handler context
	.endm

;========== run indicators

busypause	equ	10101010b
busynormal	equ	11110000b

;======================================================================
;           system and state flags
;======================================================================
;

trigflags	equ  0		;trigonometry mode settings
trigdeg 	equ    2	; 1=degrees, 0=radians

kbdflags	equ  0		;keyboard scan
kbdscr		equ    3	; 1=scan code ready
kbdkeypress	equ    4	; 1=key has been pressed

doneflags	equ  0		;display "done"
doneprgm	equ    5	; 1=display "done" after prgm

iodelflag	equ  0
indelete	equ    0	;1 = delete screen 

;----------------------------------------------------------------------
editflags	equ  1
editopen	equ    2	; 1=edit buffer is open

monflags	equ  1		;monitor flags
monabandon	equ    4	; 1=don't start any long process
				; in put away (#715)
;----------------------------------------------------------------------
plotflags	equ  2		;plot generation flags
plottrace	equ  0
plotloc 	equ    1	; 0=bkup & display, 1=display only
plotdisp	equ    2	; 1=plot is in display, 0=text in display


grfmodeflags	equ  2		;graph mode settings
grffuncm	equ    4	; 1=function graph
grfpolarm	equ    5	; 1=polar graph
grfparamm	equ    6	; 1=parametric graph
grfrecurm       equ    7        ; 1=recursion graph

graphflags	equ  3
graphprog	equ  1     ; 1=graph in progress
graphdraw	equ    0	; 0=graph is valid, 1=redraw graph

grfdbflags	equ  4
grfdot		equ    0	; 0=line, 1=dot
grfsimul	equ    1	; 0=sequential, 1=simultaneous
grfgrid 	equ    2	; 0=no grid, 1=grid
grfpolar	equ    3	; 0=rectangular, 1=polar coordinates
grfnocoord	equ    4	; 0=display coordinates, 1=off
grfnoaxis	equ    5	; 0=axis, 1=no axis
grflabel	equ    6	; 0=off, 1=axis label

textflags	equ  5		;text output flags
texterasebelow	equ    1	; 1=erase line below small char
textscrolled	equ    2	; 1=screen scrolled
textinverse	equ    3	; 1=display inverse bit-map
textinsmode	equ    4	; 0=overstrike, 1=insert mode

parsflag2	equ  7		;parser flags
numop1		equ    0	; 1=result in op1, 0=no result

newdispf        equ  8		;derivative mode flags
preclrformode   equ    0	; 1=help blink on mode screen

apdflags	equ  8		;automatic power-down
apdable 	equ    2	; 1=apd enabled
apdrunning	equ    3	; 1=apd clock running


web_err_mask    equ  60h

onflags 	equ  9		;on key flags
onrunning	equ	 3
oninterrupt	equ    4	; 1=on key interrupt request

statflags	equ  9		;statistics flags
statsvalid	equ    6	; 1=stats are valid

fmtflags	equ  10		;numeric format flags
fmtexponent	equ    0	; 1=show exponent, 0=no exponent
fmteng		equ    1	; 1=engineering notion, 0=scientific
fmthex		equ    2	; 1=hexadecimal
fmtoct		equ    3	; 1=octal
fmtbin		equ    4	; 1=binary
;
nummode         equ  10
fmtreal         equ    5
fmtrect         equ    6
fmtpolar        equ    7

realmode        equ    5
rectmode        equ    6
polarmode       equ    7
;					;   if hex and oct both = 1
;					;   then bin=0 means >frac
;					;	 bin=1 means >dms
fmtbasemask     equ  00011100b		; mask to base flags
fmtbaseshift    equ  2			; offset to base flags
;
;       check if these are used by numform,
;
;               equ  6
;               equ  7

fmtoverride	equ  11		;copy of fmtflags with conversion override

fmteditflags	equ  12		;numeric editing flags
fmtedit 	equ    0	; 1=format number for editing

curflags	equ  12		;cursor
curable 	equ    2	; 1=cursor flash is enabled
curon		equ    3	; 1=cursor is showing
curlock 	equ    4	; 1=cursor is locked off

appflags	equ  13		;application flags
appwantintrpt	equ    0		; 1=want on key interrupts
apptextsave	equ    1		; 1=save characters in textshadow
appautoscroll	equ    2		; 1=auto-scroll text on last line
appmenus	equ    3		; 1=process keys that bring up menus
					; 0=check lock menu flag
applockmenus	equ    4		; 1=ignore menu keys
					; 0=switch to home screen and bring up menu
appcurgraphic	equ    5		; 1=graphic cursor
appcurword	equ    6		; 1=text cursor covers entire word
appexit 	equ    7		; 1=application handles [exit] key itself

appwantintrptf	equ	1<<appwantintrpt
apptextsavef	equ	1<<apptextsave
appautoscrollf	equ	1<<appautoscroll
appmenusf	equ	1<<appmenus
applockmenusf	equ	1<<applockmenus
appcurgraphicf	equ	1<<appcurgraphic
appcurwordf	equ	1<<appcurword
appexitf	equ	1<<appexit


seqflags        equ  15              ; sequential graph flags
webmode         equ    0             ; 0 = normal seq mode, 1 = web mode
webvert         equ    1             ;
sequv           equ    2             ; u vs v
seqvw           equ    3             ; v vs w
sequw           equ    4             ; u vs w


promptflags	equ  17		;prompt line flags
promptedit	equ    0	; 1=editing in prompt buffer

indicflags	equ  18		;indicator flags
indicrun	equ    0	; 1=run indicator on
indicinuse	equ    1	; indicator save area in use=1, free=0
indiconly	equ    2	; interrupt handler only checks run indicator

shiftflags	equ  18		;[2nd] and [alpha] flags
shift2nd	equ    3	; 1=[2nd] has been pressed
shiftalpha	equ    4	; 1=[alpha] has been pressed
shiftlwralph	equ    5	; 1=lower case, 0=upper case
shiftalock	equ    6	; 1=alpha lock has been pressed
shiftkeepalph	equ    7	; 1=cannot cancel alpha shift


tblflags        equ  19		;table flags.
autofill        equ    4	; 1=prompt, 0=fillauto
autocalc        equ    5	; 1=prompt, 0=calcauto
retable         equ    6	; 0=table is ok, 1=must recompute table.

fracmode        equ      20             ;FRAC FLAGS
abcf            equ       0             ; a b/c if 1, b/c if 0.
autosimpf       equ       1             ; autosimp if 0, mansimp if 1.
mansimpf        equ       1             ; autosimp if 0, mansimp if 1.

constmode       equ     20
hide            equ       2            ;0 if show, 1 if hide.
multi           equ       3            ;0 if single, 1 if multiple.

sgrflags	equ  20
write_on_graph  equ    4	; 1 = text or equ writing to graph screen
g_style_active  equ    5	; 1 = graph styles are enabled, use them
cmp_mod_box     equ    6	; 1 = doing mod box plot computation
textwrite       equ    7
;

newindicflags   equ  21
extraindic      equ    0
saindic         equ    1

smartflags      equ  23
;
;----------------------------------------------------------------------
; note: fix these equates if smartflags are moved                            
;----------------------------------------------------------------------
;
smarter_mask    equ  3
smarter_test    equ  1
smartgraph      equ  0
smartgraph_inv  equ  1                                                 

;----------------------------------------------------------------------
;           available for asm programming
;----------------------------------------------------------------------

asm_flag1       equ  33          ; asm coding
asm_flag2       equ  34          ; asm coding
asm_flag3       equ  35          ; asm coding

;----------------------------------------------------------------------

fracFlags	equ	  36		; fraction display/edit
fracStopEOS	equ	   0
fracInside	equ	   1
fracDontDraw	equ	   2
fracEditMode	equ	   3
fracInNum	equ	   4
fracScrScrolled equ	   5
fracFlag1	equ	   6
fracClearEOS	equ	   7	   ; tell drawExpr code to clear to end of screen when done
;
fontFlags	equ	   37
fracDrawNum	equ	   0
fracDrawDenom	equ	   1
fracDrawLFont	equ	   2
fracTallLFont	equ	   3
fracLastChar	equ	   4
fracCursor	equ	   5
DrawLfont       equ       6
CustomFont      equ       7
;
;flags for fraction math routines:
FracFlags2      equ     38
DtoF10F         equ      0      ;10digit flag for Dec ->Frac
SimpKeyF        equ      1      ;Used for Simp K.
RatD1K          equ      1      ;also used to show denominator=1000
FinalType       equ      2      ;1=final type is improper 0=type is simple
FinalFrac       equ      3      ;set if final fraction result expected.
ManDtoF         equ      4      ;manual simp=1, Dec->frac bit.
ManForceS       equ      5      ;manual simp=1, Abc=0; force simplify anyhow.
ManAdjW         equ      6      ;manual simp=1, Abc=0; Adjust whole flag.
ManBin          equ      7      ;manual simp=1, Binop set.


;flags for fraction math routines:
FracFlags3      equ     39
FNormOver       equ      0      ;Fraction Normalization Override if set.
DivSmallNum     equ      1      ;set if Frac Numerator < denominator.
IntDivDisp      equ      2      ;integer divide display flag.
StrictInt       equ      3      ;strict integer check by intdiv if set.
yxRatF          equ      4      ;y^x rational approximation flag.
nid             equ      5      ;1 if non integer decimal
SimpDisp        equ      6      ;1 if need to display frac simplify factor.
HadTrueMixed    equ      7      ;at least one true fraction in an expression.
;(HadTrueMixed is needed by >Ab/c<>d/e so that if any mixed fraction, then
; if convert, will stay b/c if b/c mode & not convert back to mixed.)

;
getsendflg	equ  47
comfailed	equ  3	      ; 1 = get/send communication failed
;
;
apiflg3		equ  52 
;
apiflg4		equ  53
fullscrndraw	equ    2	; draw into last row/col of screen

xapflag0        equ  43      ; external app flags
xapflag1        equ  44
xapflag2        equ  45
xapflag3        equ  46

plotflag3	equ  47  
bufferonly	equ    6
usefastcirc	equ    4

;
vartypemask	equ  1fh             
vargraphref	equ    6

;======================================================================
;           character font equates  
;======================================================================
;
lrecurn         equ     001h
lrecuru 	equ	002h
lrecurv 	equ	003h
lrecurw		equ	004h
lconvert	equ	005h
lsqup		equ	006h
lsqdown		equ	007h
lintegral	equ	008h
lcross		equ	009h
lboxicon 	equ     00ah
lcrossicon 	equ     00bh
ldoticon 	equ     00ch
lsubt   	equ     00dh		;small capital t for parametric mode.
lcuber  	equ     00eh		;slightly different 3 for cubed root.
lhexf		equ	00fh
lroot		equ	010h
linverse	equ	011h
lsquare		equ	012h
langle		equ	013h
ldegree		equ	014h
lradian		equ	015h
ltranspose	equ	016h
lle		equ	017h
lne		equ	018h
lge		equ	019h
lneg		equ	01ah
lexponent	equ	01bh
lstore		equ	01ch
lten		equ	01dh
luparrow	equ	01eh
ldownarrow	equ	01fh
lspace		equ	020h
lexclam		equ	021h
lquote		equ	022h
lpound		equ	023h
lfourth		equ     024h
lpercent	equ	025h
lampersand	equ	026h
lapostrophe	equ	027h
llparen		equ	028h
lrparen		equ	029h
lasterisk	equ	02ah
lplussign	equ	02bh
lcomma		equ	02ch
ldash		equ	02dh
lperiod		equ	02eh
lslash		equ	02fh
l0		equ	030h
l1		equ	031h
l2		equ	032h
l3		equ	033h
l4		equ	034h
l5		equ	035h
l6		equ	036h
l7		equ	037h
l8		equ	038h
l9		equ	039h
lcolon		equ	03ah
lsemicolon	equ	03bh
llt		equ	03ch
leq		equ	03dh
lgt		equ	03eh
lquestion	equ	03fh
latsign		equ	040h
lcapa	equ	041h
lcapb	equ	042h
lcapc	equ	043h
lcapd	equ	044h
lcape	equ	045h
lcapf	equ	046h
lcapg	equ	047h
lcaph	equ	048h
lcapi	equ	049h
lcapj	equ	04ah
lcapk	equ	04bh
lcapl	equ	04ch
lcapm	equ	04dh
lcapn	equ	04eh
lcapo	equ	04fh
lcapp	equ	050h
lcapq	equ	051h
lcapr	equ	052h
lcaps	equ	053h
lcapt	equ	054h
lcapu	equ	055h
lcapv	equ	056h
lcapw	equ	057h
lcapx	equ	058h
lcapy	equ	059h
lcapz	equ	05ah
ltheta  equ     05bh
lbackslash	equ	05ch
lrbrack		equ	05dh
lcaret		equ	05eh
lunderscore	equ	05fh
lbackquote	equ	060h
la	equ	061h
lb	equ	062h
lc	equ	063h
ld	equ	064h
le	equ	065h
lf	equ	066h
lg	equ	067h
lh	equ	068h
li	equ	069h
lj	equ	06ah
lk	equ	06bh
ll	equ	06ch
lm	equ	06dh
ln	equ	06eh
lo	equ	06fh
lp	equ	070h
lq	equ	071h
lr	equ	072h
ls	equ	073h
lt	equ	074h
lu	equ	075h
lv	equ	076h
lw	equ	077h
lx	equ	078h
ly	equ	079h
lz	equ	07ah
llbrace	equ	07bh
lbar	equ	07ch
lrbrace	equ	07dh
ltilde	equ	07eh
linveq	equ	07fh
lsub0	equ	080h
lsub1	equ	081h
lsub2	equ	082h
lsub3	equ	083h
lsub4	equ	084h
lsub5	equ	085h
lsub6	equ	086h
lsub7	equ	087h
lsub8	equ	088h
lsub9	equ	089h
lcapaacute	equ	08ah
lcapagrave	equ	08bh
lcapacaret	equ	08ch
lcapadier	equ	08dh
laacute		equ	08eh
lagrave		equ	08fh
lacaret		equ	090h
ladier		equ	091h
lcapeacute	equ	092h
lcapegrave	equ	093h
lcapecaret	equ	094h
lcapedier	equ	095h
leacute		equ	096h
legrave		equ	097h
lecaret		equ	098h
ledier		equ	099h
lcapiacute	equ	09ah
lcapigrave	equ	09bh
lcapicaret	equ	09ch
lcapidier	equ	09dh
liacute		equ	09eh
ligrave		equ	09fh
licaret		equ	0a0h
lidier		equ	0a1h
lcapoacute	equ	0a2h
lcapograve	equ	0a3h
lcapocaret	equ	0a4h
lcapodier	equ	0a5h
loacute		equ	0a6h
lograve		equ	0a7h
locaret		equ	0a8h
lodier		equ	0a9h
lcapuacute	equ	0aah
lcapugrave	equ	0abh
lcapucaret	equ	0ach
lcapudier	equ	0adh
luacute		equ	0aeh
lugrave		equ	0afh
lucaret		equ	0b0h
ludier		equ	0b1h
lcapcced	equ	0b2h
lcced		equ	0b3h
lcapntilde	equ	0b4h
lntilde		equ	0b5h
laccent		equ	0b6h
lgrave		equ	0b7h
ldieresis	equ	0b8h
lquesdown	equ	0b9h
lexclamdown	equ	0bah
lalpha		equ	0bbh
lbeta		equ	0bch
lgamma		equ	0bdh
lcapdelta	equ	0beh
ldelta		equ	0bfh
lepsilon	equ	0c0h
llbrack 	equ     0c1h
llambda		equ	0c2h
lmu		equ	0c3h
lpi		equ	0c4h
lrho		equ	0c5h
lcapsigma	equ	0c6h
lsigma		equ	0c7h
ltau		equ	0c8h
lphi		equ	0c9h
lcapomega	equ	0cah
lxmean		equ	0cbh
lymean		equ	0cch
lsupx		equ	0cdh
lellipsis	equ	0ceh
lleft		equ	0cfh
lblock		equ	0d0h
lper		equ	0d1h
lhyphen		equ	0d2h
larea		equ	0d3h
ltemp		equ	0d4h
lcube		equ	0d5h
lenter		equ	0d6h
limagi		equ	0d7h
lphat		equ	0d8h
lchi		equ	0d9h
lstatf		equ     0dah
llne		equ	0dbh
llistl		equ	0dch
lfinann 	equ	0ddh
l2_r_paren 	equ	0deh
lblockarrow     equ     0dfh
lcuro   	equ     0e0h
lcuro2  	equ     0e1h
lcurocapa       equ     0e2h
lcuroa  	equ     0e3h
lcuri   	equ     0e4h
lcuri2  	equ     0e5h
lcuricapa       equ     0e6h
lcuria  	equ     0e7h
lgline          equ     0e8h    ; = 0
lgthick         equ     0e9h    ; = 1
lgabove         equ     0eah    ; = 2
lgbelow         equ     0ebh    ; = 3
lgpath          equ     0ech    ; = 4
lganimate       equ     0edh    ; = 5
lgdot           equ     0eeh    ; = 6
lupblk          equ     0efh    ;up arrow and block in solver
ldnblk          equ     0f0h    ;down arrow and block in solver
lcurfull        equ     0f1h    ;note: must be last char (putmap checks)

;========= floating point

fplen           equ     9               ;length of a floating-point number

;========= constant table equates

drconst 	equ	0
pi2const	equ	1
pi4const	equ	2
logeconst	equ	3
piconst 	equ	4
dtorconst	equ	5
ln10		equ	6

;========== text screen
textrows	equ	8
textcols        equ     16

;========== link i/o
stsndrec1stbyte	equ	9
;========== hardware stack
stack	       equ	0fff7h
;
;========== alphatext screen
alphabuffersize equ	16
;
;========== msc fraction equates
mantsignbit     equ  7       ;msbit of data type byte is set if negative.
redf            equ  1       ;reduced if 0, unreduced if 1.
unredf          equ  1       ;reduced if 0, unreduced if 1.
simpf           equ  0       ;simple if 0, mixed if 1.
mixedf          equ  0       ;simple if 0, mixed if 1.
;
;========== msc general equates
idlistmenuitem  equ  10
;
;
;========== last entry/ans sizes

lastentrysize	equ 512

lastanssize	equ 512

;
;====== keyboard key names
;
;       key presses that do not echo into the display
;       and are not application or menu loading key presses
;
kright          equ       001h
kleft           equ       002h
kup             equ       003h
kdown           equ       004h
kenter          equ       005h
kalphaenter     equ       006h
kalphaup        equ       007h
kalphadown      equ       008h
kclear          equ       009h
kdel            equ       00ah
kins            equ       00bh
krecall         equ       00ch
klastent        equ       00dh
kbol            equ       00eh
keol            equ       00fh
;
kselall         equ       010h
kunselall       equ       011h
kltoti82        equ       012h
backup         equ       013h
krecieve        equ       014h
klnkquit        equ       015h
ktrans          equ       016h
krename         equ       017h
koverw          equ       018h
komit           equ       019h
kcont           equ       01ah
kltoti83        equ       01bh
ksendapp        equ       01ch
kvapps          equ       01ch
ksendid         equ       01dh
ksendsw         equ       01eh
kvall           equ       01fh
kvsendtype      equ       020h  ;send a particular data type (in kextend)
knoway          equ       021h
kyes            equ       022h
kabout          equ       023h  ;about screen
kapp            equ       024h
;
kprogexec       equ       025h
klistflag       equ       026h
;
;open for expansion       028h..02bh
;

enustart       equ       02ch
;
;       key presses that do not echo into the display
;       and are menu switching key presses
;
kareyousure     equ       02ch
kprgm           equ       02dh
kzoom           equ       02eh        ; pull down
kdraw           equ       02fh
ksplot          equ       030h
kstat           equ       031h
kmath           equ       032h
ktest           equ       033h
kappsmenu       equ       034h
kvars           equ       035h
kmem            equ       036h
kconstsetmenu   equ       037h
kconvmenu       equ       038h
ktrigmenu       equ       039h
klist           equ       03ah
constmenu      equ       03bh
;
menuend         equ       constmenu
;
kconst          equ       03ch
;
;
kalpha          equ       03dh
kcatalog        equ       03eh        ; catalog
kinputdone      equ       3fh

;
;       key presses that do not echo into the display
;       and are context switching key presses
;
kquit           equ       040h
appstart        equ       kquit
klinkio         equ       041h
kchkapps        equ       042h        ; check apps
kstated         equ       043h
kgraph          equ       044h
kmode           equ       045h
kprgmed         equ       046h        ; program edit
kprgmcr         equ       047h        ; program create
kwindow         equ       048h        ; range editor
kyequ           equ       049h        ; equation editor
ktable          equ       04ah        ; table editor
ktblset         equ       04bh        ; table set
kchkram         equ       04ch        ; check ram
kdelmem         equ       04dh        ; delete mem
kresetmem       equ       04eh        ; reset mem
kresetdef	equ	  04fh        ; reset default
kprgminput      equ       050h        ; program input
kzfacted        equ       051h        ; zoom factor editor
kerror          equ       052h        ; error
kextapps        equ       053h        ; extra applications.     new
ksolveroot	equ	  054h        ; solve editor
kstatp          equ       055h        ; stat plot
kconsted        equ       056h        ; constant equ editor.
kprgmsetvar     equ       057h        ; setmenu in a program.
kformat         equ       058h        ; format
;
;
append          equ       kformat
;
;       58h - 59h are empty for growth
;
echostart1      equ       05ah
;
;       conditional keys that if in prog edit will echo
;       or if not in prog edit go to the graph screen
;       'trace' , 'zooms'
;
;
ktrace          equ       05ah
kzfit           equ       05bh
kzin            equ       05ch
kzout           equ       05dh
kzprev          equ       05eh
kzbox           equ       05fh
kzdecml         equ       060h
ksetzm          equ       061h
kzsquar         equ       062h
kzstd           equ       063h
kztrig          equ       064h
kzquad1         equ       065h
;kusrzm          equ       065h
;kzsto           equ       066h
kzint           equ       067h
kzstat          equ       068h
;
;       conditional keys that if in prog edit or if the
;       graph screen is not displayed will echo, else will
;       go to the graph screen. 'draw' menu, eval
echostart2      equ       069h
;
;
kselect         equ       069h
kcircl          equ       06ah
kcldrw          equ       06bh
kline           equ       06ch
kpen            equ       06dh
kptchg          equ       06eh
kptoff          equ       06fh
kpton           equ       070h
kvert           equ       071h
khoriz          equ       072h
ktext           equ       073h
ktanln          equ       074h
kmanfit         equ       075h
;
;   these keys will always force to graph mode
;
keval           equ       076h
;
echostart       equ       07dh
;
;       key presses that echo into a buffer
;
kremain         equ       07dh          ;only needed to display 'r'
kplot3          equ       07eh
klistname       equ       07fh
kadd            equ       080h
ksub            equ       081h
kmul            equ       082h
kdiv            equ       083h
kexpon          equ       084h
klparen         equ       085h
krparen         equ       086h
klbrack         equ       087h
krbrack         equ       088h
kshade          equ       089h
kstore          equ       08ah
kcomma          equ       08bh
kchs            equ       08ch
kdecpnt         equ       08dh
k0              equ       08eh
k1              equ       08fh
k2              equ       090h
k3              equ       091h
k4              equ       092h
k5              equ       093h
k6              equ       094h
k7              equ       095h
k8              equ       096h
k9              equ       097h
kee             equ       098h
kspace          equ       099h
kcapa           equ       09ah
kcapb           equ       09bh
kcapc           equ       09ch
kcapd           equ       09dh
kcape           equ       09eh
kcapf           equ       09fh
kcapg           equ       0a0h
kcaph           equ       0a1h
kcapi           equ       0a2h
kcapj           equ       0a3h
kcapk           equ       0a4h
kcapl           equ       0a5h
kcapm           equ       0a6h
kcapn           equ       0a7h
kcapo           equ       0a8h
kcapp           equ       0a9h
kcapq           equ       0aah
kcapr           equ       0abh
kcaps           equ       0ach
kcapt           equ       0adh
kcapu           equ       0aeh
kcapv           equ       0afh
kcapw           equ       0b0h
kcapx           equ       0b1h
kcapy           equ       0b2h
kcapz           equ       0b3h
kvarx           equ       0b4h
kpi             equ       0b5h
kinv            equ       0b6h
ksin            equ       0b7h
kasin           equ       0b8h
kcos            equ       0b9h
kacos           equ       0bah
ktan            equ       0bbh
katan           equ       0bch
ksquare         equ       0bdh
ksqrt           equ       0beh
kln             equ       0bfh
kexp            equ       0c0h
klog            equ       0c1h
kalog           equ       0c2h
ktoabc          equ       0c3h          ;this is abc<>bc & does tobc too.
;
;kclrtbl         equ       0c4h
;
kans            equ       0c5h
kcolon          equ       0c6h
;

kunit           equ       0c7h          ;                       new
kfracslash      equ       0c8h          ;                       new
kroot           equ       0c9h
;
kquest          equ       0cah        ; question mark
kquote          equ       0cbh        ; "
ktheta          equ       0cch
kif             equ       0cdh
kthen           equ       0ceh
kelse           equ       0cfh
kfor            equ       0d0h
kwhile          equ       0d1h
krepeat         equ       0d2h
kend            equ       0d3h
kpause          equ       0d4h
klbl            equ       0d5h
kgoto           equ       0d6h
kisg            equ       0d7h
kdsl            equ       0d8h
kmenu           equ       0d9h
kexec           equ       0dah
kreturn         equ       0dbh
kstop           equ       0dch
kinput          equ       0ddh
kprompt         equ       0deh
kdisp           equ       0dfh
kdispg          equ       0e0h
kdispt          equ       0e1h
koutput         equ       0e2h
kgetkey         equ       0e3h
kclrhome        equ       0e4h
;
;kprtscr         equ       0e5h
;
kpercent        equ        0e6h         ;%                      new
kmixsimp        equ        0e7h         ;                       new
kfracdec        equ        0e8h         ;                       new
ksimp           equ        0e9h         ;                       new
kunredf         equ        0eah         ;                       new
kidiv           equ        0ebh         ;                       new
kconvert        equ       0ech          ;                       new
klbrace         equ       0edh
krbrace         equ       0eeh
kl1a            equ       0efh
kl2a            equ       0f0h
kl3a            equ       0f1h
kl4a            equ       0f2h
kl5a            equ       0f3h
kl6a            equ       0f4h
kconvlength     equ       0f5h        ;1st byte of conversion key.
kconvarea       equ       0f6h        ;1st byte of conversion key.
kconvvolume     equ       0f7h        ;1st byte of conversion key.
kconvtime       equ       0f8h        ;1st byte of conversion key.
kconvtemp       equ       0f9h        ;1st byte of conversion key.
kconvmass       equ       0fah        ;1st byte of conversion key.
kconvspeed      equ       0fbh        ;1st byte of conversion key.
;
;       this key means that it is a 2 byte keycode
;       there are 2 of these keys be careful
;
;
kextendecho2    equ       0fch
;
;
;       this key means that the key press is one that echos
;       into a buffer, and it is a 2 byte key code, go look at
;       (extecho) for the key value
;
kextendecho     equ       0feh
;
ke1bt           equ       0

kdrawinv        equ       ke1bt
kdrawf          equ       ke1bt+1
kpixelon        equ       ke1bt+2
kpixeloff       equ       ke1bt+3
kpxltest        equ       ke1bt+4
krcgdb          equ       ke1bt+5
krcpic          equ       ke1bt+6
kstgdb          equ       ke1bt+7
kstpic          equ       ke1bt+8
kabs            equ       ke1bt+9
ktequ           equ       ke1bt+10    ; ==
ktnoteq         equ       ke1bt+11    ; <>
ktgt            equ       ke1bt+12    ; >
ktgte           equ       ke1bt+13    ; > =
ktlt            equ       ke1bt+14    ; <
ktlte           equ       ke1bt+15    ; < =

kand            equ       ke1bt+16
kor             equ       ke1bt+17
kxor            equ       ke1bt+18
knot            equ       ke1bt+19

;klr1            equ       ke1bt+20

kxroot          equ       ke1bt+21
kcube           equ       ke1bt+22
kcbrt           equ       ke1bt+23    ; cube root
ktodec          equ       ke1bt+24
;
kcubicr         equ       ke1bt+25
kquartr         equ       ke1bt+26
;
kplot1          equ       ke1bt+27
kplot2          equ       ke1bt+28
;

kround          equ       ke1bt+29
kipart          equ       ke1bt+30
kfpart          equ       ke1bt+31
kint            equ       ke1bt+32

krand           equ       ke1bt+33
knpr            equ       ke1bt+34
kncr            equ       ke1bt+35
kxfactorial     equ       ke1bt+36

krad            equ       ke1bt+37
kdegr           equ       ke1bt+38    ; degrees conv
kapost          equ       ke1bt+39
ktodms          equ       ke1bt+40
krtopo          equ       ke1bt+41    ; r
krtopr          equ       ke1bt+42
kptorx          equ       ke1bt+43
kptory          equ       ke1bt+44

krowswap        equ       ke1bt+45
krowplus        equ       ke1bt+46
ktimrow         equ       ke1bt+47
ktrowp          equ       ke1bt+48

ksorta          equ       ke1bt+49
ksortd          equ       ke1bt+50
kseq            equ       ke1bt+51

kmin            equ       ke1bt+52
kmax            equ       ke1bt+53
kmean           equ       ke1bt+54
kmedian         equ       ke1bt+55
ksum            equ       ke1bt+56
kprod           equ       ke1bt+57

kdet            equ       ke1bt+58
ktransp         equ       ke1bt+59
kdim            equ       ke1bt+60    ; same as one in list
kfill           equ       ke1bt+61    ; same as one in list
kident          equ       ke1bt+62
krandm          equ       ke1bt+63
kaug            equ       ke1bt+64

klstmode        equ       ke1bt+65
;available      equ       ke1bt+66

kmrad           equ       ke1bt+67
kmdeg           equ       ke1bt+68
kmnormf         equ       ke1bt+69
kmsci           equ       ke1bt+70
kfix            equ       ke1bt+71
kmfloat         equ       ke1bt+72
ksetmenu        equ       ke1bt+73
keofrac         equ       ke1bt+74

kboyicon        equ     ke1bt+75
ktreeicon       equ     ke1bt+76
kfaceicon       equ     ke1bt+77
kcircleicon     equ     ke1bt+78
kdiamondicon    equ     ke1bt+79
kstaricon       equ     ke1bt+80
kdollaricon      equ    ke1bt+81
;available      equ     ke1bt+82
;available      equ     ke1bt+83

kfnon           equ       ke1bt+84
kfnoff          equ       ke1bt+85

kplotson        equ       ke1bt+86
kplotsoff       equ       ke1bt+87

kpixelchg       equ       ke1bt+88

ksendmbl        equ       ke1bt+89
krecvmbl        equ       ke1bt+90

kboxplot        equ       ke1bt+ 91
kboxicon        equ       ke1bt+ 92

kcrossicon      equ       ke1bt+ 93
kdoticon        equ       ke1bt+ 94

ke2bt           equ       ke1bt+95

kseqential      equ       ke2bt         ;105
ksimulg         equ       ke2bt+1       ;106
kcoordon        equ       ke2bt+2       ;107
kcoordoff       equ       ke2bt+3       ;108
kdrawline       equ       ke2bt+4       ;109
kdrawdot        equ       ke2bt+5       ;110
kaxison         equ       ke2bt+6       ;111
kaxisoff        equ       ke2bt+7       ;112
kgridon         equ       ke2bt+8       ;113
kgridoff        equ       ke2bt+9       ;114
klblon          equ       ke2bt+10      ;115
klbloff         equ       ke2bt+11      ;116
kpolarg         equ       ke2bt+12      ;117
krectg          equ       ke2bt+13      ;118

kl1             equ       ke2bt+14      ;119 = 77h
kl2             equ       ke2bt+15
kl3             equ       ke2bt+16
kl4             equ       ke2bt+17
kl5             equ       ke2bt+18
kl6             equ       ke2bt+19

kmata           equ       ke2bt+20
kmatb           equ       ke2bt+21
kmatc           equ       ke2bt+22
kmatd           equ       ke2bt+23
kmate           equ       ke2bt+24

kxmin           equ       ke2bt+25
kxmax           equ       ke2bt+26
kxscl           equ       ke2bt+27
kymin           equ       ke2bt+28
kymax           equ       ke2bt+29
kyscl           equ       ke2bt+30
ktmin           equ       ke2bt+31
ktmax           equ       ke2bt+32
ktstep          equ       ke2bt+33
komin           equ       ke2bt+34
komax           equ       ke2bt+35
kostep          equ       ke2bt+36
ku0             equ       ke2bt+37
kv0             equ       ke2bt+38
knmin           equ       ke2bt+39
knmax           equ       ke2bt+40
kdeltay         equ       ke2bt+41
kdeltax         equ       ke2bt+42

kzxmin          equ       ke2bt+43
kzxmax          equ       ke2bt+44
kzxscl          equ       ke2bt+45
kzymin          equ       ke2bt+46
kzymax          equ       ke2bt+47
kzyscl          equ       ke2bt+48
kztmin          equ       ke2bt+49
kztmax          equ       ke2bt+50
kztstep         equ       ke2bt+51
kzomin          equ       ke2bt+52
kzomax          equ       ke2bt+53
kzostep         equ       ke2bt+54
;
kabc            equ       ke2bt+55
kbc             equ       ke2bt+56
kautosimp       equ       ke2bt+57
kmansimp        equ       ke2bt+58

kdellast        equ       ke2bt+59
ksinglec        equ       ke2bt+60
kmultic         equ       ke2bt+61

kpic1           equ       ke2bt+62
kpic2           equ       ke2bt+63
kpic3           equ       ke2bt+64

kdelvar         equ       ke2bt+65
kgetcalc        equ       ke2bt+66

kstndrd         equ       ke2bt+67
kparam          equ       ke2bt+68
kpolar          equ       ke2bt+69
kafillon        equ       ke2bt+70
kafilloff       equ       ke2bt+71
kacalcon        equ       ke2bt+72
kacalcoff       equ       ke2bt+73

ksetconst       equ       ke2bt+74
;
kgraphstyle     equ       ke2bt+75
;
kexpron         equ       ke2bt+76
kexproff        equ       ke2bt+77
kstata          equ       ke2bt+78
kstatb          equ       ke2bt+79
kstatc          equ       ke2bt+80
kcorr           equ       ke2bt+81
kstatd          equ       ke2bt+82
kstate          equ       ke2bt+83
kregeq          equ       ke2bt+84
kminx           equ       ke2bt+85
kq1             equ       ke2bt+86
kmd             equ       ke2bt+87
kq3             equ       ke2bt+88
kmaxx           equ       ke2bt+89
kstatx1         equ       ke2bt+90
kstaty1         equ       ke2bt+91
kstatx2         equ       ke2bt+92
kstaty2         equ       ke2bt+93
kstatx3         equ       ke2bt+94
kstaty3         equ       ke2bt+95
ktblmin         equ       ke2bt+96
ktblstep        equ       ke2bt+97
ksetuplst       equ       ke2bt+98
kclralllst      equ       ke2bt+99
klogistic       equ       ke2bt+100
kc1             equ       ke2bt+101   ;constant c1
kc2             equ       ke2bt+102   ;constant c2
kc3             equ       ke2bt+103   ;constant c3
kc4             equ       ke2bt+104   ;constant c4
kplotstart      equ       ke2bt+105   ; plotstart
kzplotstart     equ       ke2bt+106   ; zplotstart
kxfact          equ       ke2bt+107   ; xfact
kyfact          equ       ke2bt+108   ; yfact
kmaxy           equ       ke2bt+109   ; maxy
ktblinput       equ       ke2bt+110   ; web off
kplotstep       equ       ke2bt+111
kzplotstep      equ       ke2bt+112
kselecta        equ       ke2bt+113
kzfita          equ       ke2bt+114
konevar         equ       ke2bt+115
ktwovar         equ       ke2bt+116
klr             equ       ke2bt+117
klrexp          equ       ke2bt+118
klrln           equ       ke2bt+119
;klrpwr          equ       ke2bt+
;kmanfit         equ       ke2bt+
kmedmed         equ       ke2bt+120
kquad           equ       ke2bt+121
kclrlst         equ       ke2bt+122
khist           equ       ke2bt+123
kxyline         equ       ke2bt+124
kscatter        equ       ke2bt+125
klr1            equ       ke2bt+126
kclrtbl         equ       ke2bt+127
ke2bt_end       equ       kclrtbl
;
;

; new 2 byte keys

ke2bt2          equ       0

kgdb1           equ       ke2bt2
kgdb2           equ       ke2bt2+1
kgdb3           equ       ke2bt2+2
ky1             equ       ke2bt2+3
ky2             equ       ke2bt2+4
ky3             equ       ke2bt2+5
ky4             equ       ke2bt2+6
ky5             equ       ke2bt2+7
ky6             equ       ke2bt2+8
ky7             equ       ke2bt2+9
ky8             equ       ke2bt2+10
ky9             equ       ke2bt2+11
ky0             equ       ke2bt2+12
kx1t            equ       ke2bt2+13
ky1t            equ       ke2bt2+14
kx2t            equ       ke2bt2+15
ky2t            equ       ke2bt2+16
kx3t            equ       ke2bt2+17
ky3t            equ       ke2bt2+18
kx4t            equ       ke2bt2+19
ky4t            equ       ke2bt2+20
kx5t            equ       ke2bt2+21
ky5t            equ       ke2bt2+22
kx6t            equ       ke2bt2+23
ky6t            equ       ke2bt2+24
kr1             equ       ke2bt2+25
kr2             equ       ke2bt2+26
kr3             equ       ke2bt2+27
kr4             equ       ke2bt2+28
kr5             equ       ke2bt2+29
kr6             equ       ke2bt2+30
kgdb4           equ       ke2bt2+31
kgdb5           equ       ke2bt2+32
kgdb6           equ       ke2bt2+33
kpic4           equ       ke2bt2+34
kpic5           equ       ke2bt2+35
kpic6           equ       ke2bt2+36
kgdb7           equ       ke2bt2+37
kgdb8           equ       ke2bt2+38
kgdb9           equ       ke2bt2+39
kgdb0           equ       ke2bt2+40
kpic7           equ       ke2bt2+41
kpic8           equ       ke2bt2+42
kpic9           equ       ke2bt2+43
kpic0           equ       ke2bt2+44
kstatn          equ       ke2bt2+45
kxmean          equ       ke2bt2+46
;kconj           equ       ke2bt2+47
;kreal           equ       ke2bt2+48
;kfangle         equ       ke2bt2+49
klcm            equ       ke2bt2+50
kgcd            equ       ke2bt2+51
krandint        equ       ke2bt2+52
kcoin           equ       ke2bt2+53
kdice           equ       ke2bt2+54
kremainder      equ       ke2bt2+55
;krandnorm       equ       ke2bt2+53
;ktopolar        equ       ke2bt2+54
;ktorect         equ       ke2bt2+55
kymean          equ       ke2bt2+56
kstdx           equ       ke2bt2+57
kstdx1          equ       ke2bt2+58
kw0             equ       ke2bt2+59
kmatf           equ       ke2bt2+60
kmatg           equ       ke2bt2+61
kmatrh          equ       ke2bt2+62
kmati           equ       ke2bt2+63
kmatj           equ       ke2bt2+64
kymean1         equ       ke2bt2+65
kstdy           equ       ke2bt2+66
kstdy1          equ       ke2bt2+67
kmattolst       equ       ke2bt2+68
klsttomat       equ       ke2bt2+69
kcumsum         equ       ke2bt2+70
kdeltalst       equ       ke2bt2+71
kstddev         equ       ke2bt2+72
kvariance       equ       ke2bt2+73
klength         equ       ke2bt2+74
kequtostrng     equ       ke2bt2+75
kstrngtoequ     equ       ke2bt2+76
kexpr           equ       ke2bt2+77
ksubstrng       equ       ke2bt2+78
kinstrng        equ       ke2bt2+79
kstr1           equ       ke2bt2+80
kstr2           equ       ke2bt2+81
kstr3           equ       ke2bt2+82
kstr4           equ       ke2bt2+83
kstr5           equ       ke2bt2+84
kstr6           equ       ke2bt2+85
kstr7           equ       ke2bt2+86
kstr8           equ       ke2bt2+87
kstr9           equ       ke2bt2+88
kstr0           equ       ke2bt2+89
ksimpfact       equ       ke2bt2+90
kpictplot       equ     ke2bt2+91
kcircleplot     equ     ke2bt2+92
kstemplot       equ     ke2bt2+93
kbarplot        equ     ke2bt2+94
;kboyicon        equ     ke2bt2+95
;kgirlicon       equ     ke2bt2+96
;kfaceicon       equ     ke2bt2+97
;kcircleicon     equ     ke2bt2+98
;kdiamondicon    equ     ke2bt2+99
;kstaricon       equ     ke2bt2+100
;kdollaricon      equ     ke2bt2+101

ksumx           equ       ke2bt2+102
ksumx2          equ       ke2bt2+103

ksumy           equ       ke2bt2+104
kxres           equ       ke2bt2+105

ksumy2          equ       ke2bt2+106
ksumxy          equ       ke2bt2+107
kuxres          equ       ke2bt2+108
kmodbox         equ       ke2bt2+109
knormprob       equ       ke2bt2+110

kminy           equ       ke2bt2+111   ; miny
kref            equ       ke2bt2+112
krref           equ       ke2bt2+113
klrsqr          equ       ke2bt2+114
kbrsqr          equ       ke2bt2+115
kdiagon         equ       ke2bt2+116
kdiagoff        equ       ke2bt2+117
;kun1            equ       ke2bt2+152   ; for rcl use when gotten from 82
;kvn1            equ       ke2bt2+153   ; for rcl use when gotten from 82
ke2bt2_end      equ       kdiagoff

;=================================================
;    context equates
;=================================================
cxcmd           equ     kquit                   ;home screen
cxappsapp       equ     kchkapps                ;apps usage screen
cxprgmedit      equ     kprgmed                 ;program editor
cxequedit       equ     kyequ                   ;equation editor
cxgrrange       equ     kwindow                 ;graph range editor
cxgrzfact       equ     kzfacted                ;graph zoom factors editor
cxgraph         equ     kgraph                  ;graph mode
cxstatedit      equ     kstated                 ;statistics list editor
cxprgminput     equ     kprgminput              ;programmed input
cxerror         equ     kerror                  ;error handler
cxlinkio        equ     klinkio                 ;link i/o interface
cxmem           equ     kresetmem               ;reset memory
cxdefmem        equ     kresetdef               ;reset default
cxramapp        equ     kchkram                 ;ram usage screen
cxmode          equ     kmode                   ;mode settings screen
cxerase         equ     kdelmem                 ;memory erase
;cxstatset       equ     kstatsetup              ;stat set up
cxprgmcreate    equ     kprgmcr                 ;program create
cxtableeditor   equ     ktable                  ;table editor
cxtableset      equ     ktblset                 ;table set up
cxstatplot      equ     kstatp                  ;stat plots
;cxinfstat       equ     kinfstat                ;inferential statistic
cxformat        equ     kformat                 ;format context
;cxsolvetvm      equ     ksolvetvm               ;solve tvm
cxsolveroot     equ     ksolveroot              ;solve root
cxextapps       equ     kextapps        ;053h   ; extra applications.     new
cxconsted       equ     kconsted        ;056h   ; constant equ editor.
cxprgmsetvar    equ     kprgmsetvar     ;057h   ; setmenu in a program.

;============================================
;	scan code equates
;============================================

skdown          equ              01h
skleft          equ              02h
skright         equ              03h
skup            equ              04h
skenter         equ              09h
skadd           equ              0ah
sksub           equ              0bh
skmul           equ              0ch
skdiv           equ              0dh
skconst         equ              0eh
skclear         equ              0fh
skchs           equ              11h
sk3             equ              12h
sk6             equ              13h
sk9             equ              14h
skrparen        equ              15h
skmixsimp       equ              16h
skappsmenu      equ              17h
skdecpnt        equ              19h
sk2             equ              1ah
sk5             equ              1bh
sk8             equ              1ch
sklparen        equ              1dh
skfracdec       equ              1eh
skprgm          equ              1fh
skstated        equ              20h
sk0             equ              21h
sk1             equ              22h
sk4             equ              23h
sk7             equ              24h
skpercent       equ              25h
skfracslash     equ              26h
skexpon         equ              27h
skdraw          equ              28h
skstore         equ              2ah
skcomma         equ              2bh
skvarx          equ              2ch
sksimp          equ              2dh
skunit          equ              2eh
sksquare        equ              2fh
skmath          equ              30h
skgraph         equ              31h
sktrace         equ              32h
skzoom          equ              33h
skwindow        equ              34h
skyequ          equ              35h
sk2nd           equ              36h
skmode          equ              37h
skdel           equ              38h

;==========================================
;	tokens
;==========================================

eosstart        equ       0

;   display conversions  come immediately before 'tstore'

dconv   equ     01h

ttodms          equ       dconv       ; 01h
tconvert        equ       dconv+1     ; 02h
ttoabc          equ       dconv+2     ; 03h    > a b/c
tunredf         equ     03h     ;down arrow for unreduced fraction token

tstore          equ       04h          ;04h lstore                01
;
tboxplot        equ       05h
;
bracks          equ       06h
;
tlbrack         equ       bracks      ; 06h '['
trbrack         equ       bracks+1    ; 07h ']'
tlbrace         equ       bracks+2    ; 08h '{'
trbrace         equ       bracks+3    ; 09h '}'

tpost1          equ       bracks+4
;
tfromrad        equ       tpost1       ; 0ah lradian
tfromdeg        equ       tpost1+1     ; 0bh ldegree
trecip          equ       tpost1+2     ; 0ch linverse
tsqr            equ       tpost1+3     ; 0dh lsquare
ttrnspos        equ       tpost1+4     ; 0eh ltranspose
tcube           equ       tpost1+5     ; 0fh '^3'
;
tlparen         equ       10h         ; 10h '('
trparen         equ       11h         ; 11h ')'
;
;
;   immediate unary tokens go from here to 'eosel-1'
;
imun            equ       12h
;
tround          equ       imun        ; 12h 'round'
tpxtst          equ       imun+1      ; 13h 'pxl-test'
taug            equ       imun+2      ; 14h 'aug'
trowswap        equ       imun+3      ; 15h 'rswap'
trowplus        equ       imun+4      ; 16h 'radd'
tmrow           equ       imun+5      ; 17h 'multr'
tmrowplus       equ       imun+6      ; 18h 'mradd'
tmax            equ       imun+7      ; 19h 'max'
tmin            equ       imun+8      ; 1ah 'min'
trtopr          equ       imun+9      ; 1bh 'r>pr
trtopo          equ       imun+10     ; 1ch 'r>po
tptorx          equ       imun+11     ; 1dh 'p>rx
tptory          equ       imun+12     ; 1eh 'p>ry
tmedian         equ       imun+13     ; 1fh 'median
trandm          equ       imun+14     ; 20h 'randm'
tmean           equ       imun+15     ; 21h 'mean('
tlstmode        equ       imun+16     ; 22h 'mode('
;the below tokens are immediate, but require an expression as 1st arg & so
;are grouped together...
troot           equ       imun+17     ; 23h 'solve('
tseries         equ       imun+18     ; 24h 'seq'
tevalf          equ       imun+20     ; 26h             ;keep until sure.
imunend         equ     imun+20         ;end of immediate unaries.

tsolveequal     equ     imun+21         ;teq ('=') in solver -> tsolveequal.
teofrac         equ     imun+22         ; 28h end of fraction in home screen.
;
;
;  token 29h cannot be an eos function since el(=29h already
;
teosel          equ       imun+23
;
tspace          equ       teosel       ; 29h   ' '
tstring         equ       teosel+1     ; 2ah  '"'
tcomma          equ       teosel+2     ; 2bh  ','
;
;
;
tremain         equ     2ch     ;'r' for remainder display.
; tremain is needed in home screen where turn display chars into tokens.
; (there is no token for small 'r').
; ===== postfix functions
;
tpost           equ       2eh
;
tfact           equ       tpost       ; 2eh '!'
tpercent        equ     tpost+1       ; 2fh '%'                 new

;       number tokens

nums            equ       30h
;
t0              equ       nums        ; 30h
t1              equ       nums+1      ; 31h
t2              equ       nums+2      ; 32h
t3              equ       nums+3      ; 33h
t4              equ       nums+4      ; 34h
t5              equ       nums+5      ; 35h
t6              equ       nums+6      ; 36h
t7              equ       nums+7      ; 37h
t8              equ       nums+8      ; 38h
t9              equ       nums+9      ; 39h
tdecpt          equ       nums+10     ; 3ah
tee             equ       nums+11     ; 3bh
;
;   binary op
;
tor             equ       3ch         ; 3ch  '_or_'
txor            equ       3dh         ; 3dh
;
tcolon          equ       3eh         ; 3eh ':'
tenter          equ       3fh         ; 3fh  lenter
;
;   binary op
;
tand            equ       40h         ; 40h  '_and_'               05
;
;       letter tokens
;
let             equ       41h
;
ta              equ       let         ; 41h
tb              equ       let+1       ; 42h
tc              equ       let+2       ; 43h
td              equ       let+3       ; 44h
te              equ       let+4       ; 45h
tf              equ       let+5       ; 46h
tg              equ       let+6       ; 47h
th              equ       let+7       ; 48h
ti              equ       let+8       ; 49h
tj              equ       let+9       ; 4ah
tk              equ       let+10      ; 4bh
tl              equ       let+11      ; 4ch
tm              equ       let+12      ; 4dh
tn              equ       let+13      ; 4eh
to              equ       let+14      ; 4fh
tp              equ       let+15      ; 50h
tq              equ       let+16      ; 51h
tr              equ       let+17      ; 52h
ts              equ       let+18      ; 53h
tt              equ       let+19      ; 54h
tu              equ       let+20      ; 55h
tv              equ       let+21      ; 56h
tw              equ       let+22      ; 57h
tx              equ       let+23      ; 58h
ty              equ       let+24      ; 59h
tz              equ       let+25      ; 5ah
teof            equ       let+26      ; 5bh
;
;
;   these var tokens are 1st of a double token
;
;   see 'vars.doc' for more info on vars
;
vtoks   equ     let+27    ;
;
;       user matrix token, 2nd token needed for name
;
tvarmat         equ       vtoks       ; 5ch
;
;       user list token, 2nd token needed for name
;
tvarlst         equ       vtoks+1     ; 5dh
;
;       user equation token, 2nd token needed for name
;
tvarequ         equ       vtoks+2     ; 5eh
;
;       this is a single token, i will have to read the name in
;       every single time i execute
;
tprog           equ       vtoks+3     ; 5fh
;
;       user pict token, 2nd token needed for name
;
tvarpict        equ       vtoks+4     ; 60h
;
;       user gdb token, 2nd token needed for name
;
tvargdb         equ       vtoks+5     ; 61h
;
;       system vars that are used for output only
;       cannot be stored to by the user
;
tvarout         equ       vtoks+6     ; 62h
;
;       system vars that are not in the symbol table
;       but the user can store to them.
;
tvarsys         equ       vtoks+7     ; 63h
;
;
; ===== mode setting commands
;
modesa          equ       vtoks+8     ; 64h
;
trad            equ       modesa      ; 64h 'radian'
tdeg            equ       modesa+1    ; 65h 'degree'
tnormf          equ       modesa+2    ; 66h 'normal'
tsci            equ       modesa+3    ; 67h 'sci'
tfloat          equ       modesa+4    ; 68h 'float'
tfix            equ       modesa+5    ; 69h 'fix'
;
cmps            equ       6ah
;
teq             equ       cmps        ; 6ah '=='
tlt             equ       cmps+1      ; 6bh '<'
tgt             equ       cmps+2      ; 6ch '>'
tle             equ       cmps+3      ; 6dh lle
tge             equ       cmps+4      ; 6eh lge
tne             equ       cmps+5      ; 6fh lne
;
;   binary op
;
tadd            equ       70h         ; 70h '+'
tsub            equ       71h         ; 71h '-'
;
tans            equ       72h         ; 72h
;
; ===== mode setting commands
;       mode tokens are now 2byte tokens, same as graph format tokens.
tmodes2          equ       73h
;
;
;       graph format tokens are 2 byte tokens
;
tgformat        equ       tmodes2      ; 73h
;
;
tboyicon        equ     78h           ; for stat plots in programs
ttreeicon       equ     79h           ; for stat plots in programs
tdollaricon     equ     7ah           ; for stat plots in programs
tfaceicon       equ     7bh           ; for stat plots in programs
tcircleicon     equ     7ch           ; for stat plots in programs
tdiamondicon    equ     7dh           ; for stat plots in programs
tstaricon       equ     7eh           ; for stat plots in programs
tboxicon        equ       7fh
tcrossicon      equ       80h
tdoticon        equ       81h
;
;       binary op
;
tmul            equ       82h         ; 82h  '*'
tdiv            equ       83h         ; 83h  '/'
tidiv           equ       84h         ; 84h  'int/'
;
;       some graph commands
;
grcmds          equ       85h
;
ttrace          equ       grcmds      ; 85h 'trace'
tcldrw          equ       grcmds+1    ; 86h 'cldrw'
tzoomstd        equ       grcmds+2    ; 87h 'zstd'
tzoomtrg        equ       grcmds+3    ; 88h 'ztrg'
tzoombox        equ       grcmds+4    ; 89h 'zbox'
tzoomin         equ       grcmds+5    ; 8ah 'zin'
tzoomout        equ       grcmds+6    ; 8bh 'zout'
tzoomsqr        equ       grcmds+7    ; 8ch 'zsqr'
tzoomint        equ       grcmds+8    ; 8dh 'zint'
tzoomprev       equ       grcmds+9    ; 8eh 'zprev'
tzoomdec        equ       grcmds+10   ; 8fh 'zdecm'
tzoosstat       equ       grcmds+11   ; 90h 'zstat
tzoomquad1      equ       grcmds+12   ; 91h 'quadrant 1'
tzoomso        equ       grcmds+14   ; 93h  zoom store
ttext           equ       grcmds+15   ; 94h
;
;       binary op
;
tnpr            equ       grcmds+16   ; 95h '_npr_'
tncr            equ       grcmds+17   ; 96h '_ncr_'
;
;       more graph commands (start of graph commands requiring arguments)
;
tyon            equ       grcmds+18   ; 97h 'fnon_'
tyoff           equ       grcmds+19   ; 98h 'fnoff_'
tstpic          equ       grcmds+20   ; 99h 'stpic_'
trcpic          equ       grcmds+21   ; 9ah 'rcpic_'
tline           equ       grcmds+22   ; 9bh 'line'
tvert           equ       grcmds+23   ; 9ch 'vert_'
tpton           equ       grcmds+24   ; 9dh 'pton'
tptoff          equ       grcmds+25   ; 9eh 'ptoff'
;
;  token a0 cannot be an eos function since low mult=a0 already
;
tptchg          equ       grcmds+26   ; 9fh 'ptchg'
tpxon           equ       grcmds+27   ; a0h
tpxoff          equ       grcmds+28   ; a1h
tpxchg          equ       grcmds+29   ; a2h
tshade          equ       grcmds+30   ; a3h 'shade'
tcircl          equ       grcmds+31   ; a4h 'circl'
thorz           equ       grcmds+32   ; a5h 'horizontal'
ttanln          equ       grcmds+33   ; a6h 'tanln'
;tmanfit         equ       grcmds+34   ; a7h 'manual fit'

;
;available      equ       grcmds+35   ; a8h
;
tdrinv          equ       grcmds+36   ; a9h 'drinv_'
tdrawf          equ       grcmds+36   ; a9h 'drawf_'
;
tvarstrng       equ       0aah
;
; ===== functions with no arguments
;
noarg   equ     0abh
;
trand           equ       noarg       ; abh 'rand'
tpi             equ       noarg+1     ; ach  lpi
tgetkey         equ       noarg+2     ; adh 'getky'
;
;
tapost          equ       tgetkey+1   ; aeh apostrophy
tquest          equ       tapost+1    ; afh question mark
;
;
; ===== unary functions
;
;       unary : these unary functions take a combination of
;               arguments, real,list,matrix
;
unary   equ     tquest+1              ; b0h
;
tchs            equ       unary       ; b0h
tint            equ       unary+1     ; b1h
tabs            equ       unary+2     ; b2h
tdet            equ       unary+3     ; b3h
tident          equ       unary+4     ; b4h
tdim            equ       unary+5     ; b5h
tsum            equ       unary+6     ; b6h
tprod           equ       unary+7     ; b7h
tipart          equ       unary+9     ; b9h
tfpart          equ       unary+10    ; bah
;
;
;       new 2 byte tokens
;
t2bytetok       equ       0bbh         ;
;

;       unarylr : unary functions that only work with
;                 real numbers and lists
;
unarylr equ     unary+12
;
tsqrt           equ       unarylr     ; bch
tcubrt          equ       unarylr+1   ; bdh
tln             equ       unarylr+2   ; beh
texp            equ       unarylr+3   ; bfh
tlog            equ       unarylr+4   ; c0h
talog           equ       unarylr+5   ; c1h
tsin            equ       unarylr+6   ; c2h
tasin           equ       unarylr+7   ; c3h
tcos            equ       unarylr+8   ; c4h
tacos           equ       unarylr+9   ; c5h
ttan            equ       unarylr+10  ; c6h
tatan           equ       unarylr+11  ; c7h
lastunary2       equ       unarylr+11  ; c7h
tmixsimp        equ     unarylr+12 ;c8convert between mixed(ab/c) & simple(b/c)
tfracdec        equ     unarylr+13 ;c9h convert between fraction & decimal.
tsimp           equ     unarylr+14 ;cah simplify a fraction.

;       binary op
;
tpower          equ       0cbh        ; cbh    '^'
txroot          equ       0cch        ; cch    lsupx,lroot

;       some programming commands
;
progtok equ     unarylr+17
;
tsetmenu        equ       progtok     ; cdh
tif             equ       progtok+1   ; ceh
tthen           equ       progtok+2   ; cfh
telse           equ       progtok+3   ; d0h
twhile          equ       progtok+4   ; d1h
trepeat         equ       progtok+5   ; d2h
tfor            equ       progtok+6   ; d3h
tend            equ       progtok+7   ; d4h
treturn         equ       progtok+8   ; d5h
tlbl            equ       progtok+9   ; d6h   'lbl_'
tgoto           equ       progtok+10  ; d7h   'goto_'
tpause          equ       progtok+11  ; d8h   'pause_'
tstop           equ       progtok+12  ; d9h   'stop'
tisg            equ       progtok+13  ; dah   'is>'
tdsl            equ       progtok+14  ; dbh   'ds<'
tinput          equ       progtok+15  ; dch   'input_'
tprompt         equ       progtok+16  ; ddh   'prompt_'
tdisp           equ       progtok+17  ; deh   'disp_'
tdispg          equ       progtok+18  ; dfh   'dispg'


;  token e0 cannot be an eos function since hi mu
;
toutput         equ       progtok+19  ; e0h    'outpt'
tcllcd          equ       progtok+20  ; e1h    'cllcd'
tfill           equ       progtok+21  ; e2h    'fill'
tsorta          equ       progtok+22  ; e3h    'sorta_'
tsortd          equ       progtok+23  ; e4h    'sortd_'
tdisptab        equ       progtok+24  ; e5h    'disp table
tmenu           equ       progtok+25  ; e6h    'menu'
tsendmbl        equ       progtok+26  ; e7h    'send'
tgetmbl         equ       progtok+27  ; e8h    'get'
;
;       stat plot commands
;
statpcmd        equ       progtok+28
;
tploton         equ       statpcmd    ; e9h     ' plotson'
tplotoff        equ       statpcmd+1   ; eah     ' plotsoff
cmdlen0         equ     tplotoff - progtok + 1
cmdoff1         equ     cmdlen0 - 1
;
tlistname       equ       0ebh        ; list designator
;
tplot1          equ       0ech
tplot2          equ       0edh
tplot3          equ       0eeh
cmdlen2         equ     tplot3 - tplot1 + 1
tunit           equ       0efh        ; efh unit for mixed fractions (a u b/c)
tfracslash      equ       0f0h         ; f0h '/' (bc symbol)

tstatcmd         equ       0f2h
;
; 2nd half of graph format tokens
;
; ===== format settings commands
;

gfmt            equ       1
;
tseq            equ       gfmt        ; 01  'seqg'
tsimulg         equ       gfmt+1      ; 02  'simulg'
tcoordon        equ       gfmt+2      ; 03  'coordon'
tcoordoff       equ       gfmt+3      ; 04  'coordoff'
tdrawline       equ       gfmt+4      ; 05  'drawline'
tdrawdot        equ       gfmt+5      ; 06  'drawdot'
taxison         equ       gfmt+6      ; 07  'axeson'
taxisoff        equ       gfmt+7      ; 08  'axesoff'
tgridon         equ       gfmt+8      ; 09  'gridon'
tgridoff        equ       gfmt+9      ; 0a  'gridoff'
tlblon          equ       gfmt+10     ; 0b  'labelon'
tlbloff         equ       gfmt+11     ; 0c  'labeloff'
tpolarg         equ       gfmt+12     ; 0d  'polargc'
trectg          equ       gfmt+13     ; 0e  'rectgc'
;
;
; 2nd half of mode setting tokens
;
modes2          equ       gfmt+14
;
tstndrd         equ       modes2      ; 0fh 'func'
tparam          equ       modes2+1    ; 10h 'param'
tpolar          equ       modes2+2    ; 11h 'pol'
tafillon        equ       modes2+3    ; 12h 'auto fill on
tafilloff       equ       modes2+4    ; 13h
tacalcon        equ       modes2+5    ; 14h
tacalcoff       equ       modes2+6    ; 15h 'autofill off
;
tmabc           equ       modes2+7    ; 16h 'aub/c mode'
tmbc            equ       modes2+8    ; 17h 'b/c mode'
tmautosimp      equ       modes2+9    ; 18h 'b/c mode'
tmmansimp       equ       modes2+10   ; 19h 'b/c mode'
tmsinglec       equ       modes2+11   ; 1ah single constant
tmmultic        equ       modes2+12   ; 1bh multi constant


statregs        equ     1
statcmd         equ       0F2h
tonevar         equ     statregs      ; 01h    'onevar_'
ttwovar         equ     statregs+1    ; 02h
tlr1            equ     statregs+2    ; 03h    'linr(ax+b
tmanfit         equ     statcmd+5   ;07h     'manual fit'
tmedmed         equ     statregs+6    ; 08h
tquad           equ     statregs+7    ; 09h
tclrlst         equ     statregs+8    ; 0ah    'clear list
tclrtbl         equ     statregs+9    ; 0bh    clear table
thist           equ     statregs+10   ; 0ch    'hist_'
txyline         equ     statregs+11   ; 0dh    'xyline_'
tscatter        equ     statregs+12   ; 0eh    'scatter_'
tlr             equ     statregs+13   ; 0fh    'linr(a+bx

;       2nd half of user list tokens
;
tl1             equ       01h         ; list 1
tl2             equ       02h         ; list 2
tl3             equ       03h         ; list 3
tl4             equ       04h         ; list 4
tl5             equ       05h         ; list 5
tl6             equ       06h         ; list 6

;       2nd half of user equation tokens
;
;
;       "y" equations have bit 4 set
ty1             equ       10h         ; y1
ty2             equ       11h         ; y2
ty3             equ       12h         ; y3
ty4             equ       13h         ; y4
;
;       constant equations have bit 7 set
tc1             equ       80h         ; const1
tc2             equ       81h         ; const2
tc3             equ       82h         ; const3
tc4             equ       83h         ; const4
;
;       2nd half of user picture tokens
;
tpic1           equ       01h         ; pic1
tpic2           equ       02h         ; pic2
tpic3           equ       03h         ; pic3

;       2nd half of user graph database tokens
;
tgdb1           equ       01h         ; gdb1
tgdb2           equ       02h         ; gdb2
tgdb3           equ       03h         ; gdb3
tgdb4           equ       04h         ; gdb4
tgdb5           equ       05h         ; gdb5
tgdb6           equ       06h         ; gdb6
tgdb7           equ       07h         ; gdb7
tgdb8           equ       08h         ; gdb8
tgdb9           equ       09h         ; gdb9
tgdb0           equ       0ah         ; gdb0
;
;       2nd half of string vars
;
tstr1           equ       01h
tstr2           equ       02h
tstr3           equ       03h
tstr4           equ       04h
tstr5           equ       05h
tstr6           equ       06h
tstr7           equ       07h
tstr8           equ       08h
tstr9           equ       09h
tstr0           equ       0ah

;
;       2nd half of system output only variables
;
;
;   open        equ       00h           ;0 must never be used as a token value.
;
tregeq          equ       01h         ; regression equation
;
tstatn          equ       02h         ; statistics n
;
txmean          equ       03h         ; x mean
tsumx           equ       04h         ; sum(x)
tsumxsqr        equ       05h         ; sum(x^2)
tstdx           equ       06h         ; standard dev x
tstdpx          equ       07h         ; standard dev pop x
tminx           equ       08h         ; min x value
tmaxx           equ       09h         ; max x value
;
tminy           equ       0ah         ; min y value
tmaxy           equ       0bh         ; max y value
tymean          equ       0ch         ; y mean
tsumy           equ       0dh         ; sum(y)
tsumysqr        equ       0eh         ; sum(y^2)
tstdy           equ       0fh         ; standard dev y
tstdpy          equ       10h         ; standard dev pop y
;
tsumxy          equ       11h         ; sum(xy)
tcorr           equ       12h         ; correlation
tmedx           equ       13h         ; med(x)
tq1             equ       14h         ; 1st quadrant of x
tq3             equ       15h         ; 3rd quadrant of x
tquada          equ       16h         ; 1st term of quad poly reg/ y-int
tquadb          equ       17h         ; 2nd term of quad poly reg/ slope
tquadc          equ       18h         ; 3rd term of quad poly reg
tcubed          equ       19h         ; 4th term of cubic poly reg
tquarte         equ       1ah         ; 5th term of quart poly reg
tmedx1          equ       1bh         ; x1 for med-med
tmedx2          equ       1ch         ; x2 for med-med
tmedx3          equ       1dh         ; x3 for med-med
tmedy1          equ       1eh         ; y1 for med-med
tmedy2          equ       1fh         ; y2 for med-med
tmedy3          equ       20h         ; y3 for med-med
;
tlrsqr          equ       21h         ; r^2
tbrsqr          equ       22h         ; r^2
;
terror    equ     23h       ; token in case of error in entry (ans only)
tdone     equ     24h
;
; system variable equates
;       vars with no restriction
;
tuxscl       equ  1
tuyscl       equ  2
txscl        equ  3
tyscl        equ  4

;       vars rounded to 10 digits
;
txmin        equ  0ah
txmax        equ  0bh
tymin        equ  0ch
tymax        equ  0dh
ttmin        equ  0eh
ttmax        equ  0fh
tthetamin    equ  10h
tthetamax    equ  11h
tuxmin       equ  12h
tuxmax       equ  13h
tuymin       equ  14h
tuymax       equ  15h
tuthetmin    equ  16h
tuthetmax    equ  17h
tutmin       equ  18h
tutmax       equ  19h
ttblmin      equ  1ah
tplotstart   equ  1bh
tuplotstart  equ  1ch
tnmax        equ  1dh
tunmax       equ  1eh
tnmin        equ  1fh
tunmin       equ  20h
;
;       vars cannot be 0
;
ttblstep     equ  21h
ttstep       equ  22h
tthetastep    equ  23h
tutstep      equ  24h
tuthetstep     equ  25h
;
;       special limits on these
;
tdeltax      equ  26h
tdeltay      equ  27h
;
;       special limits on these
;
txfact       equ  28h
tyfact       equ  29h
;
ttblinput       equ       2ah
;
;       no restrictions
tsimpfact       equ     2bh     ;simplification factor.
;
tplotstep       equ       34h
tuplotstep      equ       35h
;
txres           equ       36h
tuxres          equ       37h
;

;2nd byte of 2byte tokens

tlcm            equ       01h         ; immediate unary
tgcd            equ       02h         ; immediate unary
trandint        equ       03h         ; immediate unary
tsubstrng       equ       04h         ; immediate unary
tstddev         equ       05h         ; immediate unary
tvariance       equ       06h         ; immediate unary
tinstrng        equ       07h         ; immediate unary
tcoin           equ       08h         ; immediate unary
tdice           equ       09h         ; immediate unary
tremainder      equ       0ah         ; immediate unary
lastimunary      equ       0ah         ; last immediate unary token.
firstunary      equ       0bh         ; first unary operator.
tcumsum         equ       0bh         ; unary
texpr           equ       0ch         ; unary                        do autop
tlength         equ       0dh         ; unary                        do autop
tdeltalst       equ       0eh         ; unary                        do autop
tref            equ       0fh         ; unary                        no autop
autop10         equ       10h
trref           equ       10h         ; unary                        no autop
newtok12         equ    12h

autop11         equ       12h
tmattolst       equ       12h         ; command pushed               do autop
tlsttomat       equ       13h         ; command pushed               do autop
tsetconst       equ       14h         ;                                 autop
tgraphstyle     equ       15h         ; command pushed                  autop
autop12          equ       15h
tsetuplst       equ       16h         ; command opt pushed           no autop
texpron         equ       1ah         ; mode command
texproff        equ       1bh         ; mode command
tclralllst      equ       1ch         ; command not pushed
tgetcalc        equ       1dh         ; command not pushed
tdelvar         equ       1eh         ; command not pushed
tequtostrng     equ       1fh         ; command not pushed
tstrngtoequ     equ       20h         ; command not pushed
tdellast        equ       21h         ; command not pushed
tselect        equ       22h         ; command not pushed
tmodbox         equ       23h         ; icon
tnormprob       equ       24h         ; icon
tpictplot       equ       25h
tcircleplot     equ       26h
tstemplot       equ       27h
tbarplot        equ       28h
tzfit           equ       29h         ; zoom fit
tdiag_on        equ       2ah         ; dianostic display on
tdiag_off       equ       2bh         ; dianostic display off

;==========================================
;	data type equates
;===========================================
realobj         equ       0
listobj         equ       1
matobj          equ       2
equobj          equ       3
strngobj        equ       4
progobj         equ       5
protprogobj     equ       6
pictobj         equ       7
gdbobj          equ       8
unknownobj      equ       9
unknownequobj   equ       0ah
newequobj       equ       0bh
cplxobj         equ       0ch
clistobj        equ       0dh
slistobj        equ       0dh   ;string list
undefobj        equ       0eh
windowobj       equ       0fh
zstoobj         equ       10h
tblrngobj       equ       11h
lcdobj          equ       12h
backupobj       equ       13h
fracobj         equ       14h
fracrsobj       equ       14h           ;reduced   simple fraction.
fracrmobj       equ       15h           ;reduced   mixed  fraction.
fracusobj       equ       16h           ;unreduced simple fraction.
fracumobj       equ       17h           ;unreduced mixed fraction.
nameobj         equ       18h           ;name string (will fit in fplen).
appobj          equ       19h           ;application, only used in menus/link
appvarobj       equ       1ah           ;application variable
tmpconstobj     equ       1bh           ;temporary type of constant.

;======================================================================
;           i/o equates                                                          
;======================================================================
;
d0d1_bits	equ  03h
d0ld1l		equ  03h
d0ld1h		equ  01h
d0hd1l		equ  02h
d0hd1h		equ  00h
bport		equ  0			; 4-bit link port (i/o) 
;
;======================================================================
;           device codes                                                       
;======================================================================
;
;
ti82dev		equ  82h
pc82dev		equ  02h
mac82dev	equ  12h
;
ti83fdev	equ  73h
link83fdev	equ  23h
;
ti83dev		equ  83h
pc83dev		equ  03h
mac83dev	equ  13h
;
ti85dev		equ  95h      ; diff then real 85 so me talk
pc85dev		equ  05h
mac85dev	equ  15h
;
ti73dev		equ  74h      ; device x3 is always an 83
pc73dev		equ  07h
mac73dev	equ  17h
;
ti83fdev	equ  73h
link73fdev	equ  23h
pc83fdev	equ  23h

;=========================================
;	system error codes
;=========================================

e_editf		equ	7		;allow re-entering application
e_edit		equ	1<<e_editf
e_mask		equ	7fh

e_overflow	equ	1+e_edit
e_divby0	equ	2+e_edit
e_singularmat	equ	3+e_edit
e_domain	equ	4+e_edit
e_increment	equ	5+e_edit
e_break		equ	6+e_edit
e_nonreal       equ     7+e_edit
e_syntax        equ     8+e_edit
e_datatype      equ     9+e_edit
e_argument      equ     10+e_edit
e_dimmismatch   equ     11+e_edit
e_dimension     equ     12+e_edit
e_undefined     equ     13+e_edit
e_memory        equ     14+e_edit
e_invalid       equ     15+e_edit
e_illegalnest   equ     16+e_edit
e_bound         equ     17+e_edit
e_graphrange    equ     18+e_edit
e_zoom          equ     19+e_edit
e_label         equ     20
e_stat          equ     21
e_solver        equ     22+e_edit
e_singularity   equ     23+e_edit
e_signchange    equ     24+e_edit
e_iterations    equ     25+e_edit
e_badguess      equ     26+e_edit
e_statplot      equ     27
e_toltoosmall   equ     28+e_edit
e_reserved      equ     29+e_edit
e_mode          equ     30+e_edit
;
e_scale         equ     31+e_edit
;
e_lnkerr        equ     32+e_edit
e_lnkmemerr     equ     33+e_edit
e_lnktranserr   equ     34+e_edit
e_lnkduperr     equ     35+e_edit
e_lnkmemfull    equ     36+e_edit
e_unknown       equ     37
e_idnotfound    equ     38
e_nomode        equ     39+e_edit
e_validation    equ     40
e_length        equ     41+e_edit
e_application   equ     42+e_edit
e_apperr1       equ     43+e_edit
e_apperr2       equ     44+e_edit
e_expiredapp	equ	45
e_badaddr	equ	46
higerrnum	equ	46

; obsolete error numbers34              ;first link error
e_linkiochksum	equ	34
e_linkiotimeout	equ	35
e_linkiobusy	equ	36
e_linkiover	equ	37

;==============================================
;       equates to ram locations for stat vars
;==============================================
;
statn   equ     statvars
xmean   equ     statn      + fplen
sumx    equ     xmean      + fplen
sumxsqr equ     sumx       + fplen
stdx    equ     sumxsqr    + fplen
stdpx   equ     stdx       + fplen
minx    equ     stdpx      + fplen
maxx    equ     minx       + fplen
miny    equ     maxx       + fplen
maxy    equ     miny       + fplen
ymean   equ     maxy       + fplen
sumy    equ     ymean      + fplen
sumysqr equ     sumy       + fplen
stdy    equ     sumysqr    + fplen
stdpy   equ     stdy       + fplen
sumxy   equ     stdpy      + fplen
corr    equ     sumxy      + fplen
medx    equ     corr       + fplen
q1      equ     medx       + fplen
q3      equ     q1         + fplen
quada   equ     q3         + fplen
quadb   equ     quada      + fplen
quadc   equ     quadb      + fplen
cubed   equ     quadc      + fplen
quarte  equ     cubed      + fplen
medx1   equ     quarte     + fplen
medx2   equ     medx1      + fplen
medx3   equ     medx2      + fplen
medy1   equ     medx3      + fplen
medy2   equ     medy1      + fplen
medy3   equ     medy2      + fplen
slower  equ     medy3      + 2*fplen
supper  equ     medy3      + fplen

l3rd        equ              op6+4
r3rd        equ              op6+6
ridx        equ              op6
lidx        equ              op6+2
pstat   equ     medy3      + 2*fplen
zstat   equ     pstat      + fplen
tstat   equ     zstat      + fplen
chistat equ     tstat      + fplen
fstat   equ     chistat    + fplen
df      equ     fstat      + fplen
phat    equ     df         + fplen
phat1   equ     phat       + fplen
phat2   equ     phat1      + fplen
meanx1  equ     phat2      + fplen
stdx1   equ     meanx1     + fplen
statn1  equ     stdx1      + fplen
meanx2  equ     statn1     + fplen
stdx2   equ     meanx2     + fplen
statn2  equ     stdx2      + fplen
stdxp2  equ     statn2     + fplen
;slower  equ     stdxp2     + fplen
;supper  equ     slower     + fplen
sstat   equ     supper     + fplen

getcurrentpage macro
    in		a, (mempageaport)
    .endm



