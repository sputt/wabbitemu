;-----------------------------------------------------------------------------
;TI-83 Include File
;-----------------------------------------------------------------------------
; 06/27/05
;-----------------------------------------------------------------------------
;Brandon Wilson
;-----------------------------------------------------------------------------
__bank_call             equ     0000h
__bank_entry            equ     0000h
__bank_ret              equ     0000h
__bank_jump             equ     0000h

;ROM Calls
;------------------------------------
_ldHLind                equ     4000h
_cphlde                 equ     4004h
_divHLby10              equ     4008h
_divHLbyA               equ     400Ch
_kbdScan                equ     4010h
_getcsc                 equ     4014h
_coorMon                equ     4018h
_mon                    equ     401Bh
_monForceKey            equ     401Eh
_sendKPress             equ     4021h
_JforceCmdNoChar        equ     4024h
_JforceCmd              equ     4027h
_sysErrHandler          equ     402Ah
_newContext             equ     402Eh
_newContext0            equ     4032h
_A2POINTHLIND           equ     4036h
_POINTHLIND             equ     403Ah
_PPutAwayPrompt         equ     403Eh
_PPutAway               equ     4042h
_PutAway                equ     4046h
_SizeWind               equ     404Ah
_ErrorEP                equ     404Eh
_callMain               equ     4052h
_monErrHand             equ     4056h
_appInit                equ     405Ah
_appInitIfDec           equ     405Eh
_initialize             equ     4062h
_lcd_busy               equ     4066h
_MIN                    equ     406Ah
_MAX                    equ     406Eh
_ABSO1PABSO2            equ     4072h
_INTGR                  equ     4076h
_TRUNC                  equ     407Ah
_Invsub                 equ     407Eh
_Times2                 equ     4082h
_Plus1                  equ     4086h
_Minus1                 equ     408Ah
_FPsub                  equ     408Eh
_FPadd                  equ     4092h
_Dtor                   equ     4096h
_RTOD                   equ     409Ah
_Cube                   equ     409Eh
_Timespt5               equ     40A2h
_FPsquare               equ     40A6h
_FPmult                 equ     40AAh
_LJRND                  equ     40AEh
_Invop1sc               equ     40B2h
_Invop1s                equ     40B6h
_Invop2s                equ     40BAh
_FRAC                   equ     40BEh
_FPrecip                equ     40C2h
_FPDIV                  equ     40C6h
_SQROOT                 equ     40CAh
_RNDGUARD               equ     40CEh
_RNFX                   equ     40D2h
_Int                    equ     40D6h
_ROUND                  equ     40DAh
_LNX                    equ     40DEh
_Logx                   equ     40E2h
_LJNORND                equ     40E6h
_ETOX                   equ     40EAh
_TENX                   equ     40EEh
_SINCOSRAD              equ     40F2h
_SIN                    equ     40F6h
_Cos                    equ     40FAh
_Tan                    equ     40FEh
_SINHCOSH               equ     4102h
_Tanh                   equ     4106h
_Cosh                   equ     410Ah
_Sinh                   equ     410Eh
_ACOSRAD                equ     4112h
_ATANRAD                equ     4116h
_ATAN2RAD               equ     411Ah
_ASINRAD                equ     411Eh
_ACOS                   equ     4122h
_ATAN                   equ     4126h
_ASIN                   equ     412Ah
_ATAN2                  equ     412Eh
_Atanh                  equ     4132h
_ASINH                  equ     4136h
_Acosh                  equ     413Ah
_Ptor                   equ     413Eh
_Rtop                   equ     4142h
_HLTIMES9               equ     4146h
_CKOP1CPLX              equ     414Ah
_CKOP1REAL              equ     414Eh
_Angle                  equ     4152h
_COP1SET0               equ     4156h
_CPOP4OP3               equ     415Ah
_MOV9OP2CP              equ     415Eh
_ABSO1O2CP              equ     4162h
_Cpop1op2               equ     4166h
_Op3tooP4               equ     416Ah
_Op1tooP4               equ     416Eh
_Op2tooP4               equ     4172h
_OP4TOOP2               equ     4176h
_OP3TOOP2               equ     417Ah
_OP1TOOP3               equ     417Eh
_Op5tooP2               equ     4182h
_OP5TOOP6               equ     4186h
_OP5TOOP4               equ     418Ah
_Op1tooP2               equ     418Eh
_OP6TOOP2               equ     4192h
_OP6TOOP1               equ     4196h
_Op4tooP1               equ     419Ah
_OP5TOOP1               equ     419Eh
_Op3tooP1               equ     41A2h
_OP6TOOP5               equ     41A6h
_Op4tooP5               equ     41AAh
_Op3tooP5               equ     41AEh
_OP2TOOP5               equ     41B2h
_Op2tooP6               equ     41B6h
_OP1TOOP6               equ     41BAh
_OP1TOOP5               equ     41BEh
_OP2TOOP1               equ     41C2h
_MOVREG                 equ     41C6h
_MOV10B                 equ     41CAh
_MOV9B                  equ     41CEh
_MOV18                  equ     41D2h
_MOV8B                  equ     41D6h
_MOV7B                  equ     41DAh
_MOV14                  equ     41DEh
_MOV6B                  equ     41E2h
_MOV5B                  equ     41E6h
_MOV4B                  equ     41EAh
_MOV3B                  equ     41EEh
_MOV2B                  equ     41F2h
_Op2tooP3               equ     41F6h
_OP4TOOP3               equ     41FAh
_OP5TOOP3               equ     41FEh
_OP4TOOP6               equ     4202h
_MOV9TOOP1              equ     4206h
_MOV9OP1OP2             equ     420Ah
_MOV9TOOP2              equ     420Eh
_MOVFROP1               equ     4212h
_Op4set1                equ     4216h
_OP3SET1                equ     421Ah
_OP2SET8                equ     421Eh
_Op2set5                equ     4222h
_OP2SETA                equ     4226h
_Op2set4                equ     422Ah
_Op2set3                equ     422Eh
_OP1SET1                equ     4232h
_Op1set4                equ     4236h
_Op1set3                equ     423Ah
_Op3set2                equ     423Eh
_Op1set2                equ     4242h
_Op2set2                equ     4246h
_SETNUM2                equ     424Ah
_SETMANT1               equ     424Eh
_Op2set1                equ     4252h
_SETNUM1                equ     4256h
_SETNUM                 equ     425Ah
_SETNUMA                equ     425Eh
_SETMANT                equ     4262h
_ZERO16D                equ     4266h
_SET16A                 equ     426Ah
_SET14A                 equ     426Eh
_SET14D                 equ     4272h
_OP5SET0                equ     4276h
_OP4SET0                equ     427Ah
_OP3SET0                equ     427Eh
_Op2set0                equ     4282h
_Op1set0                equ     4286h
_SETNUM0                equ     428Ah
_zeroop1                equ     428Eh
_ZEROOP2                equ     4292h
_ZEROOP3                equ     4296h
_ZEROOP                 equ     429Ah
_CLRLP                  equ     429Eh
_SHRACC                 equ     42A2h
_SHLACC                 equ     42A6h
_shlacc                 equ     42A6h
_SHR18                  equ     42AAh
_SHR18A                 equ     42AEh
_SHR16                  equ     42B2h
_SHR14                  equ     42B6h
_SHL16                  equ     42BAh
_SHL14                  equ     42BEh
_SRDO1                  equ     42C2h
_SHRDRND                equ     42C6h
_MANTPA                 equ     42CAh
_ADDPROP                equ     42CEh
_ADDPROPLP              equ     42D2h
_ADD16D                 equ     42D6h
_ADD14D                 equ     42DAh
_SUB16D                 equ     42DEh
_SUB14D                 equ     42E2h
_Op2exOp6               equ     42E6h
_Op5exOp6               equ     42EAh
_Op1exop5               equ     42EEh
_Op1exop6               equ     42F2h
_Op2exOp4               equ     42F6h
_OP2EXOP5               equ     42FAh
_OP1EXOP3               equ     42FEh
_Op1exop4               equ     4302h
_OP1EXOP2               equ     4306h
_EXLP                   equ     430Ah
_CKOP1C0                equ     430Eh
_CKOP1FP0               equ     4312h
_CKOP2FP0               equ     4316h
_POSNO0INT              equ     431Ah
_CKPOSINT               equ     431Eh
_Ckint                  equ     4322h
_CKODD                  equ     4326h
_CKOP1M                 equ     432Ah
_GETCON1                equ     432Eh
_GETCON                 equ     4332h
_PIDIV2                 equ     4336h
_PIDIV4                 equ     433Ah
_TWOPI                  equ     433Eh
_PICON                  equ     4342h
_PIDIV4A                equ     4346h
_PIDIV2A                equ     434Ah
_EXPTOHEX               equ     434Eh
_OP1EXPTODEC            equ     4352h
_CKOP2POS               equ     4356h
_Ckop1pos               equ     435Ah
_CLROP2S                equ     435Eh
_CLROP1S                equ     4362h
_FDIV100                equ     4366h
_FDIV10                 equ     436Ah
_DECO1EXP               equ     436Eh
_INCO1EXP               equ     4372h
_INCEXP                 equ     4376h
_CKVALIDNUM             equ     437Ah
_GETEXP                 equ     437Eh
_HtimesL                equ     4382h
_EOP1NOTREAL            equ     4386h
_THETANAME              equ     438Ah
_RNAME                  equ     438Eh
_REGEQNAME              equ     4392h
_RECURNNAME             equ     4396h
_XNAME                  equ     439Ah
_YNAME                  equ     439Eh
_TNAME                  equ     43A2h
_REALNAME               equ     43A6h
_SETesTOfps             equ     43AAh
_CHKTEMPDIRT            equ     43AEh
_OP1MOP2EXP             equ     43B2h
_OP1EXPMDE              equ     43B6h
_CHKERRBREAK            equ     43BAh
_ISA2BYTETOK            equ     43BEh
_GETLASTENTRY           equ     43C2h
_GETLASTENTRYPTR        equ     43C6h
_REGRCLRCHNG            equ     43CAh
_RESETWINTOP            equ     43CEh
_SETYUP                 equ     43D2h
_SETXUP                 equ     43D6h
_ISO1NONTLSTorPROG      equ     43DAh
_ISO1NONTEMPLST         equ     43DEh
_IS_A_LSTorCLST         equ     43E2h
_CHK_HL_999             equ     43E6h
_equ_or_NEWequ          equ     43EAh
_GET_PLUS1_SAVE         equ     43EEh
_ERRD_OP1NOTPOS         equ     43F2h
_ERRD_OP1NOT_R          equ     43F6h
_ERRD_OP1NOTPOSINT      equ     43FAh
_ERRD_OP1_LE_0          equ     43FEh
_ERRD_OP1_0             equ     4402h
_FINDSYM_GET_SIZE       equ     4406h
_STO_STATVAR            equ     440Ah
_RCL_STATVAR            equ     440Eh
_Ckop2real              equ     4412h
_GET_X_INDIRECT         equ     4416h
_MEMCHK                 equ     441Ah
_CMPPRGNAMLEN1          equ     441Eh
_CMPPRGNAMLEN           equ     4422h
_FINDPROGSYM            equ     4426h
_Chkfindsym             equ     442Ah
_FINDSYM                equ     442Eh
_INSERTMEM              equ     4432h
_INSERTMEMA             equ     4436h
_ENOUGHMEM              equ     443Ah
_CMPMEMNEED             equ     443Eh
_CREATEPVAR4            equ     4442h
_CREATEPVAR3            equ     4446h
_CREATEVAR3             equ     444Ah
_CREATECPLX             equ     444Eh
_CREATEREAL             equ     4452h
_CREATETRLIST           equ     4456h
_CREATERLIST            equ     445Ah
_CREATETCLIST           equ     445Eh
_CREATECLIST            equ     4462h
_CREATETRMAT            equ     4466h
_CREATERMAT             equ     446Ah
_CREATETSTRNG           equ     446Eh
_CREATESTRNG            equ     4472h
_CREATE0equ             equ     4476h
_CREATETequ             equ     447Ah
_CREATEequ              equ     447Eh
_CREATEPICT             equ     4482h
_CREATEGDB              equ     4486h
_CREATEPROG             equ     448Ah
_CHKDEL                 equ     448Eh
_CHKDELA                equ     4492h
_ADJPARSER              equ     4496h
_ADJMATH                equ     449Ah
_ADJM7                  equ     449Eh
_DELMEMA                equ     44A2h
_GET_FORM_NUM           equ     44A6h
_DELVAR                 equ     44AAh
_DELVARIO               equ     44AEh
_DELMEM                 equ     44B2h
_DELVAR3D               equ     44B6h
_DELVAR3C               equ     44BAh
_DELVAR3DC              equ     44BEh
_SYM_PROG_NON_T_LST     equ     44C2h
_ADJSYMPTRS             equ     44C6h
_DATASIZEA              equ     44CAh
_DATASIZE               equ     44CEh
_POPMCPLXO1             equ     44D2h
_POPMCPLX               equ     44D6h
_MOVCPLX                equ     44DAh
_POPOP5                 equ     44DEh
_POPOP3                 equ     44E2h
_POPOP1                 equ     44E6h
_POPREALO6              equ     44EAh
_POPREALO5              equ     44EEh
_POPREALO4              equ     44F2h
_POPREALO3              equ     44F6h
_POPREALO2              equ     44FAh
_POPREALO1              equ     44FEh
_POPREAL                equ     4502h
_FPOPCPLX               equ     4506h
_FPOPREAL               equ     450Ah
_FPOPFPS                equ     450Eh
_DEALLOCFPS             equ     4512h
_DEALLOCFPS1            equ     4516h
_ALLOCFPS               equ     451Ah
_ALLOCFPS1              equ     451Eh
_PUSHREALO6             equ     4522h
_PUSHREALO5             equ     4526h
_PUSHREALO4             equ     452Ah
_PUSHREALO3             equ     452Eh
_PUSHREALO2             equ     4532h
_PUSHREALO1             equ     4536h
_PUSHREAL               equ     453Ah
_PUSHOP5                equ     453Eh
_PUSHOP3                equ     4542h
_PUSHMCPLXO3            equ     4546h
_PUSHOP1                equ     454Ah
_PUSHMCPLXO1            equ     454Eh
_PUSHMCPLX              equ     4552h
_EXMCPLXO1              equ     4556h
_EXCH9                  equ     455Ah
_CPYTO1FPS11            equ     455Eh
_CPYTO2FPS5             equ     4562h
_CPYTO1FPS5             equ     4566h
_CPYTO2FPS6             equ     456Ah
_CPYTO1FPS6             equ     456Eh
_CPYTO2FPS7             equ     4572h
_CPYTO1FPS7             equ     4576h
_CPYTO1FPS8             equ     457Ah
_CPYTO2FPS8             equ     457Eh
_CPYTO1FPS10            equ     4582h
_CPYTO1FPS9             equ     4586h
_CPYTO2FPS4             equ     458Ah
_CPYTO6FPS3             equ     458Eh
_CPYTO6FPS2             equ     4592h
_CPYTO2FPS3             equ     4596h
_CPYCTO1FPS3            equ     459Ah
_CPYTO1FPS3             equ     459Eh
_CPYFPS3                equ     45A2h
_CPYTO1FPS4             equ     45A6h
_CPYTO3FPS2             equ     45AAh
_CPYTO5FPST             equ     45AEh
_CPYTO6FPST             equ     45B2h
_CPYTO4FPST             equ     45B6h
_CPYTO3FPST             equ     45BAh
_CPYTO2FPST             equ     45BEh
_CPYTO1FPST             equ     45C2h
_CPYFPST                equ     45C6h
_CPYSTACK               equ     45CAh
_CPYTO3FPS1             equ     45CEh
_CPYTO2FPS1             equ     45D2h
_CPYTO1FPS1             equ     45D6h
_CPYFPS1                equ     45DAh
_CPYTO2FPS2             equ     45DEh
_CPYTO1FPS2             equ     45E2h
_CPYFPS2                equ     45E6h
_CPYO3TOFPST            equ     45EAh
_CPYO2TOFPST            equ     45EEh
_CPYO6TOFPST            equ     45F2h
_CPYO1TOFPST            equ     45F6h
_CPYTOFPST              equ     45FAh
_CPYTOSTACK             equ     45FEh
_CPYO3TOFPS1            equ     4602h
_CPYO5TOFPS1            equ     4606h
_CPYO2TOFPS1            equ     460Ah
_CPYO1TOFPS1            equ     460Eh
_CPYTOFPS1              equ     4612h
_CPYO2TOFPS2            equ     4616h
_CPYO3TOFPS2            equ     461Ah
_CPYO6TOFPS2            equ     461Eh
_CPYO1TOFPS2            equ     4622h
_CPYTOFPS2              equ     4626h
_CPYO5TOFPS3            equ     462Ah
_CPYO2TOFPS3            equ     462Eh
_CPYO1TOFPS3            equ     4632h
_CPYTOFPS3              equ     4636h
_CPYO1TOFPS6            equ     463Ah
_CPYO1TOFPS7            equ     463Eh
_CPYO1TOFPS5            equ     4642h
_CPYO2TOFPS4            equ     4646h
_CPYO1TOFPS4            equ     464Ah
_ERRNOTENOUGHMEM        equ     464Eh
_FPSMINUS9              equ     4652h
_HLMINUS9               equ     4656h
_ERROVERFLOW            equ     465Ah
_ERRDIVBY0              equ     465Dh
_errSingularMat         equ     4660h
_ERRDOMAIN              equ     4663h
_errIncrement           equ     4666h
_ERRNON_REAL            equ     4669h
_errSyntax              equ     466Ch
_ERRDATATYPE            equ     466Fh
_errArgument            equ     4672h
_errDimMismatch         equ     4675h
_ERRDIMENSION           equ     4678h
_ERRUNDEFINED           equ     467Bh
_ERRMEMORY              equ     467Eh
_ERRINVALID             equ     4681h
_ERRBREAK               equ     4684h
_errStat                equ     4687h
_errSignChange          equ     468Ah
_errIterations          equ     468Dh
_errBadGuess            equ     4690h
_errTolTooSmall         equ     4693h
_ERRSTATPLOT            equ     4696h
_errLinkXmit            equ     4699h
_JError                 equ     469Ch
_JErrorNo               equ     469Fh
_noErrorEntry           equ     46A2h
_pushErrorHandlER       equ     46A5h
_popErrorHandleR        equ     46A9h
_strCopy                equ     46ADh
_strCat                 equ     46B1h
_isInSet                equ     46B5h
_sDone                  equ     46B9h
_serrort                equ     46BDh
_sNameEq                equ     46C1h
_sUnderScr              equ     46C5h
_sFAIL                  equ     46C9h
_sName                  equ     46CDh
_sOK                    equ     46D1h
_seqn                   equ     46D5h
_Sselect                equ     46D9h
_STransmit              equ     46DDh
_SRECURN                equ     46E1h
_GEQNAMEA               equ     46E5h
_RECNAME                equ     46E9h
___bank_call            equ     46EDh
___bank_ret             equ     46F1h
___bank_jump            equ     46F5h
___bank_entry           equ     46F9h
_READDISP2              equ     46FDh
_putmap                 equ     4701h
_putc                   equ     4705h
_dispHL                 equ     4709h
_puts                   equ     470Dh
_putpsb                 equ     4711h
_putps                  equ     4715h
_wputps                 equ     4719h
_putbuf                 equ     471Dh
_putbuf1                equ     4721h
_wputc                  equ     4725h
_wputs                  equ     4729h
_wputsEOL               equ     472Dh
_wdispEOL               equ     4731h
_whomeUp                equ     4735h
_setNumWindow           equ     4739h
_newline                equ     473Dh
_moveDown               equ     4741h
_scrollUp               equ     4745h
_shrinkWindow           equ     4749h
_moveUp                 equ     474Dh
_scrollDown             equ     4751h
_clrlcdfull             equ     4755h
_clrLCD                 equ     4759h
_clrscrnfull            equ     475Dh
_clrScrn                equ     4761h
_clrtxtshd              equ     4765h
_clrWindow              equ     4769h
_eraseeol               equ     476Dh
_eraseEOW               equ     4771h
_homeUp                 equ     4775h
_getcurloc              equ     4779h
_vputmap                equ     477Dh
_vputs                  equ     4781h
_vputsn                 equ     4785h
_vputsnG                equ     4789h
_vputsnT                equ     478Dh
_runIndicOn             equ     4791h
_runIndicOff            equ     4795h
_saveCmdShadow          equ     4799h
_saveShadow             equ     479Dh
_rstrShadow             equ     47A1h
_rstrpartial            equ     47A5h
_rstrCurRow             equ     47A9h
_rstrUnderMenu          equ     47ADh
_rstrbotrow             equ     47B1h
_saveTR                 equ     47B5h
_restoreTR              equ     47B9h
_GETKEYPRESS            equ     47BDh
_GETTOKLEN              equ     47C1h
_GET_TOK_STRNG          equ     47C5h
_GETTOKSTRING           equ     47C9h
_PUTBPATBUF2            equ     47CDh
_PUTBPATBUF             equ     47D1h
_putbPAT                equ     47D5h
_putcCheckScrolL        equ     47D9h
_dispEOL                equ     47DDh
_fdispEOL               equ     47E1h
_MAKEROWCMD             equ     47E5h
_TOTOSTRP               equ     47E9h
_SETVARNAME             equ     47EDh
_dispDone               equ     47F1h
_dispDone               equ     47F1h
_finishoutput           equ     47F5h
_curBlink               equ     47F9h
_CURSOROFF              equ     47FDh
_hideCursor             equ     4801h
_cursorOn               equ     4805h
_showCursor             equ     4809h
_KEYTOSTRING            equ     480Dh
_PULLDOWNCHK            equ     4811h
_MenuCatCommon          equ     4815h
_ZIfCatalog             equ     4819h
_ZIfMatrixMenu          equ     481Dh
_loadCurCat             equ     4821h
_NCifprgmedmode         equ     4825h
_LoadMenuNum            equ     4829h
_LoadMenuNumL           equ     482Dh
_MenuEdKey              equ     4831h
_MenCatRet              equ     4835h
_notalphnum             equ     4839h
_SaveSavedFlags         equ     483Dh
_SetMenuFlags           equ     4841h
_RstrSomeFlags          equ     4845h
_RstrSmallText          equ     4849h
_dispListName           equ     484Dh
_SeeIfErrorCx           equ     4851h
_RstrOScreen            equ     4855h
_SaveOscreen            equ     4859h
_DispLAlphaName         equ     485Dh
_AbortPrgmode           equ     4861h
_IS_FULLCNTX            equ     4865h
_ADRMROW                equ     4869h
_ADRMELE                equ     486Dh
_GETMATOP1A             equ     4871h
_GETM1TOOP1             equ     4875h
_GETM1TOP1A             equ     4879h
_GETMTOOP1              equ     487Dh
_PUTTOM1A               equ     4881h
_PUTTOMA1               equ     4885h
_PUTTOMAT               equ     4889h
_MAT_EL_DIV             equ     488Dh
_CMATFUN                equ     4891h
_ROWECH_POLY            equ     4895h
_ROWECHELON             equ     4899h
_ADRLELE                equ     489Dh
_GETL1TOOP1             equ     48A1h
_GETL1TOP1A             equ     48A5h
_Getltoop1              equ     48A9h
_GETL1TOOP2             equ     48ADh
_GETL1TOP2A             equ     48B1h
_GETL2TOP1A             equ     48B5h
_PUTTOLA1               equ     48B9h
_PUTTOL                 equ     48BDh
_MAXMINLST              equ     48C1h
_LLOW                   equ     48C5h
_LHIGH                  equ     48C9h
_LSUM                   equ     48CDh
_CUMSUM                 equ     48D1h
_TOFRAC                 equ     48D5h
_SEQSET                 equ     48D9h
_SEQSOLVE               equ     48DDh
_CMP_NUM_INIT           equ     48E1h
_BINOPEXEC              equ     48E5h
_EXMEAN1                equ     48E9h
_SET2MVLPTRS            equ     48EDh
_SETMAT1                equ     48F1h
_CREATETLIST            equ     48F5h
_UNOPEXEC               equ     48F9h
_THREEEXEC              equ     48FDh
_RESTOREERRNO           equ     4901h
_FOUREXEC               equ     4904h
_FIVEEXEC               equ     4908h
_CPYTO2ES1              equ     490Ch
_CPYTO6ES1              equ     4910h
_CPYTO1ES1              equ     4914h
_CPYTO3ES1              equ     4918h
_CPYTO3ES2              equ     491Ch
_CPYTO2ES2              equ     4920h
_CPYTO1ES2              equ     4924h
_CPYTO2ES3              equ     4928h
_CPYTO1ES3              equ     492Ch
_CPYTO3ES4              equ     4930h
_CPYTO6ES3              equ     4934h
_CPYTO2ES4              equ     4938h
_CPYTO1ES4              equ     493Ch
_CPYTO2ES5              equ     4940h
_CPYTO1ES5              equ     4944h
_CPYTO4EST              equ     4948h
_CPYTO2EST              equ     494Ch
_CPYTO1EST              equ     4950h
_CPYTO2ES6              equ     4954h
_CPYTO1ES6              equ     4958h
_CPYTO2ES7              equ     495Ch
_CPYTO1ES7              equ     4960h
_CPYTO2ES8              equ     4964h
_CPYTO1ES8              equ     4968h
_CPYTO1ES9              equ     496Ch
_CPYTO2ES9              equ     4970h
_CPYTO2ES10             equ     4974h
_CPYTO1ES10             equ     4978h
_CPYTO2ES11             equ     497Ch
_CPYTO1ES11             equ     4980h
_CPYTO2ES12             equ     4984h
_CPYTO1ES12             equ     4988h
_CPYTO2ES13             equ     498Ch
_CPYTO1ES13             equ     4990h
_CPYTO1ES14             equ     4994h
_CPYTO1ES16             equ     4998h
_CPYTO1ES17             equ     499Ch
_CPYTO1ES18             equ     49A0h
_CPYTO1ES15             equ     49A4h
_CPYTO2ES15             equ     49A8h
_CPYO1TOEST             equ     49ACh
_CPYO1TOES1             equ     49B0h
_CPYO6TOES1             equ     49B4h
_CPYO6TOES3             equ     49B8h
_CPYO1TOES2             equ     49BCh
_CPYO2TOES2             equ     49C0h
_CPYO1TOES3             equ     49C4h
_CPYO1TOES4             equ     49C8h
_CPYO1TOES5             equ     49CCh
_CPYO1TOES6             equ     49D0h
_CPYO1TOES7             equ     49D4h
_CPYO2TOES4             equ     49D8h
_CPYO2TOES5             equ     49DCh
_CPYO2TOES6             equ     49E0h
_CPYO2TOES7             equ     49E4h
_CPYO2TOES8             equ     49E8h
_CPYO2TOES9             equ     49ECh
_CPYO1TOES8             equ     49F0h
_CPYO1TOES9             equ     49F4h
_CPYO1TOES10            equ     49F8h
_CPYO1TOES11            equ     49FCh
_CPYO1TOES12            equ     4A00h
_CPYO1TOES13            equ     4A04h
_CPYO1TOES14            equ     4A08h
_CPYO1TOES15            equ     4A0Ch
_EVALF3A                equ     4A10h
COMPLEX_EXEC            equ     4A14h
_GetK                   equ     4A18h
_setTitlE               equ     4A1Ch
_dispVarVal             equ     4A20h
_RecallEd               equ     4A24h
_setupBuffer            equ     4A28h
_createNumEditBUF       equ     4A2Ch
_CallCommon             equ     4A30h
_CommonKeys             equ     4A34h
_Leftmore               equ     4A37h
_fDel                   equ     4A3Ah
_fClear                 equ     4A3Eh
_finsDisp               equ     4A42h
_FinsDisp02             equ     4A45h
_setIndicator           equ     4A49h
_CloseEditBufNo         equ     4A4Dh
_releaseBuffer          equ     4A51h
_varnameToOP1hl         equ     4A55h
_nameToOP1              equ     4A59h
_numPPutAway            equ     4A5Dh
_numRedisp              equ     4A61h
_numError02             equ     4A65h
_LOAD_SFONT             equ     4A68h
_SFONT_LEN              equ     4A6Ch
_InitNumVec             equ     4A70h
_SETXXOP1               equ     4A74h
_SETXXOP2               equ     4A78h
_setXXXXOp2             equ     4A7Ch
_UCLINES                equ     4A80h
_CLine                  equ     4A84h
_CLineS                 equ     4A88h
_XROOTY                 equ     4A8Ch
_YTOX                   equ     4A90h
_ZMSTATS                equ     4A94h
_POINT_STAT_HLP         equ     4A98h
_DRAWSPLOT              equ     4A9Ch
_INITNEWTRACEP          equ     4AA0h
_SPLOTCOORD             equ     4AA4h
_SPLOTRIGHT             equ     4AA8h
_SPLOTLEFT              equ     4AACh
_CMPBOXINFO             equ     4AB0h
_NEXTPLOT               equ     4AB4h
_PREVPLOT               equ     4AB8h
_CLRPREVPLOT            equ     4ABCh
_PUT_INDEX_LST          equ     4AC0h
_GET_INDEX_LST          equ     4AC4h
_HEAP_SORT              equ     4AC8h
_STOGDB2                equ     4ACCh
_RCLGDB2                equ     4AD0h
_CIRCCMD                equ     4AD4h
_GRPHCIRC               equ     4AD8h
_MOV18B                 equ     4ADCh
_DARKLINE               equ     4AE0h
_ILINE                  equ     4AE4h
_IPOINT                 equ     4AE8h
_XYRNDBOTH              equ     4AECh
_XYRND                  equ     4AF0h
_CheckTOP               equ     4AF4h
_CheckXY                equ     4AF8h
_DARKPNT                equ     4AFCh
_CPointS                equ     4B00h
_WTOV                   equ     4B04h
_VtoWHLDE               equ     4B08h
_XItoF                  equ     4B0Ch
_YFTOI                  equ     4B10h
_XFTOI                  equ     4B14h
_TraceOff               equ     4B18h
_GrRedisp               equ     4B1Ch
_gdisptoken             equ     4B20h
_GRDECODA               equ     4B24h
_LABCOOR                equ     4B28h
_COORDISP               equ     4B2Ch
_TMPequNOSRC            equ     4B30h
_GRLABELS               equ     4B34h
_YPIXSET                equ     4B38h
_XPIXSET                equ     4B3Ch
_COPYRNG                equ     4B40h
_VALCUR                 equ     4B44h
_GRPUTAWAY              equ     4B48h
_RSTGFLAGS              equ     4B4Ch
_GRReset                equ     4B50h
_XYCENT                 equ     4B54h
_ZOOMXYCMD              equ     4B58h
_CPTDELY                equ     4B5Ch
_CPTDELX                equ     4B60h
_SETFUNCM               equ     4B64h
_SETSEQM                equ     4B68h
_SETPOLM                equ     4B6Ch
_SETPARM                equ     4B70h
_ZMINT                  equ     4B74h
_ZMDECML                equ     4B78h
_ZMPREV                 equ     4B7Ch
_ZMUSR                  equ     4B80h
_SETUZM                 equ     4B84h
_ZMFIT                  equ     4B88h
_ZMSQUARE               equ     4B8Ch
_ZMTRIG                 equ     4B90h
_SetXMinMax             equ     4B94h
_ZooDefault             equ     4B98h
_GRBUFCPY               equ     4B9Ch
_DRAWSPLITLINE          equ     4BA0h
_RestoreDisp            equ     4BA4h
_FNDDB                  equ     4BA8h
_ALLEQ                  equ     4BACh
_fndallseleq            equ     4BB0h
_NEXTEQ                 equ     4BB4h
_PREVEQ                 equ     4BB8h
_BLINKGCUR              equ     4BBCh
_NBCURSOR               equ     4BC0h
_STATMARK               equ     4BC4h
_CHKTEXTCURS            equ     4BC8h
_Regraph                equ     4BCCh
_DOREFFLAGS02           equ     4BD0h
_INITNSEQ               equ     4BD4h
_YRES                   equ     4BD8h
_PLOTPTXY2              equ     4BDCh
_Ceiling                equ     4BE0h
_PutXY                  equ     4BE4h
_PUTequNO               equ     4BE7h
_PDspGrph               equ     4BEBh
_PDspGrph               equ     4BEBh
_Horizcmd               equ     4BEFh
_VERTCMD                equ     4BF3h
_LINECMD                equ     4BF7h
_UNLINECMD              equ     4BFBh
_POINTCMD               equ     4BFFh
_PIXELTEST              equ     4C03h
_PIXELCMD               equ     4C07h
_TANLNF                 equ     4C0Bh
_DRAWCMD_INIT           equ     4C0Fh
_DRAWCMD                equ     4C13h
_SHADECMD               equ     4C17h
_INVCMD                 equ     4C1Bh
_STATSHADE              equ     4C1Fh
_dspmattable            equ     4C23h
_dsplsts                equ     4C27h
_closeEditBuf           equ     4C2Bh
_parseEditBuf           equ     4C2Fh
_putsm                  equ     4C33h
_DspCurTbl              equ     4C37h
_DSPGRTBL               equ     4C3Bh
_ZeroTemplate           equ     4C3Fh
_settblrefs             equ     4C43h
_dispTblBot             equ     4C47h
_DispTblTop             equ     4C4Bh
_dispTblbody            equ     4C4Fh
_vputblank              equ     4C53h
_VPUTBLANK              equ     4C53h
_TBLTRACE               equ     4C57h
_dispListNameY          equ     4C5Bh
_CurNameLength          equ     4C5Fh
_NameToBuf              equ     4C63h
_jpromptcursor          equ     4C67h
_bufLeft                equ     4C6Ah
_BUFRIGHT               equ     4C6Eh
_bufInsert              equ     4C72h
_bufQueueChar           equ     4C76h
_bufReplace             equ     4C7Ah
_bufDelete              equ     4C7Eh
_BUFPEEK                equ     4C82h
_BUFPEEK1               equ     4C86h
_BUFPEEK2               equ     4C8Ah
_BUFPEEK3               equ     4C8Eh
_BufToBtm               equ     4C92h
_setupEditEqu           equ     4C96h
_bufToTop               equ     4C9Ah
_isEditFull             equ     4C9Eh
_isEditEmpty            equ     4CA2h
_ISATTOP                equ     4CA6h
_isAtBtm                equ     4CAAh
_bufClear               equ     4CAEh
_JcursorFirst           equ     4CB2h
_JcursorLast            equ     4CB5h
_cursorLeft             equ     4CB8h
_JcursorRight           equ     4CBCh
_JcursorUp              equ     4CBFh
_cursorDown             equ     4CC2h
_cursorToOffset         equ     4CC6h
_insDisp                equ     4CCAh
_FDISPBOL1              equ     4CCEh
_FDISPBOL               equ     4CD2h
_dispEOW                equ     4CD6h
_dispHead               equ     4CDAh
_dispTail               equ     4CDEh
_PutTokString           equ     4CE2h
_setupEditCmd           equ     4CE6h
_setEmptyEditEqU        equ     4CEAh
_setEmptyEditPtR        equ     4CEEh
_closeEditEqu           equ     4CF2h
_toggleIns              equ     4CF6h
_GETPREVTOK             equ     4CFAh
_getkey                 equ     4CFEh
_getkey                 equ     4CFEh
_canIndic               equ     4D02h
_LCD_DRIVERON           equ     4D06h
_DFMIN2                 equ     4D0Ah
_formDisp               equ     4D0Eh
_formMatrix             equ     4D12h
_wscrollLeft            equ     4D16h
_wscrollUp              equ     4D1Ah
_wscrollDown            equ     4D1Eh
_wscrollRight           equ     4D22h
_formEReal              equ     4D26h
_formERealTOK           equ     4D2Ah
_formdcplx              equ     4D2Eh
_formReal               equ     4D32h
_formScrollUp           equ     4D36h
_setwinabove            equ     4D3Ah
_disarmScroll           equ     4D3Eh
_OP1toEdit              equ     4D42h
_MinToEdit              equ     4D46h
_rclVarToEdit           equ     4D4Ah
_rclVarToEditPtR        equ     4D4Eh
_RCLENTRYTOEDIT         equ     4D52h
_rclToQueue             equ     4D56h
_FORMTOTOK              equ     4D5Ah
_DISP_INTERVAL          equ     4D5Eh
_DisplstName            equ     4D62h
_dispSLstNameHL         equ     4D66h
_EditEqu                equ     4D6Ah
_closeEquField          equ     4D6Eh
_AutoSelect             equ     4D72h
_DISPYEOS               equ     4D76h
_dispNumEOS             equ     4D7Ah
_setupdispeq            equ     4D7Eh
_dispForward            equ     4D82h
_DispYPrompt2           equ     4D86h
_stringwidth            equ     4D8Ah
_dispErrorScreeN        equ     4D8Eh
_POPCX                  equ     4D92h
_loadnoeentry           equ     4D96h
_SaveScreen             equ     4D9Ah
_RETSCREEN              equ     4D9Eh
_RetScreenErr           equ     4DA2h
_SplitChange            equ     4DA6h
_SolveRedisp            equ     4DAAh
_SolveDisp              equ     4DAEh
_itemName               equ     4DB2h
_SETNORM_VALS           equ     4DB6h
_SETVERT_VALS           equ     4DBAh
_CONVKEYTOTOK           equ     4DBEh
_KEYTOTOKNEW2B          equ     4DC2h
_KEYTOTOK2BYTE          equ     4DC6h
_TOKTOKEY               equ     4DCAh
_LOAD_LFONT             equ     4DCEh
_SEND1BERR              equ     4DD2h
_GETVARCMD              equ     4DD6h
_SENDVARCMD             equ     4DDAh
_PRINTSCREEN            equ     4DDEh
_KEYSCNLNK              equ     4DE2h
_IORESETALL             equ     4DE6h
_DELRES                 equ     4DEAh
_CONVLCTOLR             equ     4DEEh
_REDIMMAT               equ     4DF2h
_INCLSTSIZE             equ     4DF6h
_insertlist             equ     4DFAh
_dellistel              equ     4DFEh
_EDITPROG               equ     4E02h
_CLOSEPROG              equ     4E06h
_CLRGRAPHREF            equ     4E0Ah
_FIXTEMPCNT             equ     4E0Eh
_SAVEDATA               equ     4E12h
_RESTOREDATA            equ     4E16h
_FindAlphaup            equ     4E1Ah
_FindAlphadn            equ     4E1Eh
_CMPSYMS                equ     4E22h
_CREATETEMP             equ     4E26h
_CLEANALL               equ     4E2Ah
_MOVETONEXTSYM          equ     4E2Eh
_CONVLRTOLC             equ     4E32h
_TblScreenDn            equ     4E36h
_TblScreenUp            equ     4E3Ah
_SCREENUP               equ     4E3Eh
_SCREENUPDOWN           equ     4E42h
_ZifRclHandler          equ     4E46h
_zifrclkapp             equ     4E4Ah
_rclkeyRtn              equ     4E4Eh
_RCLKEY                 equ     4E52h
_RCLREGEQ_CALL          equ     4E55h
_RCLREGEQ               equ     4E59h
_initNamePrompt         equ     4E5Ch
_NamePrompt2            equ     4E60h
_CATALOGCHK             equ     4E64h
_clrTR                  equ     4E68h
_QUAD                   equ     4E6Ch
_GRAPHQUAD              equ     4E70h
_BC2NONREAL             equ     4E74h
_ERRNONREAL             equ     4E78h
_WRITE_TEXT             equ     4E7Ch
_FORSEQINIT             equ     4E80h
_GRPHPARS               equ     4E84h
_PLOTPARS               equ     4E88h
_PARSEinp               equ     4E8Ch
_PARSEOFF               equ     4E90h
_PARSESCAN              equ     4E94h
_GETPARSE               equ     4E98h
_SAVEPARSE              equ     4E9Ch
_InitPFlgs              equ     4EA0h
_CKENDLINERR            equ     4EA4h
_Op2set60               equ     4EA8h
_GETSTATPTR             equ     4EACh
_CMP_STATPTR            equ     4EB0h
_VARSYSADR              equ     4EB4h
_Stosystok              equ     4EB8h
_STOANS                 equ     4EBCh
_STOTHETA               equ     4EC0h
_STOR                   equ     4EC4h
_Stoy                   equ     4EC8h
_STON                   equ     4ECCh
_STOT                   equ     4ED0h
_Stox                   equ     4ED4h
_Stoother               equ     4ED8h
_RCLANS                 equ     4EDCh
_Rcly                   equ     4EE0h
_RCLN                   equ     4EE4h
_Rclx                   equ     4EE8h
_Rclvarsym              equ     4EECh
_Rclsystok              equ     4EF0h
_StMatEl                equ     4EF4h
_STLSTVECEL             equ     4EF8h
_Convop1                equ     4EFCh
_isaletter              equ     4F00h
_Find_Parse_ForMULA     equ     4F04h
_PARSE_FORMULA          equ     4F08h
_STRNGENT1              equ     4F0Ch
_GETNUMLET              equ     4F15h
_INIT_PROG_LIST         equ     4F1Eh
_PRGRDLP                equ     4F27h
_VARENT                 equ     4F30h
_RCLSTAT                equ     4F39h
_PARSELOOP              equ     4F42h
_PARSEONC               equ     4F4Bh
_PARSEON                equ     4F52h
_PARSECMD               equ     4F5Ah
_STOTYPE                equ     4F61h
_CREATEPAIR             equ     4F6Ah
_PUSHNUM                equ     4F73h
_INCCURPCERREND         equ     4F7Ch
_ERREND                 equ     4F85h
_COMMAERRF              equ     4F8Eh
_COMMAERR               equ     4F97h
_STEQARG2               equ     4FA0h
_STEQARG                equ     4FA9h
_INPARG                 equ     4FB2h
_STEQARG3               equ     4FBBh
_NXTFETCH               equ     4FC4h
_CKFETCHVAR             equ     4FCDh
_FETCHVARA              equ     4FD6h
PARSER_EXEC             equ     4FDFh
_FETCHVAR               equ     4FE3h
_CKENDLIN               equ     4FECh
_CKENDEXP               equ     4FF5h
_CKPARSEND              equ     4FFEh
_STOTYPEARG             equ     5007h
_CONVDIM                equ     5010h
_CONVDIM00              equ     5019h
_AHEADequAL             equ     5022h
_PARSAHEADS             equ     502Bh
_PARSAHEAD              equ     5034h
_ANSNAME                equ     503Dh
_STOCMPREALS            equ     5046h
_GETDEPTR               equ     504Fh
_PUSH2BOPER             equ     5058h
_POP2BOPER              equ     5061h
_PUSHOPER               equ     506Ah
_POPOPER                equ     5073h
_FIND_E_UNDEF           equ     507Ch
_STTMPEQ                equ     5085h
_FINDEOL                equ     508Eh
_BRKINC                 equ     5097h
_INCFETCH               equ     50A0h
_CURFETCH               equ     50A9h
PGMIO_EXEC              equ     50B2h
_Random                 equ     50B6h
_Storand                equ     50BAh
_RandInit               equ     50BEh
_resetStacks            equ     50C2h
_FACTORIAL              equ     50C6h
_YONOFF                 equ     50CAh
_EQSELUNSEL             equ     50CEh
_ITSOLVER               equ     50D2h
_GRITSOLVER             equ     50D6h
_ITSOLVERB              equ     50DAh
_ITSOLVERNB             equ     50DEh
_ExTest_INT             equ     50E2h
_DIST_FUN               equ     50E6h
_LOGGAMMA               equ     50EAh
_ONEVAR                 equ     50EEh
_ONEVARS_0              equ     50F2h
_ORDSTAT                equ     50F6h
_INITSTATANS2           equ     50FAh
_ANOVA_SPEC             equ     50FEh
_EXEC_ASSEMBLY          equ     5102h
_outputexpr             equ     5106h
_CentCursor             equ     510Ah
_TEXT                   equ     5113h
_FINISHSPEC             equ     511Ch
_TRCYFUNC               equ     5125h
_RCL_SEQ_X              equ     512Eh
_RCLSEQ2                equ     5137h
_GRPPutAway             equ     5140h
_CKVALDELX              equ     5149h
_CKVALDELTA             equ     5152h
_Grbufclr               equ     515Bh
_Grbufcpy_v             equ     5164h
_FNDSELEQ               equ     516Dh
_CLRGRAPHXY             equ     5176h
_NEXT_Y_STYLE           equ     517Fh
GRAPH_EXEC              equ     5188h
_PLOTPT                 equ     518Ch
_NEWINDEP               equ     5195h
_Axes                   equ     519Eh
_setPenX                equ     51A7h
_setPenY                equ     51B0h
_setPenT                equ     51B9h
_TAN_equ_DISP           equ     51C2h
_putAns                 equ     51CBh
_DISPOP1A               equ     51D4h
_MATHTANLN              equ     51DDh
_ENDDRAW                equ     51E6h
IO_EXEC                 equ     51EFh
EXECUTE_Z80             equ     52E5h

