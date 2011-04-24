.nolist
;TI-73 Include File
;Brandon Wilson

;TASM Compatibility
;------------------
#define equ .equ
#define EQU .EQU
#define end .end
#define END .END

;BCALL/BJUMP Macros
;------------------
#define bcall(xxxx) rst 28h \ .dw xxxx
#define bjump(xxxx) call 50h \ .dw xxxx

;Page 18h ROM Calls
;------------------------------
_LdHLind             equ 4000h
_CpHLDE              equ 4003h
_DivHLBy10           equ 4006h
_DivHLByA            equ 4009h
_GetCSC              equ 400Fh
_JForceCmdNoChar     equ 401Eh
_PutAway             equ 4039h
_AppInit             equ 4048h
_LCD_BUSY            equ 4051h
_Min                 equ 4054h
_Max                 equ 4057h
_AbsO1PAbsO2         equ 405Ah
_Intgr               equ 405Dh
_Trunc               equ 4060h
_InvSub              equ 4063h
_Times2              equ 4066h
_Plus1               equ 4069h
_Minus1              equ 406Ch
_FPSub               equ 406Fh
_FPAdd               equ 4072h
_DToR                equ 4075h
_RToD                equ 4078h
_Cube                equ 407Bh
_TimesPt5            equ 407Eh
_FPSquare            equ 4081h
_FPMult              equ 4084h
_InvOP1SC            equ 408Ah
_InvOP1S             equ 408Dh
_InvOP2S             equ 4090h
_Frac                equ 4093h
_FPRecip             equ 4096h
_FPDiv               equ 4099h
_SqRoot              equ 409Ch
_RndGuard            equ 409Fh
_RnFx                equ 40A2h
_Int                 equ 40A5h
_Round               equ 40A8h
_LnX                 equ 40ABh
_LogX                equ 40AEh
_EToX                equ 40B4h
_TenX                equ 40B7h
_SinCosRad           equ 40BAh
_Sin                 equ 40BDh
_Cos                 equ 40C0h
_Tan                 equ 40C3h
_ACosRad             equ 40C6h
_ATanRad             equ 40C9h
_ATan2Rad            equ 40CCh
_ASinRad             equ 40CFh
_ACos                equ 40D2h
_ATan                equ 40D5h
_ASin                equ 40D8h
_ATan2               equ 40DBh
_PToR                equ 40DEh
_RToP                equ 40E1h
_HLTimes9            equ 40E4h
_CkOP1Real           equ 40E7h
_CpOP4OP3            equ 40EAh
_AbsO1O2Cp           equ 40F0h
_CpOP1OP2            equ 40F3h
_OP3ToOP4            equ 40F6h
_OP1ToOP4            equ 40F9h
_OP2ToOP4            equ 40FCh
_OP4ToOP2            equ 40FFh
_OP3ToOP2            equ 4102h
_OP1ToOP3            equ 4105h
_OP5ToOP2            equ 4108h
_OP5ToOP6            equ 410Bh
_OP5ToOP4            equ 410Eh
_OP1ToOP2            equ 4111h
_OP6ToOP2            equ 4114h
_OP6ToOP1            equ 4117h
_OP4ToOP1            equ 411Ah
_OP5ToOP1            equ 411Dh
_OP3ToOP1            equ 4120h
_OP6ToOP5            equ 4123h
_OP4ToOP5            equ 4126h
_OP3ToOP5            equ 4129h
_OP2ToOP5            equ 412Ch
_OP2ToOP6            equ 412Fh
_OP1ToOP6            equ 4132h
_OP1ToOP5            equ 4135h
_OP2ToOP1            equ 4138h
_Mov10B              equ 413Eh
_Mov9B               equ 4141h
_Mov8B               equ 4147h
_Mov7B               equ 414Ah
_OP2ToOP3            equ 415Fh
_OP4ToOP3            equ 4162h
_OP5ToOP3            equ 4165h
_OP4ToOP6            equ 4168h
_Mov9ToOP1           equ 416Bh
_Mov9OP1OP2          equ 416Eh
_Mov9ToOP2           equ 4171h
_MovFrOP1            equ 4174h
_OP4Set1             equ 417Dh
_OP3Set1             equ 4180h
_OP2Set8             equ 4183h
_OP2Set5             equ 4186h
_OP2SetA             equ 4189h
_OP2Set4             equ 418Ch
_OP2Set3             equ 418Fh
_OP1Set1             equ 4192h
_OP1Set4             equ 4195h
_OP1Set3             equ 4198h
_OP3Set2             equ 419Bh
_OP1Set2             equ 419Eh
_OP2Set2             equ 41A1h
_OP2Set1             equ 41AAh
_Zero16D             equ 41B9h
_OP5Set0             equ 41C5h
_OP4Set0             equ 41C8h
_OP3Set0             equ 41CBh
_OP2Set0             equ 41CEh
_OP1Set0             equ 41D1h
_ZeroOP1             equ 41D7h
_ZeroOP2             equ 41DAh
_ZeroOP3             equ 41DDh
_ZeroOP              equ 41E0h
_ClrLp               equ 41E3h
_ShRAcc              equ 41E6h
_OP1ExOP5            equ 4219h
_OP1ExOP6            equ 421Ch
_OP1ExOP3            equ 421Fh
_OP1ExOP4            equ 4222h
_OP1ExOP2            equ 4225h
_ExLp                equ 4228h
_CkOP1C0             equ 422Bh
_CkOP1FP0            equ 422Eh
_CkOP2FP0            equ 4231h
_PosNo0Int           equ 4234h
_CkPosInt            equ 4237h
_CkInt               equ 423Ah
_CkOdd               equ 423Dh
_ExpToHex            equ 4249h
_OP1ExpToDec         equ 424Ch
_CkOP2Pos            equ 424Fh
_CkOP1Pos            equ 4252h
_ClrOP2S             equ 4255h
_ClrOP1S             equ 4258h
_DecO1Exp            equ 4261h
_CkValidNum          equ 426Ah
_HTimesL             equ 4270h
_EOP1NotReal         equ 4273h
_XName               equ 4279h
_YName               equ 427Ch
_IsA2ByteTok         equ 4291h
_Equ_or_NewEqu       equ 42AFh
_ErrD_OP1NotPos      equ 42B2h
_ErrD_OP1Not_R       equ 42B5h
_ErrD_OP1NotPosInt   equ 42B8h
_ErrD_OP1_LE_0       equ 42BBh
_ErrD_OP1_0          equ 42BEh
_Rcl_StatVar         equ 42C7h
_CkOP2Real           equ 42CAh
_MemChk              equ 42CDh
_ChkFindSym          equ 42D9h
_FindSym             equ 42DCh
_InsertMem           equ 42DFh
_EnoughMem           equ 42E5h
_CreateReal          equ 42F4h
_CreateRList         equ 42FAh
_CreateStrng         equ 4300h
_Create0Equ          equ 4303h
_CreateEqu           equ 4309h
_CreatePict          equ 430Ch
_CreateProg          equ 430Fh
_CreateAppVar        equ 4312h
_DelVar              equ 432Ah
_DelMem              equ 4330h
_DataSizeA           equ 4339h
_DataSize            equ 433Ch
_PopOP5              equ 433Fh
_PopOP3              equ 4342h
_PopOP1              equ 4345h
_PopRealO6           equ 4348h
_PopRealO5           equ 434Bh
_PopRealO4           equ 434Eh
_PopRealO3           equ 4351h
_PopRealO2           equ 4354h
_PopRealO1           equ 4357h
_PopReal             equ 435Ah
_DeallocFPS          equ 4363h
_DeallocFPS1         equ 4366h
_AllocFPS            equ 4369h
_AllocFPS1           equ 436Ch
_PushRealO6          equ 436Fh
_PushRealO5          equ 4372h
_PushRealO4          equ 4375h
_PushRealO3          equ 4378h
_PushRealO2          equ 437Bh
_PushRealO1          equ 437Eh
_PushReal            equ 4381h
_PushOP5             equ 4384h
_PushOP3             equ 4387h
_PushOP1             equ 438Ah
_Cpyto1FPS11         equ 438Dh
_CpyTo2FPS5          equ 4390h
_Cpyto1FPS5          equ 4393h
_CpyTo2FPS6          equ 4396h
_Cpyto1FPS6          equ 4399h
_CpyTo2FPS7          equ 439Ch
_Cpyto1FPS7          equ 439Fh
_Cpyto1FPS8          equ 43A2h
_CpyTo2FPS8          equ 43A5h
_Cpyto1FPS10         equ 43A8h
_Cpyto1FPS9          equ 43ABh
_CpyTo2FPS4          equ 43AEh
_CpyTo6FPS3          equ 43B1h
_CpyTo6FPS2          equ 43B4h
_CpyTo2FPS3          equ 43B7h
_Cpyto1FPS3          equ 43BDh
_Cpyto1FPS4          equ 43C3h
_CpyTo3FPS2          equ 43C6h
_CpyTo5FPST          equ 43C9h
_CpyTo6FPST          equ 43CCh
_CpyTo4FPST          equ 43CFh
_CpyTo3FPST          equ 43D2h
_CpyTo2FPST          equ 43D5h
_Cpyto1FPST          equ 43D8h
_CpyStack            equ 43DEh
_CpyTo3FPS1          equ 43E1h
_CpyTo2FPS1          equ 43E4h
_Cpyto1FPS1          equ 43E7h
_CpyTo2FPS2          equ 43EDh
_Cpyto1FPS2          equ 43F0h
_CpyO3toFPST         equ 43F6h
_CpyO2toFPST         equ 43F9h
_CpyO6toFPST         equ 43FCh
_CpyO1toFPST         equ 43FFh
_CpyToFPST           equ 4402h
_CpyToStack          equ 4405h
_CpyO3toFPS1         equ 4408h
_CpyO5toFPS1         equ 440Bh
_CpyO2toFPS1         equ 440Eh
_CpyO1toFPS1         equ 4411h
_CpyToFPS1           equ 4414h
_CpyO2toFPS2         equ 4417h
_CpyO3toFPS2         equ 441Ah
_CpyO6toFPS2         equ 441Dh
_CpyO1toFPS2         equ 4420h
_CpyToFPS2           equ 4423h
_CpyO5toFPS3         equ 4426h
_CpyO2toFPS3         equ 4429h
_CpyO1toFPS3         equ 442Ch
_CpyToFPS3           equ 442Fh
_CpyO1toFPS6         equ 4432h
_CpyO1toFPS7         equ 4435h
_CpyO1toFPS5         equ 4438h
_CpyO2toFPS4         equ 443Bh
_CpyO1toFPS4         equ 443Eh
_ErrNotEnoughMem     equ 4441h
_StrCopy             equ 444Ah
_PutMap              equ 4468h
_PutC                equ 446Bh
_DispHL              equ 446Eh
_PutS                equ 4471h
_PutPSB              equ 4474h
_PutPS               equ 4477h
_NewLine             equ 4495h
_ClrLCDFull          equ 44A7h
_ClrLCD              equ 44AAh
_ClrScrnFull         equ 44ADh
_ClrScrn             equ 44B0h
_ClrTxtShd           equ 44B3h
_EraseEOL            equ 44B9h
_VPutMap             equ 44C5h
_VPutS               equ 44C8h
_VPutSN              equ 44CBh
_RunIndicOn          equ 44D4h
_RunIndicOff         equ 44D7h
_GetTokLen           equ 44F5h
_Get_Tok_Strng       equ 44F8h
_DispEOL             equ 450Ah
_DispDone            equ 4519h
_CursorOff           equ 4522h
_CursorOn            equ 4528h
_KeyToString         equ 452Eh
_AdrMRow             equ 4567h
_AdrMEle             equ 456Ah
_GetLToOP1           equ 4573h
_PutToL              equ 4582h
_ToFrac              equ 4594h
_BinOPExec           equ 459Ah
_UnOpExec            equ 45A9h
_ThreeExec           equ 45ACh
_FourExec            equ 45B2h
_FiveExec            equ 45B5h
_CloseEditBufNoR     equ 46A2h
_ReleaseBuffer       equ 46A5h
_Load_SFont          equ 46B7h
_SFont_Len           equ 46BAh
_SetXXOP1            equ 46C0h
_SetXXOP2            equ 46C3h
_SetXXXXOP2          equ 46C6h
_UCLines             equ 46C9h
_CLine               equ 46CCh
_CLineS              equ 46CFh
_XRootY              equ 46D2h
_YtoX                equ 46D5h
_ZmStats             equ 46D8h
_StoGDB2             equ 46F0h
_RclGDB2             equ 46F3h
_CircCmd             equ 46F6h
_GrphCirc            equ 46F9h
_Mov18B              equ 46FCh
_DarkLine            equ 46FFh
_ILine               equ 4702h
_IPoint              equ 4705h
_DarkPnt             equ 4714h
_CPointS             equ 4717h
_VtoWHLDE            equ 471Dh
_XitoF               equ 4720h
_YftoI               equ 4723h
_XftoI               equ 4726h
_SetFuncM            equ 4762h
_ZmInt               equ 476Bh
_ZmDecml             equ 476Eh
_ZmPrev              equ 4771h
_ZmUsr               equ 4774h
_ZmFit               equ 477Ah
_ZmSquare            equ 477Dh
_ZmTrig              equ 4780h
_ZooDefault          equ 4786h
_GrBufCpy            equ 4789h
_RestoreDisp         equ 478Ch
_AllEq               equ 4792h
_Regraph             equ 47AAh
_PDspGrph            equ 47C2h
_HorizCmd            equ 47C5h
_VertCmd             equ 47C8h
_LineCmd             equ 47CBh
_UnLineCmd           equ 47CEh
_PointCmd            equ 47D1h
_PixelTest           equ 47D4h
_DrawCmd             equ 47DDh
_InvCmd              equ 47E3h
_CloseEditBuf        equ 47E9h
_IsEditEmpty         equ 483Ah
_BufClear            equ 4840h
_PutTokString        equ 485Eh
_CloseEditEqu        equ 4867h
_GetKey              equ 486Dh
_LCD_DRIVERON        equ 4873h
_FormEReal           equ 4885h
_FormReal            equ 488Bh
_SendVarCmd          equ 48F1h
_keyscnlnk           equ 48F4h
_DelRes              equ 48FAh
_RedimMat            equ 48FDh
_IncLstSize          equ 4900h
_InsertList          equ 4903h
_DelListEl           equ 4906h
_EditProg            equ 4909h
_CloseProg           equ 490Ch
_ClrGraphRef         equ 490Fh
_FixTempCnt          equ 4912h
_FindAlphaUp         equ 491Bh
_FindAlphaDn         equ 491Eh
_CmpSyms             equ 4921h
_CleanAll            equ 4927h
_ErrNonReal          equ 495Ah
_ParseInp            equ 4966h
_OP2Set60            equ 497Bh
_StoSysTok           equ 4987h
_StoAns              equ 498Ah
_StoY                equ 498Dh
_StoX                equ 4990h
_StoOther            equ 4993h
_RclAns              equ 4996h
_RclY                equ 4999h
_RclX                equ 499Ch
_RclVarSym           equ 499Fh
_RclSysTok           equ 49A2h
_StMatEl             equ 49A5h
_ConvOP1             equ 49ABh
_Find_Parse_Formula  equ 49B1h
_ErrOverflow         equ 49E1h
_ErrDivBy0           equ 49E4h
_ErrDomain           equ 49E7h
_ErrIncrement        equ 49EAh
_ErrNon_Real         equ 49EDh
_ErrSyntax           equ 49F0h
_ErrDataType         equ 49F9h
_ErrArgument         equ 49FFh
_ErrDimMismatch      equ 4A02h
_ErrDimension        equ 4A05h
_ErrUndefined        equ 4A08h
_ErrMemory           equ 4A0Bh
_ErrInvalid          equ 4A11h
_ErrBreak            equ 4A20h
_ErrStat             equ 4A23h
_ErrSignChange       equ 4A29h
_ErrIterations       equ 4A2Ch
_ErrBadGuess         equ 4A2Fh
_ErrTolTooSmall      equ 4A32h
_ErrStatPlot         equ 4A35h
_ErrLinkXmit         equ 4A38h
_MemClear            equ 4A47h
_MemSet              equ 4A4Ah
_JError              equ 4A4Dh
_ReloadAppEntryVecs  equ 4A50h
_PointOn             equ 4A56h
_StrLength           equ 4A5Ch
_IBounds             equ 4AA4h
_IOffset             equ 4AA7h
_DrawCirc2           equ 4AADh
_CanAlphIns          equ 4AB0h
_AdrLEle             equ 4AB6h
_GrBufClr            equ 4AD4h
_DispOP1A            equ 4AFBh
_ConvDim             equ 4B58h
_ConvDim00           equ 4B5Bh
_AnsName             equ 4B67h
_GetBaseVer          equ 4B8Eh
_AppGetCbl           equ 4B97h
_AppGetCalc          equ 4B9Ah
_Random              equ 4B9Dh
_SaveDisp            equ 4BA3h
_DisableAPI          equ 4BDFh
_DisableApd          equ 4BE8h
_EnableApd           equ 4BEBh
_JErrorNo            equ 4C03h
_ApdSetup            equ 4C06h
_RandInit            equ 4C21h
_SrchVLstUp          equ 4C33h
_SrchVLstDn          equ 4C36h
_LoadPattern         equ 4C51h
_SStringLength       equ 4C54h
_ClearRow            equ 4C8Dh
_ClearRect           equ 4CFCh
_InvertRect          equ 4CFFh
_FillRect            equ 4D02h
_DrawRectBorder      equ 4D2Ch
_EraseRectBorder     equ 4D35h
_FillRectPattern     equ 4D38h
_DrawRectBorderClear equ 4D3Bh
_IBoundsFull         equ 4D4Dh
_DisplayImage        equ 4D5Ch
_CPoint              equ 4D89h
_StoRand             equ 4E5Eh

;Page 1Fh ROM Calls
;--------------------------------
bootbtf				equ 8000h
;400Fh may point to zero-terminated version string
_MD5Final			equ 8018h
_RSAValidate			equ 801Bh
_cmpStr				equ 801Eh ;BigNumCompare
_WriteAByte			equ 8021h
_EraseFlash			equ 8024h
_FindFirstCertField             equ 8027h
_ZeroToCertificate		equ 802Ah
_GetCertificateEnd		equ 802Dh
_FindGroupedField		equ 8030h
_ret_1				equ 8033h
_ret_2				equ 8036h
_ret_3				equ 8039h
_ret_4				equ 803Ch
_ret_5				equ 803Fh
_Mult8By8			equ 8042h
_Mult16By8			equ 8045h
_Div16By8			equ 8048h
_Div16By16			equ 804Bh
;804Eh ;scary certificate reading and writing, something about calc ID and fields 0A10/0A20
_LoadAIndPaged			equ 8051h
_FlashToRam2			equ 8054h
_GetCertificateStart		equ 8057h
_GetFieldSize			equ 805Ah
_FindSubField			equ 805Dh
_EraseCertificateSector		equ 8060h
_CheckHeaderKey			equ 8063h
;8066h ;scans for certificate field 310h, subfield 600h/610h (certificate revision stuff?)
;8069h ;makes sure certificate field 0710h (in appID) doesn't exist, scans for 800h/810h (certificate repair/revision thing?)
_Load_LFontV2			equ 806Ch
_Load_LFontV			equ 806Fh
_ReceiveOS			equ 8072h
_FindOSHeaderSubField		equ 8075h
_FindNextCertField              equ 8078h
_GetByteOrBoot			equ 807Bh
_getSerial			equ 807Eh ;GetCalcSerial
_ReceiveCalcID			equ 8081h ;receives certificate replacement (including calculator ID) and writes it, requires Flash unlocked
_EraseFlashPage			equ 8084h
_WriteFlashUnsafe		equ 8087h
_dispBootVer			equ 808Ah
_MD5Init			equ 808Dh
_MD5Update			equ 8090h
_MarkOSInvalid			equ 8093h
_FindProgramLicense		equ 8096h ;copies 8010h field to appID and other insane stuff that makes zero sense
_MarkOSValid			equ 8099h
_CheckOSValidated		equ 809Ch
_SetupAppPubKey			equ 809Fh
_SigModR			equ 80A2h
_TransformHash			equ 80A5h
_IsAppFreeware			equ 80A8h
_FindAppHeaderSubField		equ 80ABh
_WriteValidationNumber		equ 80AEh ;generates two-byte validation number from calc ID and stores to certificate
_Div32By16			equ 80B1h
_FindGroup			equ 80B4h ;searches until field of DE-like group is found (DE=0A00h, it stops when it finds 0Ax0h)
_getBootVer			equ 80B7h
_getHardwareVersion		equ 80BAh
_xorA				equ 80BDh ;xor a
_bignumpowermod17		equ 80C0h
_ProdNrPart1			equ 80C3h
_WriteAByteSafe			equ 80C6h
_WriteFlash			equ 80C9h
_SetupDateStampPubKey		equ 80CCh