;RAM Equates
;------------------------------------
curRow                  equ     800Ch
curCol                  equ     800Dh
OP1                     equ     8039h
op1+1                   equ     803Ah
op1M                    equ     803Bh
OP2                     equ     8044h
op2exp                  equ     8045h
op2m                    equ     8046H
op3                     equ     804Fh
op3exP                  equ     8050h
OP3M                    equ     8051h
OP3EXT                  equ     8058h
OP4                     equ     805Ah
OP4EXP                  equ     805Bh
OP4M                    equ     805Ch
OP4EXT                  equ     8063h
op5                     equ     8065h
OP5EXP                  equ     8066h
op5M                    equ     8067h
OP5EXT                  equ     806Eh
OP6                     equ     8070h
op6exP                  equ     8071h
OP6M                    equ     8072h
op6exT                  equ     8079h
Imathptr1               equ     8094h
Imathptr2               equ     8096h
IMATHPTR3               equ     8098h
Imathptr4               equ     809Ah
IMATHPTR5               equ     809Ch
ASM_IND_CALL            equ     80C8h
asm_ind_call            equ     80C8h
TEXTSHADOW              equ     80C9h
PENCOL                  equ     8252h
PENROW                  equ     8253h
savesscreen             equ     8265h
usermem_offset          equ     8565h
statvars                equ     858Fh
CURGY                   equ     886Dh
CURGX                   equ     886Eh
PLOTSSCREEN             equ     8E29h
cmdshadow               equ     9157h
progPtr                 equ     9319h
symTable                equ     FE6Eh

;Address of LCD Driver Instruction Port
;--------------------------------------
LCDINSTPORT		equ	10h

;Screen Shadows
;------------------------------------
TEXTSHADOW		equ	80C9h
CMDSHADOW		equ	9157h

;Graph Backup Buffer
;------------------------------------
PLOTSSCREEN		equ	8E29h

;Safe RAM Locations
;------------------------------------
SAVESSCREEN		equ	8265h
STATVARS		equ	858Fh

;Indirect Call Byte
;------------------------------------
ASM_IND_CALL		equ	80C8h

;Link Port Write Equates
;------------------------------------
D0LD1L			equ	0C3h
D0LD1H			equ	0C1h
D0HD1L			equ	0C2h
D0HD1H			equ	0C0h
BPORT			equ	0

;System Flag Equates
;------------------------------------
trigflags		equ	0
trigdeg			equ	2
;
plotflags		equ	2
plotloc			equ	1
plotdisp		equ	2
;
grfmodeflags		equ	2
grffuncm		equ	4
grfpolarm		equ	5
grfparamm		equ	6
grfrecurm		equ	7
;
graphflags		equ	3
graphdraw		equ	0
graphcursor		equ	2
;
grfdbflags		equ	4
grfdot			equ	0
grfsimul		equ	1
grfgrid			equ	2
grfpolar		equ	3
grfnocoord		equ	4
grfnoaxis		equ	5
grflabel		equ	6
;
textflags		equ	5
textEraseBelow		equ	1
textScrolled		equ	2
textInverse		equ	3
;
onflags			equ	9
onRunning		equ	3
onInterrupt		equ	4
;
statflags		equ	9
statsvalid		equ	6
;
fmtflags		equ	10
fmtExponent		equ	0
fmtEng			equ	1
;
nummode			equ	10
FMTREAL			equ	5
FMTRECT			equ	6
FMTPOLAR		equ	7
;
curflags		equ	12
curAble			equ	2
curOn			equ	3
curLock			equ	4
;
appflags		equ	13
appTextSave		equ	1
appAutoScroll		equ	2
;
PLOTFLAG2		equ	17
EXPR_PARAM		equ	3
EXPR_WRITING		equ	4
;
indicflags		equ	18
indicRun		equ	0
indicOnly		equ	2
;
shiftflags		equ	18
shift2nd		equ	3
shiftAlpha		equ	4
shiftALock		equ	6
;
tblflags		equ	19
AutoFill		equ	4
AutoCalc		equ	5
;
sgrflags		equ	20
grfSplit		equ	0
VertSplit		equ	1
WRITE_ON_GRAPH		equ	4
textwrite		equ	7
;
asm_flag1		equ	33
asm_flag1_0		equ	0
asm_flag1_1		equ	1
asm_flag1_2		equ	2
asm_flag1_3		equ	3
asm_flag1_4		equ	4
asm_flag1_5		equ	5
asm_flag1_6		equ	6
asm_flag1_7		equ	7
;
asm_flag2		equ	34
asm_flag2_0		equ	0
asm_flag2_1		equ	1
asm_flag2_2		equ	2
asm_flag2_3		equ	3
asm_flag2_4		equ	4
asm_flag2_5		equ	5
asm_flag2_6		equ	6
asm_flag2_7		equ	7
;
asm_flag3		equ	35
asm_flag3_0		equ	0
asm_flag3_1		equ	1
asm_flag3_2		equ	2
asm_flag3_3		equ	3
asm_flag3_4		equ	4
asm_flag3_5		equ	5
asm_flag3_6		equ	6
asm_flag3_7		equ	7