;System RAM Equates
;------------------------------
ramStart             equ 8000h
appData              equ 8000h
asm_exec_ram         equ 8001h
ramCode              equ 8100h
ramCodeEnd           equ 822Fh
baseAppBrTab         equ 8230h
bootTemp             equ 8241h
appSearchPage        equ 8293h
tempSwapArea         equ 8295h
appID                equ 837Dh
kbdScanCode          equ 83D8h
kbdKey               equ 83DDh
kbdGetKy             equ 83DEh
keyExtend            equ 83DFh
contrast             equ 83E0h
curTime              equ 83E1h
curRow               equ 83E2h
curCol               equ 83E3h
curOffset            equ 83E4h
curUnder             equ 83E5h
ioFlag               equ 83E6h
sndRecState          equ 83E8h
ioErrState           equ 83E9h
header               equ 83EAh
ioData               equ 83F3h
bakHeader            equ 83FFh
OP1                  equ 842Bh
OP1M                 equ 842Dh
OP2                  equ 8436h
OP2EXT               equ 843Fh
OP3                  equ 8441h
OP3M                 equ 8443h
OP4                  equ 844Ch
OP4M                 equ 844Eh
OP5                  equ 8457h
OP5M                 equ 8459h
OP6                  equ 8462h
OP6M                 equ 8464h
OP6EXT               equ 846Bh
progToEdit           equ 8472h
nameBuff             equ 847Ah
equ_edit_save        equ 8485h
OP2M                 equ 8485h
iMathPtr1            equ 8486h
iMathPtr2            equ 8488h
iMathPtr3            equ 848Ah
iMathPtr4            equ 848Ch
iMathPtr5            equ 848Eh
chkDelPtr1           equ 8490h
chkDelPtr2           equ 8492h
insDelPtr            equ 8494h
upDownPtr            equ 8496h
curY                 equ 8498h
curType              equ 8499h
curXRow              equ 849Ah
prevDData            equ 849Bh
lFont_record         equ 84A3h
sFont_record         equ 84ABh
menuActive           equ 84B3h
menuAppDepth         equ 84B4h
MenuCurrent          equ 84B5h
ProgCurrent          equ 84C1h
rclQueue             equ 84D2h
rclQueueEnd          equ 84D4h
indicMem             equ 84D6h
indicCounter         equ 84DEh
indicBusy            equ 84DFh
apdSubTimer          equ 84E0h
apdTimer             equ 84E1h
cxMain               equ 84E2h
cxPPutAway           equ 84E4h
cxPutAway            equ 84E6h
cxRedisp             equ 84E8h
cxErrorEP            equ 84EAh
cxSizeWind           equ 84ECh
cxPage               equ 84EEh
cxCurApp             equ 84EFh
cxPrev               equ 84F0h
monQH                equ 850Eh
monQT                equ 850Fh
monQueue             equ 8510h
onSP                 equ 8520h
ramReturnData        equ 8529h
errNo                equ 854Ah
errSP                equ 854Bh
errOffset            equ 854Dh
bstCounter           equ 8559h
flags                equ 855Bh
tokVarPtr            equ 85A1h
tokLen               equ 85A3h
fOutDat              equ 85A5h
asm_data_ptr1        equ 85AFh
asm_data_ptr2        equ 85B1h
asm_sym_ptr1         equ 85B3h
asm_sym_ptr2         equ 85B5h
asm_ind_call         equ 85B7h
appRawKeyHandle      equ 85B8h
appBank_jump         equ 85C9h
appPage              equ 85CBh
customHeight         equ 8612h
appBackUpScreen      equ 8613h
textShadow           equ 8913h
textShadCur          equ 8993h
textShadTop          equ 8995h
textShadAlph         equ 8996h
textShadIns          equ 8997h
appInfo              equ 8998h
promptRow            equ 89A2h
promptCol            equ 89A3h
promptIns            equ 89A4h
promptShift          equ 89A5h
promptRet            equ 89A6h
promptValid          equ 89A8h
promptTop            equ 89AAh
promptCursor         equ 89ACh
promptTail           equ 89AEh
promptBtm            equ 89B0h
varType              equ 89B2h
varCurrent           equ 89B3h
varClass             equ 89BBh
penCol               equ 89DFh
penRow               equ 89E0h
userMenuSA           equ 89E1h
ioPrompt             equ 8A41h
RectFillPHeight      equ 8A42h
dImageWidth          equ 8A42h
RectFillPWidth       equ 8A43h
RectFillPattern      equ 8A44h
saveSScreen          equ 8A52h
statVars             equ 8D58h
curGStyle            equ 8E94h
curGY                equ 8E95h
curGX                equ 8E96h
curGY2               equ 8E97h
curGX2               equ 8E98h
freeSaveY            equ 8E99h
freeSaveX            equ 8E9Ah
XOffset              equ 8EC8h
YOffset              equ 8EC9h
lcdTallP             equ 8ECAh
pixWideP             equ 8ECBh
pixWide_m_1          equ 8ECCh
pixWide_m_2          equ 8ECDh
lastEntryPTR         equ 8ECEh
lastEntryStk         equ 8ED0h
numLastEntries       equ 90D0h
currLastEntry        equ 90D1h
curInc               equ 92DEh
uXmin                equ 92F5h
uXmax                equ 92FEh
uXscl                equ 9307h
uYmin                equ 9310h
uYmax                equ 9319h
uYscl                equ 9322h
uThetMin             equ 932Bh
uThetMax             equ 9334h
uThetStep            equ 933Dh
uTmin                equ 9346h
uTmax                equ 934Fh
uTStep               equ 9358h
uPlotStart           equ 9361h
unMax                equ 936Ah
uu0                  equ 9373h
uv0                  equ 937Ch
unMin                equ 9385h
uu02                 equ 938Eh
uv02                 equ 9397h
uw0                  equ 93A0h
uPlotStep            equ 93A9h
uXres                equ 93B2h
uw02                 equ 93BBh
Xmin                 equ 93C7h
Xmax                 equ 93D0h
Xscl                 equ 93D9h
Ymin                 equ 93E2h
Ymax                 equ 93EBh
Yscl                 equ 93F4h
ThetaMin             equ 93FDh
ThetaMax             equ 9406h
ThetaStep            equ 940Fh
TminPar              equ 9418h
TmaxPar              equ 9421h
Tstep                equ 942Ah
PlotStart            equ 9433h
nMax                 equ 943Ch
u0                   equ 9445h
v0                   equ 944Eh
nMin                 equ 9457h
u02                  equ 9460h
v02                  equ 9469h
w0                   equ 9472h
PlotStep             equ 947Bh
XresO                equ 9484h
w02                  equ 948Dh
smallEditRAM         equ 949Fh
XFact                equ 950Bh
YFact                equ 9514h
Xres_int             equ 951Dh
deltaX               equ 951Eh
deltaY               equ 9527h
shortX               equ 9530h
shortY               equ 9539h
lower                equ 9542h
upper                equ 954Bh
XOutSym              equ 9558h
XOutDat              equ 955Ah
YOutSym              equ 955Ch
YOutDat              equ 955Eh
inputSym             equ 9560h
inputDat             equ 9562h
prevData             equ 9564h
TblMin               equ 9676h
TblStep              equ 967Fh
P1Type		     equ 968ch
SavX1List	     equ 968dh
SavY1List	     equ 9692h
SavF1List	     equ 9697h
P1FrqOnOff	     equ 96abh
P2Type		     equ 96aeh
SavX2List	     equ 96afh
SavY2List	     equ 96b4h
SavF2List	     equ 96b9h
P2FrqOnOff	     equ 96cdh
P3Type		     equ 96d0h	
SavX3List	     equ 96d1h
SavY3List	     equ 96d6h
SavF3List	     equ 96d8h
P3FrqOnOff	     equ 96efh
plotSScreen          equ 9737h
seed1                equ 9A37h
seed2                equ 9A40h
cmdCursor            equ 9A7Eh
editTop              equ 9A82h
editCursor           equ 9A84h
editTail             equ 9A86h
editBtm              equ 9A88h
editSym              equ 9A94h
editDat              equ 9A96h
winTop               equ 9B33h
winBtm               equ 9B34h
winLeftEdge          equ 9B35h
winLeft              equ 9B36h
winAbove             equ 9B38h
winRow               equ 9B3Ah
winCol               equ 9B3Ch
fmtDigits            equ 9B3Eh
fmtString            equ 9B3Fh
fmtConv              equ 9B80h
fmtLeft              equ 9B92h
fmtIndex             equ 9B94h
fmtMatSym            equ 9B96h
fmtMatMem            equ 9B98h
EQS                  equ 9B9Ah
tSymPtr1             equ 9BA6h
tSymPtr2             equ 9BA8h
chkDelPtr3           equ 9BAAh
chkDelPtr4           equ 9BACh
tempMem              equ 9BAEh
fpBase               equ 9BB0h
FPS                  equ 9BB2h
OPBase               equ 9BB4h
OPS                  equ 9BB6h
pTempCnt             equ 9BB8h
cleanTmp             equ 9BBAh
pTemp                equ 9BBCh
progPtr              equ 9BBEh
newDataPtr           equ 9BC0h
appErr1              equ 9BC2h
appErr2              equ 9BCFh
userMem              equ 9C37h
symTable             equ 0FE66h