;System Var Equates, For Storing and Recalling Them
;--------------------------------------------------
XSCLt			equ	2 ;XSCALE
YSCLt			equ	3 ;YSCALE
XMINt			equ	0Ah ;XMIN
XMAXt			equ	0Bh ;XMAX
YMINt			equ	0Ch ;YMIN
YMAXt			equ	0Dh ;XMAX
TMINt			equ	0Eh ;TMIN
TMAXt			equ	0Fh ;TMAX
THETMINt		equ	10h ;THETA MIN
THETMAXt		equ	11h ;THETA MAX
TBLMINt			equ	1Ah ;TABLE MIN
PLOTSTARTt		equ	1Bh ;PLOT START
NMAXt			equ	1Dh ;NMAX
nmint			equ	1Fh ;NMIN
TBLSTEPt		equ	21h ;TABLE STEP
TSTEPt			equ	22h ;T STEP
THETSTEPt		equ	23h ;THETA STEP
DELTAXt			equ	26h ;DELTA X
DELTAYt			equ	27h ;DELTA Y
XFACTt			equ	28h ;X ZOOM FACTOR
YFACTt			equ	29h ;Y ZOOM FACTOR
FINnT			equ	2Bh ;TVM n
FINiT			equ	2Ch ;TVM i
FINpvT			equ	2Dh ;TVM pv
FINpmtT			equ	2Eh ;TVM pmt
FINfvT			equ	2Fh ;TVM fv
FINpyT			equ	30h ;TVM p/y
FINcyT			equ	31h ;TVM c/y
PLOTSTEPT		equ	34h ;PLOT STEP
XREST			equ	36h ;X RES

;Character Font Equates
;-----------------------------------
LSEQ_N			equ	001h
LSEQ_U			equ	002h
LSEQ_V			equ	003h
LSEQ_W			equ	004h
Lconvert		equ	005h
LsqUp			equ	006h
LsqDown			equ	007h
Lintegral		equ	008h
Lcross			equ	009h
LBOXICON		equ	00Ah
LCROSSICON		equ	00Bh
LDOTICON		equ	00Ch
LsubT			equ	00Dh ;small capital T for parametric mode.
LcubeR			equ	00Eh ;slightly different 3 for cubed root.
LhexF			equ	00Fh
Lroot			equ	010h
Linverse		equ	011h
Lsquare			equ	012h
Langle			equ	013h
Ldegree			equ	014h
Lradian			equ	015h
Ltranspose		equ	016h
LLE			equ	017h
LNE                     equ     018h
LGE                     equ     019h
Lneg                    equ     01Ah
Lexponent               equ     01Bh
Lstore                  equ     01Ch
Lten                    equ     01Dh
LupArrow                equ     01Eh
LdownArrow              equ     01Fh
Lspace                  equ     020h
Lexclam                 equ     021h
Lquote                  equ     022h
Lpound                  equ     023h
LFOURTH                 equ     024h
Lpercent                equ     025h
Lampersand              equ     026h
Lapostrophe             equ     027h
LlParen                 equ     028h
LrParen                 equ     029h
Lasterisk               equ     02Ah
LplusSign               equ     02Bh
Lcomma                  equ     02Ch
Ldash                   equ     02Dh
Lperiod                 equ     02Eh
Lslash                  equ     02Fh
L0                      equ     030h
L1                      equ     031h
L2                      equ     032h
L3                      equ     033h
L4                      equ     034h
L5                      equ     035h
L6                      equ     036h
L7                      equ     037h
L8                      equ     038h
L9                      equ     039h
Lcolon                  equ     03Ah
Lsemicolon              equ     03Bh
LLT                     equ     03Ch
LEQ                     equ     03Dh
LGT                     equ     03Eh
Lquestion               equ     03Fh
LatSign                 equ     040h
LcapA                   equ     041h
LcapB                   equ     042h
LcapC                   equ     043h
LcapD                   equ     044h
LcapE                   equ     045h
LcapF 	        	equ     046h
LcapG   	        equ     047h
LcapH			equ     048h
LcapI			equ     049h
LcapJ			equ     04Ah
LcapK			equ     04Bh
LcapL			equ     04Ch
LcapM			equ     04Dh
LcapN			equ     04Eh
LcapO			equ     04Fh
LcapP			equ     050h
LcapQ			equ     051h
LcapR			equ     052h
LcapS			equ     053h
LcapT			equ     054h
LcapU			equ     055h
LcapV			equ     056h
LcapW			equ     057h
LcapX			equ     058h
LcapY			equ     059h
LcapZ			equ     05Ah
Ltheta			equ     05Bh
Lbackslash		equ     05Ch
LrBrack			equ     05Dh
Lcaret			equ     05Eh
Lunderscore		equ     05Fh
Lbackquote		equ     060h
La			equ     061h
Lb			equ     062h
Lc			equ     063h
Ld			equ     064h
Le			equ     065h
Lf			equ     066h
Lg			equ     067h
Lh			equ     068h
Li			equ     069h
Lj			equ     06Ah
Lk			equ     06Bh
Ll			equ     06Ch
Lm			equ     06Dh
Ln			equ     06Eh
Lo			equ     06Fh
Lp			equ     070h
Lq			equ     071h
Lr			equ     072h
Ls			equ     073h
Lt			equ     074h
Lu			equ     075h
Lv			equ     076h
Lw			equ     077h
Lx			equ     078h
Ly			equ     079h
Lz			equ     07Ah
LlBrace			equ     07Bh
Lbar			equ     07Ch
LrBrace			equ     07Dh
Ltilde			equ     07Eh
LinvEQ			equ     07Fh
Lsub0			equ     080h
Lsub1			equ     081h
Lsub2			equ     082h
Lsub3			equ     083h
Lsub4			equ     084h
Lsub5			equ     085h
Lsub6			equ     086h
Lsub7			equ     087h
Lsub8			equ     088h
Lsub9			equ     089h
LcapAAcute		equ     08Ah
LcapAGrave		equ     08Bh
LcapACaret		equ     08Ch
LcapADier		equ     08Dh
LaAcute			equ     08Eh
LaGrave			equ     08Fh
LaCaret			equ     090h
LaDier			equ     091h
LcapEAcute		equ     092h
LcapEGrave		equ     093h
LcapECaret		equ     094h
LcapEDier		equ     095h
LeAcute			equ     096h
LeGrave			equ     097h
LeCaret			equ     098h
LeDier			equ     099h
LcapIAcute		equ     09Ah
LcapIGrave		equ     09Bh
LcapICaret		equ     09Ch
LcapIDier		equ     09Dh
LiAcute			equ     09Eh
LiGrave			equ     09Fh
LiCaret			equ     0A0h
LiDier			equ     0A1h
LcapOAcute		equ     0A2h
LcapOGrave		equ     0A3h
LcapOCaret		equ     0A4h
LcapODier		equ     0A5h
LoAcute			equ     0A6h
LoGrave			equ     0A7h
LoCaret			equ	0A8h
LoDier			equ     0A9h
LcapUAcute		equ     0AAh
LcapUGrave		equ     0ABh
LcapUCaret		equ     0ACh
LcapUDier		equ     0ADh
LuAcute			equ     0AEh
LuGrave			equ     0AFh
LuCaret			equ     0B0h
LuDier			equ     0B1h
LcapCCed		equ     0B2h
LcCed			equ     0B3h
LcapNTilde		equ     0B4h
LnTilde			equ     0B5h
Laccent			equ     0B6h
Lgrave			equ     0B7h
Ldieresis		equ     0B8h
LquesDown		equ     0B9h
LexclamDown		equ     0BAh
Lalpha			equ     0BBh
Lbeta			equ     0BCh
Lgamma			equ     0BDh
LcapDelta		equ     0BEh
Ldelta			equ     0BFh
Lepsilon		equ     0C0h
LlBrack			equ     0C1h
Llambda			equ     0C2h
Lmu			equ     0C3h
Lpi			equ     0C4h
Lrho			equ     0C5h
LcapSigma		equ     0C6h
Lsigma			equ     0C7h
Ltau			equ     0C8h
Lphi			equ     0C9h
LcapOmega		equ     0CAh
LxMean			equ     0CBh
LyMean			equ     0CCh
LsupX			equ     0CDh
Lellipsis		equ     0CEh
Lleft			equ     0CFh
Lblock			equ     0D0h
Lper			equ     0D1h
Lhyphen			equ     0D2h
Larea			equ     0D3h
Ltemp			equ     0D4h
Lcube			equ     0D5h
Lenter			equ     0D6h
LimagI			equ     0D7h
Lphat			equ     0D8h
Lchi			equ     0D9h
LstatF			equ     0DAh
Llne			equ     0DBh
LlistL			equ     0DCh
LfinanN			equ     0DDh