;Equates for Fraction Entry Routines
;-----------------------------------
fieldMax             equ OP5
fieldLen             equ fieldMax+1
fieldCol             equ fieldLen+1    ;pencol of start
fieldRow             equ fieldCol+1    ;penrow of start, 3 rows above char
fieldInput           equ fieldRow+1    ;9 bytes
wholeMax             equ tempSwapArea
wholeLen             equ wholeMax+1
wholeCol             equ wholeLen+1    ;pencol of start
wholeRow             equ wholeCol+1    ;penrow of start, 3 rows above char
wholeInput           equ wholeRow+1    ;9 bytes

numerMax             equ wholeInput+9
numerLen             equ numerMax+1
numerCol             equ numerLen+1    ;pencol of start
numerRow             equ numerCol+1    ;penrow of start
numerInput           equ numerRow+1    ;9 bytes

denomMax             equ numerInput+9
denomLen             equ denomMax+1
denomCol             equ denomLen+1    ;pencol of start
denomRow             equ denomCol+1    ;penrow of start
denomInput           equ denomRow+1    ;9 bytes

fracLineLen          equ denomInput+9
fracLineCol          equ fracLineLen+1

fracRightEdge        equ fracLineCol+1

curPosition          equ fracRightEdge+1

numerSpaces          equ curPosition+1
denomSpaces          equ numerSpaces+1
haveFrac             equ 3
haveUnit             equ 4
incCurPos            equ 5

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

;========== run indicators

busyPause	equ	10101010b
busynNormal	equ	11110000b

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

.list