;# Tokens in Use = 249
;--------------------------------
;length (+80h for leading blank)
;(+40h for trailing blank)
EOSSTART		equ	0

;Display Conversions Come Immediately Before 'tstore'
;----------------------------------------------------
DCONV			equ	01h
;
TTODMS			equ	DCONV ;01h
TTODEC			equ	DCONV+1 ;02h
TTOAbc			equ	DCONV+2 ;03h > A b/c
;
TSTORE			equ	DCONV+3 ;04h Lstore 01
;
TBOXPLOT		equ	05h
;
BRACKS			equ	06h
;
tLBrack			equ	BRACKS ;06h '['
tRBrack			equ	BRACKS+1 ;07h ']'
tLBrace			equ	BRACKS+2 ;08h '{'
tRBrace			equ	BRACKS+3 ; 09h '}'
;
TPOST1			equ	BRACKS+4
;
tFromRad		equ	TPOST1 ;0Ah Lradian
tFromDeg		equ	TPOST1+1 ;0Bh Ldegree
tRecip			equ	TPOST1+2 ;0Ch Linverse
tSqr			equ	TPOST1+3 ;0Dh Lsquare
tTrnspos		equ	TPOST1+4 ;0Eh Ltranspose
TCUBE			equ	TPOST1+5 ;0Fh '^3'
;
tLParen			equ	10h ;10h '('
tRParen			equ	11h ;11h ')'

;Immediate Unary Tokens Go From Here To 'EOSEL-1'
;--------------------------------------------------------
IMUN			equ	12h
;
tRound			equ	IMUN ;12h 'round'
TPXTST			equ	IMUN+1 ;13h 'PXL-TEST'
tAug			equ	IMUN+2 ;14h 'aug'
tRowSwap		equ	IMUN+3 ;15h 'rSwap'
tRowPlus		equ	IMUN+4 ;16h 'rAdd'
tmRow			equ	IMUN+5 ;17h 'multR'
tmRowPlus		equ	IMUN+6 ;18h 'mRAdd'
tMax			equ	IMUN+7 ;19h 'max'
tMin			equ	IMUN+8 ;1Ah 'min'
TRTOPr			equ	IMUN+9 ;1Bh 'R>Pr
TRTOPo			equ	IMUN+10 ;1Ch 'R>Po
TPTORx			equ	IMUN+11 ;1Dh 'P>Rx
TPTORy			equ	IMUN+12 ;1Eh 'P>Ry
tMEDIAN			equ	IMUN+13 ;1Fh 'MEDIAN
tRandM			equ	IMUN+14 ;20h 'randM'
TMEAN			equ	IMUN+15 ;21h
TROOT			equ	IMUN+16 ;22h 'ROOT'
tSeries			equ	IMUN+17 ;23h 'seq'
tFnInt			equ	IMUN+18 ;24h 'fnInt'
tNDeriv			equ	IMUN+19 ;25h 'fnIr'
TEVALF			equ	IMUN+20 ;26h
TFMIN			equ	IMUN+21 ;27h
TFMAX			equ	IMUN+22 ;28h

;Token 29h Cannot Be An EOS Function Since EL(=29h Already
;---------------------------------------------------------
tEOSEL			equ	IMUN+23
;
TSPACE			equ	tEOSEL ;29h ' '
tString			equ	tEOSEL+1 ;2Ah '"'
tComma			equ	tEOSEL+2 ;2Bh ','

Tii			equ	2Ch ;i

;Postfix Functions
;---------------------------------------------
TPOST			equ	2Dh
;
tFact			equ	TPOST ;2Dh '!'

TCUBICR			equ	2Eh
TQUARTR			equ	2Fh

;Number Tokens
;--------------------------------------------
NUMS			equ	30h
;
T0			equ	NUMS ;30h
T1			equ	NUMS+1 ;31h
T2			equ	NUMS+2 ;32h
T3			equ	NUMS+3 ;33h
T4			equ	NUMS+4 ;34h
T5			equ	NUMS+5 ;35h
T6			equ	NUMS+6 ;36h
T7			equ	NUMS+7 ;37h
T8			equ	NUMS+8 ;38h
T9			equ	NUMS+9 ;39h
TDECPT			equ	NUMS+10 ;3Ah
Tee			equ	NUMS+11 ;3Bh

;Binary OP
;----------------------------------------------
tOr			equ	3Ch ;3Ch '_or_'
TXOR			equ	3Dh ;3Dh
;
tColon			equ	3Eh ;3Eh ':'
tEnter			equ	3Fh ;3Fh Lenter

;Binary OP
;--------------------------------------------------
tAnd			equ	40h ;40h '_and_' 05

;Letter Tokens
;------------------------------------------
LET			equ	41h
;
TA			equ	LET ;41h
TB			equ	LET+1 ;42h
TC			equ	LET+2 ;43h
TD			equ	LET+3 ;44h
TE			equ	LET+4 ;45h
TF			equ	LET+5 ;46h
TG			equ	LET+6 ;47h
TH			equ	LET+7 ;48h
TI			equ	LET+8 ;49h
TJ			equ	LET+9 ;4Ah
TK			equ	LET+10 ;4Bh
TL			equ	LET+11 ;4Ch
TM			equ	LET+12 ;4Dh
TN			equ	LET+13 ;4Eh
TO			equ	LET+14 ;4Fh
TP			equ	LET+15 ;50h
TQ			equ	LET+16 ;51h
TR			equ	LET+17 ;52h
TS			equ	LET+18 ;53h
TT			equ	LET+19 ;54h
TU			equ	LET+20 ;55h
TV			equ	LET+21 ;56h
TW			equ	LET+22 ;57h
TX			equ	LET+23 ;58h
TY			equ	LET+24 ;59h
TZ			equ	LET+25 ;5Ah
TTHETA			equ	LET+26 ;5Bh

;These Var Tokens Are 1st Of A Double Token
; See 'vars.doc' For More Info On Vars
;------------------------------------------
VTOKS			equ	LET+27

;User Matrix Token, 2nd Token Needed For Name
;--------------------------------------------
TVARMAT			equ	VTOKS ;5Ch

;User List Token, 2nd Token Needed For Name
;-------------------------------------------
TVARLST			equ	VTOKS+1 ;5Dh

;User Equation Token, 2nd Token Needed For Name
;----------------------------------------------
TVAREQU			equ	VTOKS+2 ;5Eh

;This Is A Single Token, I Will Have To Read The Name In
;Every Single Time I Execute
;-------------------------------------------
TPROG			equ	VTOKS+3 ;5Fh

;User Pict Token, 2nd Token Needed For Name
;-------------------------------------------
TVARPICT		equ	VTOKS+4 ;60h

;User GDB Token, 2nd Token Needed For Name
;-------------------------------------------
TVARGDB			equ	VTOKS+5 ;61h

;System Vars That Are Used For Output Only
;Cannot Be Stored To By The User
;-------------------------------------------
tVarOut			equ	VTOKS+6 ;62h

;System Vars That Are Not In The Symbol Table
;But The User Can Store To Them
;--------------------------------------------
tVarSys			equ	VTOKS+7 ;63h


;Mode Setting Commands
;------------------------------------------------------
MODESA			equ	VTOKS+8 ;64h
;
tRad			equ	MODESA ;64h 'Radian'
tDeg			equ	MODESA+1 ; 65h 'Degree'
tNormF			equ	MODESA+2 ;66h 'Normal'
tSci			equ	MODESA+3 ;67h 'Sci'
tEng			equ	MODESA+4 ;68h 'Eng'
tFloat			equ	MODESA+5 ;69h 'Float'
;
CMPS			equ	6Ah
;
tEQ			equ	CMPS ;6Ah '=='
tLT			equ	CMPS+1 ;6Bh '<'
tGT			equ	CMPS+2 ;6Ch '>'
tLE			equ	CMPS+3 ;6Dh LLE
tGE			equ	CMPS+4 ;6Eh LGE
tNE			equ	CMPS+5 ;6Fh LNE

;Binary OP
;-------------------------------------------
tAdd			equ	70h ;70h '+'
tSub			equ	71h ;71h '-'
;
TANS			equ	72h ;72h

;Mode Setting Commands
;----------------------------------
MODES			equ	73h
;
tFix			equ	MODES ;73h 'Fix_'
TSPLITON		equ	MODES+1 ;74h
TFULLSCREEN		equ	MODES+2 ;75h
tStndrd			equ	MODES+3 ;76h 'Func'
tParam			equ	MODES+4 ;77h 'Param'
tPolar			equ	MODES+5 ;78h 'Pol'
TSEQG			equ	MODES+6 ;79h
TAFILLON		equ	MODES+7 ;7Ah 'AUTO FILL ON
TAFILLOFF		equ	MODES+8 ;7Bh
TACALCON		equ	MODES+9 ;7Ch
TACALCOFF		equ	MODES+10 ;7Dh 'AUTOFILL OFF

;Graph Format Tokens Are 2 Byte Tokens
;--------------------------------------------
TGFORMAT		equ	MODES+11 ;7Eh
;
TBOXICON		equ	7Fh
TCROSSICON		equ	80h
TDOTICON		equ	81h

;Binary OP
;---------------------------
tMul			equ	82h ;82h '*'
tDiv			equ	83h ;83h '/'

;Some Graph Commands
;-------------------------
GRCMDS			equ	84h
;
tTrace			equ	GRCMDS ;84h 'Trace'
tClDrw			equ	GRCMDS+1 ;85h 'ClDrw'
tZoomStd		equ	GRCMDS+2 ;86h 'ZStd'
tZoomtrg		equ	GRCMDS+3 ;87h 'Ztrg'
tZOOMBOX		equ	GRCMDS+4 ;88h 'ZBOX'
tZoomIn			equ	GRCMDS+5 ;89h 'ZIn'
tZoomOut		equ	GRCMDS+6 ;8Ah 'ZOut'
tZoomSqr		equ	GRCMDS+7 ;8Bh 'ZSqr'
tZoomInt		equ	GRCMDS+8 ;8Ch 'ZInt'
tZoomPrev		equ	GRCMDS+9 ;8Dh 'ZPrev'
tZoomDec		equ	GRCMDS+10 ;8Eh 'ZDecm'
TZOOMSTAT		equ	GRCMDS+11 ;8Fh 'ZSTAT
tUsrZm			equ	GRCMDS+12 ;90h 'ZRcl'
tPrtScrn		equ	GRCMDS+13 ;91h 'PrtScrn'
tZOOMSTO		equ	GRCMDS+14 ;92h  ZOOM STORE
TTEXT			equ	GRCMDS+15 ;93h

;Binary OP
;----------------------------
tnPr			equ	GRCMDS+16 ;94h '_nPr_'
tnCr			equ	GRCMDS+17 ;95h '_nCr_'

;More Graph Commands
;--------------------------------
tYOn			equ	GRCMDS+18 ;96h 'FnOn_'
tYOff			equ	GRCMDS+19 ;97h 'FnOff_'
tStPic			equ	GRCMDS+20 ;98h 'StPic_'
tRcPic			equ	GRCMDS+21 ;99h 'RcPic_'
tStoDB			equ	GRCMDS+22 ;9Ah 'StGDB_'
tRclDB			equ	GRCMDS+23 ;9Bh 'RcGDB_'
tLine			equ	GRCMDS+24 ;9Ch 'Line'
tVert			equ	GRCMDS+25 ;9Dh 'Vert_'
tPtOn			equ	GRCMDS+26 ;9Eh 'PtOn'
tPtOff			equ	GRCMDS+27 ;9Fh 'PtOff'

;Token A0 Cannot Be An EOS Function Since Low MULT=A0 Already
;------------------------------------------------------------
tPtChg			equ	GRCMDS+28 ;A0h 'PtChg'
TPXON			equ	GRCMDS+29 ;A1h
TPXOFF			equ	GRCMDS+30 ;A2h
TPXCHG			equ	GRCMDS+31 ;A3h
tShade			equ	GRCMDS+32 ;A4h 'Shade'
tCircl			equ	GRCMDS+33 ;A5h 'Circl'
tHORZ			equ	GRCMDS+34 ;A6h 'HORIZONTAL'
tTanLn			equ	GRCMDS+35 ;A7h 'TanLn'
tDrInv			equ	GRCMDS+36 ;A8h 'DrInv_'
tDrawF			equ	GRCMDS+37 ;A9h 'DrawF_'
;
TVARSTRNG		equ	0AAh

;Functions With No Arguments
;------------------------------------------------------
NOARG			equ	0ABh
;
tRand			equ	NOARG ;ABh 'rand'
tPi			equ	NOARG+1 ;ACh  Lpi
tGetKey			equ	NOARG+2 ;ADh 'getKy'
;
TAPOST			equ	TGETKEY+1 ;Apostrophe
TQUEST			equ	TAPOST+1 ;Question Mark

;Unary Functions
;---------------------------------------------------
;Unary: These unary functions take a copmbination of
;        arguments, real, list, matrix
UNARY			equ	TQUEST+1 ;B0h
;
tChs			equ	UNARY ;B0h
tInt			equ	UNARY+1 ;B1h
TABS			equ	UNARY+2 ;B2h
tDet			equ	UNARY+3 ;B3h
tIdent			equ	UNARY+4 ;B4h
tDim			equ	UNARY+5 ;B5h
tSum			equ	UNARY+6 ;B6h
tProd			equ	UNARY+7 ;B7h
tNot			equ	UNARY+8 ;B8h
tIPart			equ	UNARY+9 ;B9h
tFPart			equ	UNARY+10 ;BAh

;New 2 Byte Tokens
;-----------------------------------
T2BYTETOK		equ	0BBh

;UNARYLR: Unary functions that only work with
;          real numbers and lists
;----------------------------------------------
UNARYLR EQU     UNARY+12
;
tSqrt			equ	UNARYLR ;BCh
TCUBRT			equ	UNARYLR+1 ;BDh
tLn			equ	UNARYLR+2 ;BEh
tExp			equ	UNARYLR+3 ;BFh
tLog			equ	UNARYLR+4 ;C0h
tALog			equ	UNARYLR+5 ;C1h
tSin			equ	UNARYLR+6 ;C2h
tASin			equ	UNARYLR+7 ;C3h
tCos			equ	UNARYLR+8 ;C4h
tACos			equ	UNARYLR+9 ;C5h
tTan			equ	UNARYLR+10 ;C6h
tATan			equ	UNARYLR+11 ;C7h
tSinh			equ	UNARYLR+12 ;C8h
tASinh			equ	UNARYLR+13 ;C9h
tCosh			equ	UNARYLR+14 ;CAh
tACosh			equ	UNARYLR+15 ;CBh
tTanh			equ	UNARYLR+16 ;CCh
tATanh			equ	UNARYLR+17 ;CDh

;Some Programming Commands
;--------------------------------------------------------
PROGTOK			equ	UNARYLR+18
;
tIf			equ	PROGTOK ;CEh
tThen			equ	PROGTOK+1 ;CFh
tElse			equ	PROGTOK+2 ;D0h
tWhile			equ	PROGTOK+3 ;D1h
tRepeat			equ	PROGTOK+4 ;D2h
tFor			equ	PROGTOK+5 ;D3h
tEnd			equ	PROGTOK+6 ;D4h
tReturn			equ	PROGTOK+7 ;D5h
tLbl			equ	PROGTOK+8 ;D6h 'Lbl_'
tGoto			equ	PROGTOK+9 ;D7h 'Goto_'
tPause			equ	PROGTOK+10 ;D8h 'Pause_'
tStop			equ	PROGTOK+11 ;D9h 'Stop'
tISG			equ	PROGTOK+12 ;DAh 'IS>'
tDSL			equ	PROGTOK+13 ;DBh 'DS<'
tInput			equ	PROGTOK+14 ;DCh 'Input_'
tPrompt			equ	PROGTOK+15 ;DDh 'Prompt_'
tDisp			equ	PROGTOK+16 ;DEh 'Disp_'
tDispG			equ	PROGTOK+17 ;DFh 'DispG'

;Token E0 Cannot Be An EOS Function Since HI MU
;---------------------------------------------
tOutput			equ	PROGTOK+18 ;E0h 'Outpt'
tClLCD			equ	PROGTOK+19 ;E1h 'ClLCD'
tConst			equ	PROGTOK+20 ;E2h 'Fill'
tSortA			equ	PROGTOK+21 ;E3h 'sortA_'
tSortD			equ	PROGTOK+22 ;E4h 'sortD_'
TDISPTAB		equ	PROGTOK+23 ;E5h 'DISP TABLE
tMenu			equ	PROGTOK+24 ;E6h 'Menu'
tSENDMBL		equ	PROGTOK+25 ;E7h 'SEND'
tGETMBL			equ	PROGTOK+26 ;E8h 'GET'

;Stat Plot Commands
;-------------------------
STATPCMD		equ	PROGTOK+27
;
TPLOTON			equ	STATPCMD ;E9h ' PLOTSON'
TPLOTOFF		equ	STATPCMD+1 ;EAh ' PLOTSOFF
;
TLISTNAME		equ	0EBh ;list designator

TPLOT1			equ	0ECh
TPLOT2			equ	0EDh
TPLOT3			equ	0EEh

tPower			equ	0F0h ;'^'
tXRoot			equ	0F1h ;LsupX,Lroot

STATCMD			equ	0F2h
;
tOneVar			equ	STATCMD ;F2h 'OneVar_'
TTWOVAR			equ	STATCMD+1 ;F3h
tLR			equ	STATCMD+2 ;F4h 'LinR(A+BX
tLRExp			equ	STATCMD+3 ;F5h 'ExpR_'
tLRLn			equ	STATCMD+4 ;F6h 'LnR_'
tLRPwr			equ	STATCMD+5 ;F7h 'PwrR_'
TMEDMED			equ	STATCMD+6 ;F8h
TQUAD			equ	STATCMD+7 ;F9h
tCLRLST			equ	STATCMD+8 ;FAh 'CLEAR LIST
TCLRTBL			equ	STATCMD+9 ;FBH 'CLEAR TABLE
tHist			equ	STATCMD+10 ;FCh 'Hist_'
txyLine			equ	STATCMD+11 ;FDH 'xyline_'
tScatter		equ	STATCMD+12 ;FEh 'Scatter_'
tLR1			equ	STATCMD+13 ;FFh 'LINR(AX+B

;2nd Half Of Graph Format Tokens
;--------------------------------------------------
;Format Settings Commands
GFMT			equ	0
;
tSeq			equ	GFMT ;'SeqG'
tSimulG			equ	GFMT+1 ;'SimulG'
tPolarG			equ	GFMT+2 ;'PolarGC'
tRectG			equ	GFMT+3 ;'RectGC'
tCoordOn		equ	GFMT+4 ;'CoordOn'
tCoordOff		equ	GFMT+5 ;'CoordOff'
tDrawLine		equ	GFMT+6 ;'DrawLine'
tDrawDot		equ	GFMT+7 ;'DrawDot'
tAxisOn			equ	GFMT+8 ;'AxesOn'
tAxisOff		equ	GFMT+9 ;'AxesOff'
tGridOn			equ	GFMT+10 ;'GridOn'
tGridOff		equ	GFMT+11 ;'GridOff'
tLblOn			equ	GFMT+12 ;'LabelOn'
tLblOff			equ	GFMT+13 ;'LabelOff'
tWebOn			equ	GFMT+14 ;'WebOn'
tWebOff			equ	GFMT+15 ;'WebOFF'
TUV			equ	GFMT+16 ;U vs V
TVW			equ	GFMT+17 ;V vs W
TUW			equ	GFMT+18 ;U vs W

;2nd Half Of User Matrix Tokens
;-----------------------------------------
TMATA			equ	00h ;MAT A
TMATB			equ	01h ;MAT B
TMATC			equ	02h ;MAT C
TMATD			equ	03h ;MAT D
TMATE			equ	04h ;MAT E
TMATF			equ	05h ;MAT F
TMATG			equ	06h ;MAT G
TMATH			equ	07h ;MAT H
TMATI			equ	08h ;MAT I
TMATJ			equ	09h ;MAT J

;2nd Half Of User List Tokens
;------------------------------------------
TL1			equ	00h ;List 1
TL2			equ	01h ;List 2
TL3			equ	02h ;List 3
TL4			equ	03h ;List 4
TL5			equ	04h ;List 5
TL6			equ	05h ;List 6

;2nd Half Of User Equation Tokens
;--------------------------------------
;"Y" Equations Have Bit 4 Set
TY1			equ	10h ;Y1
TY2			equ	11h ;Y2
TY3			equ	12h ;Y3
TY4			equ	13h ;Y4
TY5			equ	14h ;Y5
TY6			equ	15h ;Y6
TY7			equ	16h ;Y7
TY8			equ	17h ;Y8
TY9			equ	18h ;Y9
TY0			equ	19h ;Y0

;Param Equations Have Bit 5 Set
;---------------------------------------
TX1T			equ	20h ;X1t
TY1T			equ	21h ;Y1t
TX2T			equ	22h ;X2t
TY2T			equ	23h ;Y2t
TX3T			equ	24h ;X3t
TY3T			equ	25h ;Y3t
TX4T			equ	26h ;X4t
TY4T			equ	27h ;Y4t
TX5T			equ	28h ;X5t
TY5T			equ	29h ;Y5t
TX6T			equ	2Ah ;X6t
TY6T			equ	2Bh ;Y6t

;Polar Equations Have Bit 6 Set
;--------------------------------------
TR1			equ	40h ;R1
TR2			equ	41h ;R2
TR3			equ	42h ;R3
TR4			equ	43h ;R4
TR5			equ	44h ;R5
TR6			equ	45h ;R6

;Recursion Equations Have Bit 7 Set
;--------------------------------------
TUN			equ	80h ;Un
TVN			equ	81h ;Vn
TWN			equ	82h ;Wn

;2nd Half Of User Picture Tokens
;----------------------------------------
TPIC1			equ	00h ;PIC1
TPIC2			equ	01h ;PIC2
TPIC3			equ	02h ;PIC3
TPIC4			equ	03h ;PIC4
TPIC5			equ	04h ;PIC5
TPIC6			equ	05h ;PIC6
TPIC7			equ	06h ;PIC7
TPIC8			equ	07h ;PIC8
TPIC9			equ	08h ;PIC9
TPIC0			equ	09h ;PIC0

;2nd Half Of User Graph Database Tokens
;----------------------------------------
TGDB1			equ	00h ;GDB1
TGDB2			equ	01h ;GDB2
TGDB3			equ	02h ;GDB3
TGDB4			equ	03h ;GDB4
TGDB5			equ	04h ;GDB5
TGDB6			equ	05h ;GDB6
TGDB7			equ	06h ;GDB7
TGDB8			equ	07h ;GDB8
TGDB9			equ	08h ;GDB9
TGDB0			equ	09h ;GDB0

;2nd Half Of String Vars
;----------------------------------
TSTR1			equ	00h
TSTR2			equ	01h
TSTR3			equ	02h
TSTR4			equ	03h
TSTR5			equ	04h
TSTR6			equ	05h
TSTR7			equ	06h
TSTR8			equ	07h
TSTR9			equ	08h
TSTR0			equ	09h

;2nd Half Of System Output Only Variables
;--------------------------------------------------------------------
;OPEN			equ	00h
;
TREGEQ			equ	01h ;REGRESSION EQUATION
;
TSTATN			equ	02h ;STATISTICS N
;
TXMEAN			equ	03h ;X MEAN
TSUMX			equ	04h ;SUM(X)
TSUMXSQR		equ	05h ;SUM(X^2)
TSTDX			equ	06h ;STANDARD DEV X
TSTDPX			equ	07h ;STANDARD DEV POP X
TMINX			equ	08h ;MIN X VALUE
TMAXX			equ	09h ;MAX X VALUE
;
TMINY			equ	0Ah ;MIN Y VALUE
TMAXY			equ	0Bh ;MAX Y VALUE
TYMEAN			equ	0Ch ;Y MEAN
TSUMY			equ	0Dh ;SUM(Y)
TSUMYSQR		equ	0Eh ;SUM(Y^2)
TSTDY			equ	0Fh ;STANDARD DEV Y
TSTDPY			equ	10h ;STANDARD DEV POP Y
;
TSUMXY			equ	11h ;SUM(XY)
TCORR			equ	12h ;CORRELATION
TMEDX			equ	13h ;MED(X)
TQ1			equ	14h ;1ST QUADRANT OF X
TQ3			equ	15h ;3RD QUADRANT OF X
TQUADa			equ	16h ;1ST TERM OF QUAD POLY REG/ Y-INT
TQUADb			equ	17h ;2ND TERM OF QUAD POLY REG/ SLOPE
TQUADc			equ	18h ;3RD TERM OF QUAD POLY REG
TCUBEd			equ	19h ;4TH TERM OF CUBIC POLY REG
TQUARTe			equ	1Ah ;5TH TERM OF QUART POLY REG
TMEDX1			equ	1Bh ;x1 FOR MED-MED
TMEDX2			equ	1Ch ;x2 FOR MED-MED
TMEDX3			equ	1Dh ;x3 FOR MED-MED
TMEDY1			equ	1Eh ;y1 FOR MED-MED
TMEDY2			equ	1Fh ;y2 FOR MED-MED
TMEDY3			equ	20h ;y3 FOR MED-MED
;
TRECURN			equ	21h ;RECURSION N
TSTATP			equ	22h
TSTATZ			equ	23h
TSTATT			equ	24h
TSTATCHI		equ	25h
TSTATF			equ	26h
TSTATDF			equ	27h
TSTATPHAT		equ	28h
TSTATPHAT1		equ	29h
TSTATPHAT2		equ	2Ah
TSTATMEANX1		equ	2Bh
TSTATSTDX1		equ	2Ch
TSTATN1			equ	2Dh
TSTATMEANX2		equ	2Eh
TSTATSTDX2		equ	2Fh
TSTATN2			equ	30h
TSTATSTDXP		equ	31h
TSTATLOWER		equ	32h
TSTATUPPER		equ	33h
TSTAT_S			equ	34h
TLRSQR			equ	35h ;r^2
TBRSQR			equ	36h ;R^2

;These next tokens are only used to acess the data.
;They are display only and the user cannot access them at all.
;-------------------------------------------------------------
TF_DF			equ	37h ;ANOFAV FACTOR DF
TF_SS			equ	38h ;ANOFAV FACTOR SS
TF_MS			equ	39h ;ANOFAV FACTOR MS
TE_DF			equ	3Ah ;ANOFAV ERROR DF
TE_SS			equ	3Bh ;ANOFAV ERROR SS
TE_MS			equ	3Ch ;ANOFAV ERROR MS

;2nd Half Of System Input/Output Variables
;----------------------------------------------------
;System Variable Equates
;
;Vars With No Restriction
TUXSCL			equ	0
TUYSCL			equ	1
TXSCL			equ	2
TYSCL			equ	3
TRECURU0		equ	4 ;U 1ST INITIAL COND
TRECURV0		equ	5 ;V 1ST INITIAL COND
TUN1			equ	6 ;U(N-1) ;NOT USED
TVN1			equ	7 ;V(N-1) ;NOT USED
TURECURU0		equ	8
TURECURV0		equ	9

;Vars Rounded To 10 Digits
;----------------------------------
TXMIN			equ	0Ah
TXMAX			equ	0Bh
TYMIN			equ	0Ch
TYMAX			equ	0Dh
TTMIN			equ	0Eh
TTMAX			equ	0Fh
TTHETAMIN		equ	10h
TTHETAMAX		equ	11h
TUXMIN			equ	12h
TUXMAX			equ	13h
TUYMIN			equ	14h
TUYMAX			equ	15h
TUTHETMIN		equ	16h
TUTHETMAX		equ	17h
TUTMIN			equ	18h
TUTMAX			equ	19h
TTBLMIN			equ	1Ah
TPLOTSTART		equ	1Bh
TUPLOTSTART		equ	1Ch
TNMAX			equ	1Dh
TUNMAX			equ	1Eh
TNMIN			equ	1Fh
TUNMIN			equ	20h

;Vars Cannot Be 0
;----------------------------------
TTBLSTEP		equ	21h
TTSTEP			equ	22h
TTHETASTEP		equ	23h
TUTSTEP			equ	24h
TUTHETSTEP		equ	25h

;Special Limits On These
;----------------------------------
TDELTAX			equ	26h
TDELTAY			equ	27h

;Special Limits On These
;----------------------------------
TXFACT			equ	28h
TYFACT			equ	29h

TTBLINPUT		equ	2Ah

;Any Restrictions?
;-----------------------------------------
TFINn			equ	2Bh
TFINi			equ	2Ch
TFINpv			equ	2Dh
TFINpmt			equ	2Eh
TFINfv			equ	2Fh
TFINpy			equ	30h
TFINcy			equ	31h
;
TRECURW0		equ	32h ;W0(1)
TURECURW0		equ	33h
;
TPLOTSTEP		equ	34h
TUPLOTSTEP		equ	35h
;
TXRES			equ	36h
TUXRES			equ	37h

;All The Tokens Below Do Not Have Strings Associated.
;They Are Here Just For Storing And Recalling.
;----------------------------------------------------
TRECURU02		equ	38h ;U0(2)
TURECURU02		equ	39h
TRECURV02		equ	3Ch ;V0(2)
TURECURV02		equ	3Dh
TRECURW02		equ	3Eh ;W0(2)
TURECURW02		equ	3Fh

;No Restrictions
;------------------------------------------
;TWN1			equ	40h ;W(N-1)
;UN2			equ	41h ;U(N-2)
;VN2			equ	42h ;V(N-2)
;WN2			equ	43h ;W(N-2)

;Don't Know If Need These Yet?
;-----------------------------
;TUI12
;TUI22
;TUUI12
;TUUI22

TFINnpv			equ	00h ;immediate unary
TFINirr			equ	01h ;immediate unary
TFINbal			equ	02h ;immediate unary
TFINprn			equ	03h ;immediate unary
TFINint			equ	04h ;immediate unary
TFINtonom		equ	05h ;immediate unary
TFINtoeff		equ	06h ;immediate unary
TFINdbd			equ	07h ;immediate unary
TLCM			equ	08h ;immediate unary
TGCD			equ	09h ;immediate unary
TRANDINT		equ	0Ah ;immediate unary
TRANDBIN		equ	0Bh ;immediate unary
TSUBSTRNG		equ	0Ch ;immediate unary
TSTDDEV			equ	0Dh ;immediate unary
TVARIANCE		equ	0Eh ;immediate unary
TINSTRNG		equ	0Fh ;immediate unary
TDNORMAL		equ	10h ;immediate unary
TINVNORM		equ	11h ;immediate unary
TDT			equ	12h ;immediate unary
TCHI			equ	13h ;immediate unary
TDF			equ	14h ;immediate unary
TBINPDF			equ	15h ;immediate unary
TBINCDF			equ	16h ;immediate unary
TPOIPDF			equ	17h ;immediate unary
TPOICDF			equ	18h ;immediate unary
TGEOPDF			equ	19h ;immediate unary
TGEOCDF			equ	1Ah ;immediate unary
TNORMALPDF		equ	1Bh ;immediate unary
TTPDF			equ	1Ch ;immediate unary
TCHIPDF			equ	1Dh ;immediate unary
TFPDF			equ	1Eh ;immediate unary
;TRANDBIN		equ	1Fh ;immediate unary
TRANDNORM		equ	1Fh ;immediate unary
TFINFpmt		equ	20h ;0 or multy arg function
TFINFi			equ	21h ;0 or multy arg function
TFINFpv			equ	22h ;0 or multy arg function
TFINFn			equ	23h ;0 or multy arg function
TFINFfv			equ	24h ;0 or multy arg function
TCONJ			equ	25h ;unary
TREAL			equ	26h ;unary
TIMAG			equ	27h ;unary
TANGLE			equ	28h ;unary
TCUMSUM			equ	29h ;unary
TEXPR			equ	2Ah ;unary
TLENGTH			equ	2Bh ;unary
TDELTALST		equ	2Ch ;unary
TREF			equ	2Dh ;unary
TRREF			equ	2Eh ;unary
TTORECT			equ	2Fh ;display conversion
TTOPOLAR		equ	30h ;display conversion
TCONSTE			equ	31h ;function no arg
TSINREG			equ	32h ;command opt push
TLOGISTIC		equ	33h ;command opt push
TLINREGTTEST		equ	34h ;command opt push
TSHADENORM		equ	35h ;command pushed
TSHADET			equ	36h ;command pushed
TSHADECHI		equ	37h ;command pushed
TSHADEF			equ	38h ;command pushed
TMATTOLST		equ	39h ;command pushed
TLSTTOMAT		equ	3Ah ;command pushed
TZTEST			equ	3Bh ;command pushed
TTTEST			equ	3Ch ;command pushed
T2SAMPZTEST		equ	3Dh ;command pushed
T1PROPZTEST		equ	3Eh ;command pushed
T2PROPZTEST		equ	3Fh ;command pushed
TCHITEST		equ	40h ;command pushed
TZINTVAL		equ	41h ;command pushed
T2SAMPZINT		equ	42h ;command pushed
T1PROPZINT		equ	43h ;command pushed
T2PROPZINT		equ	44h ;command pushed
TGRAPHSTYLE		equ	45h ;command pushed
T2SAMPTTEST		equ	46h ;command opt pushed
T2SAMPFTEST		equ	47h ;command opt pushed
TTINTVAL		equ	48h ;command opt pushed
T2SAMPTINT		equ	49h ;command opt pushed
TSETUPLST		equ	4Ah ;command opt pushed
TFINpmtend		equ	4Bh ;mode command
TFINpmtbeg		equ	4Ch ;mode command
TREALM			equ	4Dh ;mode command
TPOLARM			equ	4Eh ;mode command
TRECTM			equ	4Fh ;mode command
TEXPRON			equ	50h ;mode command
TEXPROFF		equ	51h ;mode command
TCLRALLLST		equ	52h ;command not pushed
TGETCALC		equ	53h ;command not pushed
TDELVAR			equ	54h ;command not pushed
TEQUTOSTRNG		equ	55h ;command not pushed
TSTRNGTOEQU		equ	56h ;command not pushed
tdellast		equ	57h ;command not pushed
TSELECT			equ	58h ;command not pushed
TANOVA			equ	59h ;command not pushed
TMODBOX			equ	5Ah ;icon
TNORMPROB		equ	5Bh ;icon

;Tokens 5Ch to 63h cannot be the 2nd Byte of
; two byte tokens. These are already the 1st byte value
; of two byte tokens. Reading tokens backwards Will not
; work if this restriction is not in place.
;------------------------------------------------------
;NO USE			equ	5Ch
;NO USE			equ	5Dh
;NO USE			equ	5Eh
;NO USE			equ	5Fh
;NO USE			equ	60h
;NO USE			equ	61h
;NO USE			equ	62h
;NO USE			equ	63h

TMGT			equ	64h ;VERTICAL SPLIT
TZFIT			equ	65h ;ZOOM FIT
TDIAG_ON		equ	66h ;DIANOSTIC DISPLAY ON
TDIAG_OFF		equ	67h ;DIANOSTIC DISPLAY OFF