;TI-86 Include File by Brandon Wilson

#define EQU .EQU
#define equ .EQU

;ROM Calls
_bitNUMOP1              equ     4000h ;bit numop1,(iy+parsflag2)
_bitModeSolve           equ     4004h ;bit 1,(iy+8)
_isParseError           equ     4008h ;test if (_PARSEVAR+1) is 0
_clrError               equ     400Ch ;set (_PARSEVAR+1) to 0
_ldHLind                equ     4010h ;ld hl,(hl)
_intrptOn               equ     4014h ;end interrupt
_SETGRAPHDRAW           equ     4018h ;res 2,(iy+$1B)
_setTBLGraphDraw        equ     401Ch
_setTBLGraphonly        equ     4020h
_SETTBLGRAPHONLY_nof    equ     4024h
_bitOnInterrupt         equ     4028h ;bit onInterrupt,(iy+onflags)
_chkON                  equ     4028h ;bit onInterrupt,(iy+onflags)
_bitGrfFuncM            equ     402Ch ;bit grffuncm,(iy+grfmodeflags)
_bitGrfPolarM           equ     4030h ;bit grfpolarm,(iy+grfmodeflags)
_bitGrfParamM           equ     4034h ;bit grfparamm,(iy+grfmodeflags)
_bitGrfDifeqM           equ     4038h ;bit grfrecurm,(iy+grfmodeflags)
_cphlde                 equ     403Ch ;cp hl,de
_dispColon              equ     4040h ;display ":" at cursor location
_put_colon              equ     4040h ;display ":" at cursor location
_divHLby10              equ     4044h ;divide HL by 10
_divHLbyA               equ     4048h ;divide HL by A
_divHLby10_3b           equ     404Ch ;divide AHL by 10
_divAHLby10             equ     404Ch ;divide AHL by 10
_updateRunIndic         equ     4050h ;animate busy indicator
_alphaRCL               equ     4054h
_APDHandler             equ     4058h ;dec timeout counter, turn calc off if 0
_timeout                equ     4058h ;dec timeout counter. turn calc off if 0
_idle                   equ     405Ch ;halt if _APDWarmUp is not 0
_apdSetup               equ     4060h ;reset APD counter to $A1
_resetAPD               equ     4060h ;reset APD counter to $A1
_kbdScan                equ     4064h ;keyboard handler (read keypresses)
_scan_code              equ     4064h ;keyboard handler (read keypresses)
_getcsc                 equ     4068h ;A = key code
_get_key                equ     4068h ;A = key code
_unshiftAlpha           equ     406Ch
_canAlphIns             equ     4070h
_canAlpha               equ     4074h ;clear alpha lock
_forceAlpha             equ     4078h ;set unclearable alpha lock
_setAlphaLock           equ     407Ch ;set upper-case alpha lock
_setAlpha               equ     4080h ;set upper-case alpha
_mon                    equ     4084h ;go to home screen, nothing cleared/reset
_monForceKey            equ     4088h
_sendKPress             equ     408Ch ;clear onInterrupt, _monQueue through _cxMain
_popQueue               equ     4090h ;get C from _monQueue or no carry
_pushQueue              equ     4094h ;put A in _monQueue
_popCx                  equ     4098h ;DE=memsysstack
_JforceCmdNoChar        equ     409Ch ;force out of program
_JforceCmd              equ     409Fh
_Jforcekey              equ     40A2h
_resetStacks            equ     40A5h ;reset _OPS,_FPS,_ERRSP
_sysErrHandler          equ     40A9h
_newContext             equ     40ADh
_newContext0            equ     40B1h
_PPutAwayPrompt         equ     40B5h ;put home screen edit away
_resPromptFlags         equ     40B9h
_PPutAway               equ     40BDh ;call _cxPPutAway, clear alpha lock
_call_cxPPutAway	equ 	40BDh ;call _cxPPutAway, clear alpha lock
_PutAway                equ     40C1h ;call _cxPutAway, clear (PARSEVAR+1), clear alpha lock
_call_cxPutAway		equ	40C1h ;call _cxPutAway, clear (PARSEVAR+1), clear alpha lock
_SizeWind               equ     40C5h ;call _cxSizeWind, test 5,(iy+$01)
_call_cxSizeWind	equ	40C5h ;call _cxSizeWind, test 5,(iy+$01)
_ErrorEP                equ     40C9h ;call _cxErrorEP
_call_cxErrorEP		equ	40C9h ;call _cxErrorEP
_callMain               equ     40CDh ;call _cxMain
_call_cxMain		equ	40CDh ;call _cxMain
_putAwayNoOp            equ     40D1h
_cxNull			equ	40D1h
_p_cxNull		equ	40D2h
_exit                   equ     40D5h
_clearSolveMode         equ     40D9h
_monErrHand             equ     40DDh ;default error handler
_err_handler		equ	40DDh ;default error handler
_appInitIfMemx2         equ     40E1h
_set_cx_100		equ	40E1h
_appInitIfMem           equ     40E5h
_set_cx_50		equ	40E5h
_appInitIfDec           equ     40E9h
_set_cx_dec		equ	40E9h
_appInit                equ     40EDh ;init the State Monitor with a new set of context vectors
_set_context		equ	40EDh ;init the State Monitor with a new set of context vectors
_iINITC                 equ     40F1h
_iINITCED               equ     40F5h
_ifQUIT                 equ     40F9h
_onKeyHandler           equ     40FDh
_setChecksum            equ     4101h
_off			equ	4101h ;turn calc off normally
_turn86off              equ     4105h ;turn calc off
_initialize             equ     4109h ;reset calculator
_reset			equ	4109h ;reset calculator
_resetdone0             equ     410Dh
_ResetDone              equ     4111h
_initMem                equ     4115h
_INIT_ASAP_RAM          equ     4119h ;remove all user tokens
_removTok		equ	4119h ;remove all user tokens
_INITGS                 equ     411Dh ;clear _GY1,_GX1,_GR1, fill _GQ1 with $11
_initlistvar            equ     4121h ;reset list editor
_setName                equ     4125h ;move byte length string from (ix) to (_OP1+1)
_warmup                 equ     4129h
_errAxes                equ     412Dh ;error 35 AXES
_err1stOrder            equ     4130h ;error 36 FLD-ORDER
_errFldOrder		equ	4130h ;error 36 FLD-ORDER
_errStatPlot            equ     4133h ;error 34 STAT PLOT
_errOverflow            equ     4136h ;error 01 OVERFLOW
_errDivBy0              equ     4139h ;error 02 DIV BY 0
_errSingularMat         equ     413Ch ;error 03 SINGULAR MAT
_errDomain              equ     413Fh ;error 04 DOMAIN
_errIncrement           equ     4142h ;error 05 INCREMENT
_errSyntax              equ     4145h ;error 07 SYNTAX
_errNumberBase          equ     4148h ;error 08 NUMBER BASE
_errMode                equ     414Bh ;error 09 MODE
_errDataType            equ     414Eh ;error 10 DATA TYPE
_errArgument            equ     4151h ;error 11 ARGUMENT
_errDimMismatch         equ     4154h ;error 12 DIM MISMATCH
_errDimension           equ     4157h ;error 13 DIMENSION
_errUndefined           equ     415Ah ;error 14 UNDEFINED
_errEvStkOvfl           equ     415Dh ;error 15 MEMORY
_errMemory              equ     4160h ;error 15 MEMORY
_checkTokMem            equ     4163h ;error 15 MEMORY if bit 2,(iy+$13) set
_errMemFail             equ     4166h ;error 15 MEMORY, no goto
_errReserved            equ     4169h ;error 16 RESERVED
_errInvalid             equ     416Ch ;error 17 INVALID
_errIllegalNest         equ     416Fh ;error 18 ILLEGAL NEST
_errBound               equ     4172h ;error 19 BOUND
_errGraphRange          equ     4175h ;error 20 GRAPH WINDOW
_errGraphWindow		equ	4175h ;error 20 GRAPH WINDOW
_errZoom                equ     4178h ;error 21 ZOOM
_errBreak               equ     417Bh ;error 06 BREAK
_errStat                equ     417Eh ;error 23 STAT
_errConversion          equ     4181h ;error 24 CONVERSION
_errSolver              equ     4184h ;error 25 SOLVER
_errIterations          equ     4187h ;error 28 ITERATIONS
_errBadGuess            equ     418Ah ;error 29 BAD GUESS
_errDifEqSetup          equ     418Dh ;error 30 DIF EQ SETUP
_errPoly                equ     4190h ;error 32 POLY
_errTolTooSmall         equ     4193h ;error 33 TOL NOT MET
_errTolNotMet		equ	4193h ;error 33 TOL NOT MET
_errLinkXmit            equ     4196h ;error 37 LINK
_errLink		equ	4196h ;error 37 LINK
_JError                 equ     4199h ;invoke error message, a = error #
_errorA			equ	4199h ;invoke error message, a = error #
_JErrorNo               equ     419Ch ;invoke error message, _ERRNO = error #
_error			equ	419Ch ;invoke error message, _ERRNO = error #
_noErrorEntry           equ     419Fh
_pushErrorHandler       equ     41A1h ;install error handler HL
_instError		equ	41A1h ;install error handler HL
_popErrorHandler        equ     41A4h ;remove error handler
_removError		equ	41A4h ;remove error handler
_C10BNUM1               equ     41A7h ;.db 0100FC10000000000000 (fp 1 cplx)
_C9BNUM1                equ     41ABh ;.db 00FC10000000000000 (fp 1)
_C10BNUM0               equ     41AFh ;.db 0100FC00000000000000 (fp 0 cplx)
_R10NUM0                equ     41B3h ;.db 0000FC00000000000000 (fp 0)
_SET10BNUM0             equ     41B7h ;fp0 to de
_ld_de_fp0		equ	41B7h ;fp0 to de
_SET10BNUM1             equ     41BBh ;fp1 to de
_ld_de_fp1		equ	41BBh ;fp1 to de
_HLTIMES10              equ     41BFh ;multiply HL by 10
_mulHL10		equ	41BFh ;multiply HL by 10
_CKOP1CPLX              equ     41C3h ;check if OP1 is CPLX
_CKCPLX                 equ     41C7h ;expects type in A
_CKOP1REAL              equ     41CBh ;check if OP1 is REAL
_ANGLE                  equ     41CFh
_IMAG                   equ     41D3h ;OP1 = imag (OP1,OP2)
_SETOP1REAL             equ     41D7h ;set OP1 type real
_REALX                  equ     41DBh
_SETREAL                equ     41DFh ;set HL type real
_COP1SET0               equ     41E3h ;OP1,OP2 = CPLX (0,0)
_OP2SET0CPLX            equ     41E7h ;real OP1 to complex OP1,OP2
_SETOP1CPLX             equ     41EBh ;set OP1 cplx
_CPOP4OP3               equ     41EFh ;cp OP4,OP3; OP1=OP4 and OP2=OP3
_MOV10OP2CP             equ     41F3h ;cp OP1,(HL); OP2=(HL)
_ABSO1O2CP              equ     41F7h ;abs(OP1,OP2) and cp OP1,OP2
_CPOP1OP2               equ     41FBh ;cp OP1,OP2
_CP6                    equ     41FFh
_OP3TOOP4               equ     4203h ;OP3->OP4
_OP1TOOP4               equ     4207h
_OP2TOOP4               equ     420Bh
_MOVTOOP4               equ     420Fh ;move 11 bytes (HL) to OP4
_OP4TOOP2               equ     4213h
_OP4TOOP3               equ     4217h
_OP3TOOP2               equ     421Bh
_OP1TOOP3               equ     421Fh
_MOVOP1TO               equ     4223h ;move 11 bytes OP1 to de
_OP5TOOP2               equ     4227h
_OP5TOOP6               equ     422Bh
_OP5TOOP4               equ     422Fh
_OP1TOOP2               equ     4233h
_MOVTOOP2               equ     4237h ;move 11 bytes (HL) to OP2
_OP6TOOP2               equ     423Bh
_OP6TOOP1               equ     423Fh
_OP4TOOP1               equ     4243h
_OP5TOOP1               equ     4247h
_OP3TOOP1               equ	424Bh
_OP4TOOP5               equ	424Fh
_OP3TOOP5               equ	4253h
_OP2TOOP5               equ	4257h
_MOVTOOP5               equ	425Bh
_OP2TOOP6               equ	425Fh
_OP1TOOP6               equ	4263h
_MOVTOOP6               equ	4267h
_OP1TOOP5               equ	426Bh
_OP2TOOP1               equ	426Fh
_MOVTOOP1               equ	4273h
_MOVREG                 equ	4277h ;move 11 bytes from (hl) to (de)
_mov11b			equ	4277h ;move 11 bytes at (hl) to (de)
_MOV10B                 equ	427Bh ;move 10 bytes, (hl) to (de)
_MOV18                  equ	427Fh ;move 9 bytes, (hl) to (de)
_mov9b                  equ	4283h ;move 9 bytes, (hl) to (de)
_mov9b_			equ	4283h ;move 9 bytes at (hl) to (de)
_MOV8B                  equ     4287h ;move 8 bytes, (hl) to (de)
_MOV7B                  equ     428Bh ;move 7 bytes, (hl) to (de)
_mov7b_			equ	428Fh ;move 7 bytes at (hl) to (de)
_MOV14                  equ     428Fh ;move 7 bytes, (hl) to (de)
_MOV6B                  equ     4293h ;move 6 bytes, (hl) to (de)
_MOV5B                  equ     4297h ;move 5 bytes, (hl) to (de)
_MOV4B                  equ     429Bh ;move 4 bytes, (hl) to (de)
_MOV3B                  equ     429Fh ;move 3 bytes, (hl) to (de)
_MOV2B                  equ     42A3h ;move 2 bytes, (hl) to (de)
_O2TOO4D16              equ     42A7h
_op4toop2m		equ	42A7h
_O1TOO4D16              equ     42ABh
_O4TOO1D16              equ     42AFh
_O2TOO1D16              equ     42B3h
_O3TOO2D14              equ     42B7h
_O5TOO1D14              equ     42BBh
_O3TOO1D14              equ     42BFh
_O3TOO1D16              equ     42C3h
_TOOP3D14               equ     42C7h
_OP2TOOP3               equ     42CBh
_MOVTOOP3               equ     42CFh
_OP4TOOP6               equ     42D3h
_MOV10TOOP1             equ     42D7h
_MOV10OP1OP2            equ     42DBh
_mov10toop1op2		equ	42DBh
_MOV10TOOP2             equ     42DFh
_MOVFROP1               equ     42E3h
_movfroop1              equ     42E3h
_OP4SET1                equ     42E7h
_OP3SET1                equ     42EBh
_OP2SET8                equ     42EFh
_SETNUM8                equ     42F3h
_OP2SET5                equ     42F7h
_OP2SET4                equ     42FBh
_SETNUM4                equ     42FFh
_OP2SET3                equ     4303h
_SETNUM3                equ     4307h
_OP1SET1A               equ     430Bh
_op1set1                equ     430Fh
_OP1SET4                equ     4313h
_OP1SET3                equ     4317h
_OP3SET2                equ     431Bh
_OP1SET2                equ     431Fh
_OP2SET2                equ     4323h
_SETNUM2                equ     4327h
_SETMANT1               equ     432Bh
_OP2SET1                equ     432Fh
_SETNUM1                equ     4333h
_SETNUM                 equ     4337h
_SETNUMA                equ     433Bh
_SETMANT                equ     433Fh
_ZERO16D                equ     4343h
_ld_hl_8z		equ	4343h
_SET16A                 equ     4347h
_ld_hl_8a               equ     4347h
_SET14A                 equ     434Bh
_ld_hlplus1_7a		equ	434Bh
_SET14D                 equ     434Fh
_ld_hl_7a		equ	434Fh
_OP4SET0                equ     4353h
_OP3SET0                equ     4357h
_OP2SET0                equ     435Bh
_OP1SET0                equ     435Fh
_SETNUM0                equ     4363h
_ld_hl_fp0		equ	4363h
_ZEROOP1M               equ     4367h
_ZEROOP1                equ     436Bh
_ZEROOP2                equ     436Fh
_ZEROOP3                equ     4373h
_ZEROOP                 equ     4377h ;11 zeros to (HL)
_ld_hl_11z		equ	4377h ;11 zeros to (hl)
_CLRLP                  equ     437Bh ;B zeros to (HL)
_ld_hl_bz               equ     437Bh ;B zeros to (HL)
_SHRCTOACC              equ     437Fh ;high C to low A
_SHRACC                 equ     4383h ;high A to low A (A>>4)
_SHLCTOACC              equ     4387h ;low C to high A
_SHLACC                 equ     438Bh ;low A to high A (A<<4)
_SHROP1D14              equ     438Fh ;(op1+3...op1+9)>>4
_SHRO1D18               equ     4393h ;(op1+1...op1+9)>>4
_SHROP1D18              equ     4397h ;(op1+2...op1+10)>>4
_SHR18                  equ     439Bh ;(hl...hl+8)>>4
_SHR18A                 equ     439Fh ;a(hl...hl+8)>>4
_SHROP1D16              equ     43A3h
_SHR16                  equ     43A7h ;(hl...hl+7)>>4
_SHRATOOPM16            equ     43ABh ;a(hl...hl+7)>>4
_SHR14                  equ     43AFh ;a(hl...hl+6)>>4
_SHLOP3D14              equ     43B3h
_SHLOP1D14              equ     43B7h
_SHR1TOOP1M             equ     43BBh
_SHR1TOOPM              equ     43BFh
_SHLOP3D16              equ     43C3h
_SHLOP1D16              equ     43C7h
_SHL16                  equ     43CBh
_SHL14                  equ     43CFh
_SHRSIN                 equ     43D3h
_SRDRO4D16              equ     43D7h
_SRDO1ONCE              equ     43DBh
_SRDO1                  equ     43DFh
_SHRCCON                equ     43E3h
_SRDRO2D16              equ     43E7h
_SHRDRND                equ     43EBh
_MANTP1                 equ     43EFh
_MANTPA                 equ     43F3h
_ADDPROP                equ     43F7h
_ADDPROPLP              equ     43FBh
_APROPOE                equ     43FFh
_APROP10                equ     4403h
_OP2POP4D16             equ     4407h
_OP2POP1D16             equ     440Bh
_OP1POP2D16             equ     440Fh
_OP2POP2D14             equ     4413h
_OP1POP2D14             equ     4417h
_OP1POP3D16             equ     441Bh
_ADD16D                 equ     441Fh
_ADD14D                 equ     4423h
_ADD2D                  equ     4427h
_CSVAS                  equ     442Bh
_CSVAS1                 equ     442Fh
_CSVASH                 equ     4433h
_OP1POP3D14             equ     4437h
_OP2MOP1D16             equ     443Bh
_OP1MOP2D16             equ     443Fh
_OP3MOP1D16             equ     4443h
_OP1MOP3D16             equ     4447h
_SUB16D                 equ     444Bh
_SUB14D                 equ     444Fh
_SUB10D                 equ     4453h
_SUB8D                  equ     4457h
_SUB6D                  equ     445Bh
_OP4MD14                equ     445Fh
_OP4MD16                equ     4463h
_OP3MOP1D14             equ     4467h
_OP1MOP3D14             equ     446Bh
_OP2EXOP6               equ     446Fh ;swap OP2,OP6
_ex_op2_op6		equ	446Fh ;swap OP2,OP6
_OP5EXOP6               equ     4473h ;swap OP5,OP6
_ex_op5_op6             equ     4473h ;swap OP5,OP6
_OP1EXOP5               equ     4477h ;swap OP1,OP5
_ex_op1_op5             equ     4477h ;swap OP1,OP5
_OP1EXOP6               equ     447Bh ;swap OP1,OP6
_ex_op1_op6             equ     447Bh ;swap OP1,OP6
_OP2EXOP4               equ     447Fh ;swap OP2,OP4
_ex_op2_op4             equ     447Fh ;swap OP2,OP4
_OP2EXOP5               equ     4483h ;swap OP2,OP5
_ex_op2_op5             equ     4483h ;swap OP2,OP5
_OP1EXOP3               equ     4487h ;swap OP1,OP3
_ex_op1_op3             equ     4487h ;swap OP1,OP3
_OP1EXOP4               equ     448Bh ;swap OP1,OP4
_ex_op1_op4             equ     448Bh ;swap OP1,OP4
_OP1EXOP2               equ     448Fh ;swap OP1,OP2
_ex_op1_op2             equ     448Fh ;swap OP1,OP2
_EXLP                   equ     4493h ;swap B bytes (HL),(DE)
_CKOP1C0                equ     4497h ;cp (OP1,OP2) to 0
_CKOP1FP0               equ     449Bh ;cp OP1,0
_CKFP0                  equ     449Fh
_CKOP2FP0               equ     44A3h ;cp OP2,0
_POSRNO0INT             equ     44A7h
_POSNO0INT              equ     44ABh
_CKPOSINT               equ     44AFh ;set ZF if OP1 is positive fp int
_CKOP1INT               equ     44B3h ;set ZF if OP1 is fp int
_CKINT                  equ     44B7h ;set ZF if 16-dig (HL) is fp int
_CKOP1ODD               equ     44BBh ;set ZF if OP1 is even
_CKODD                  equ     44BFh ;set ZF if (HL) is even
_CKOP1M1                equ     44C3h
_CKOP1M0                equ     44C7h
_CKOP1M                 equ     44CBh
_CK15ZERO               equ     44CFh
_CKZERO                 equ     44D3h
_CK0S                   equ     44D7h
_LDCCONB                equ     44DBh
_GETCONB                equ     44DFh
_GETCON1                equ     44E3h
_GETCON                 equ     44E7h
_PIDIV2                 equ     44EBh
_PIDIV4                 equ     44EFh
_TWOPI                  equ     44F3h
_PICON                  equ     44F7h
_PIDIV4A                equ     44FBh
_PIDIV2A                equ     44FFh
_OP1EXPTODEC            equ     4503h
_DECTOHEX               equ     4507h ;BCD (HL) to word A=DE
_CKOP2POS               equ     450Bh ;test if OP2 positive
_CKOP1POS               equ     450Fh ;test if OP1 positive
_CLROP2S                equ     4513h ;OP2=abs(OP2)
_absop2                 equ     4513h ;OP2=abs(OP2)
_CLROP1S                equ     4517h ;OP1=abs(OP1)
_FDIV100                equ     451Bh ;OP1=OP1/100 if OP1 is not 0
_FDIV10                 equ     451Fh ;OP1=OP1/10 if OP1 is not 0
_DECO1EXP               equ     4523h ;OP1=OP1/10
_INCO1EXP               equ     4527h ;OP1=OP1*10
_INCEXP                 equ     452Bh ;OP(HL)=OP(HL)*10
_CKVALIDNUM             equ     452Fh
_GETEXP                 equ     4533h
_GETEXP1                equ     4537h
_ISDOLLARPO1            equ     453Bh ;cp (_OP1+2),$24; result in HL
_ISATEMPO1              equ     453Fh
_ISATEMP                equ     4543h
_HTIMESL                equ     4547h ;HL=H*L
_OP1STATLST             equ     454Bh ;load '.' var name to OP1
_OP1PREGC               equ     454Fh ;load "PRegC" var name to OP1
_EOP1NOTRC              equ     4553h ;error 10 DATA TYPE if OP1 isn't real or cplx
_EOP1NOTREAL            equ     4557h ;error 10 DATA TYPE if OP1 isn't real
_EXCNAME                equ     455Bh ;load '!' var name to OP1
_LBNAME                 equ     455Fh ;load '#' var name to OP1
_ATNAME                 equ     4563h ;load '@' var name to OP1
_ASTNAME                equ     4567h ;load '*' var name to OP1
_THETANAME              equ     456Bh ;load theta  var name to OP1
_RNAME                  equ     456Fh ;load 'r' var name to OP1
_XNAME                  equ     4573h ;load 'x' var name to OP1
_YNAME                  equ     4577h ;load 'y' var name to OP1
_TNAME                  equ     457Bh ;load 't' var name to OP1
_SETesTOfps             equ     457Fh ;set _ES to _FPS
_CHKTEMPDIRT            equ     4583h
_ISO1XYFSTAT            equ     4587h
_ISO2XYFSTAT            equ     458Bh
_CHECKGRAPH             equ     458Fh
_findsym_error          equ     458Fh
_CHECKGRAPH1            equ     4593h
_CHKMATTYPE             equ     4597h ;bit 0,(iy+0)
_SETMATCPLX             equ     459Bh ;set 0,(iy+0)
_OP1MOP2EXP             equ     459Fh
_OP1EXPMEXP1            equ     45A3h
_OP1EXPMDE              equ     45A7h
_OP1EXPMDE1             equ     45ABh
_OP1EXPMEXPM1           equ     45AFh
_CKEXP12                equ     45B3h
_CHKNUMOP1ERR           equ     45B7h ;error 7 SYNTAX if bit numop1,(iy+parsflag2) is clear
_CHKERRBREAK            equ     45BBh ;check for break
_BITSTAT1               equ     45BFh ;bit 5,(iy+statflags)
_RESMATTYPE             equ     45C3h ;res 0,(iy+0)
_ERRD_OP1NOTPOS         equ     45C7h ;error 4 DOMAIN if OP1<0
_ERRD_OP1NOT_R          equ     45CBh ;error 4 DOMAIN if OP1 not real
_ERRD_OP1NOTPOSINT      equ     45CFh ;error 4 DOMAIN if OP1 is not positive int
_ERRD_OP1_LE_0          equ     45D3h ;error 4 DOMAIN if OP1<=0
_ERRD_OP1_0             equ     45D7h ;error 4 DOMAIN if OP1=0
_EQU_or_NEWequ          equ     45DBh
_SMARTER_ACTIVE         equ     45DFh
_INVSUB                 equ     45E3h
_TIMES2                 equ     45E7h ;OP1=OP1*2, OP2=old OP1
_PLUS1                  equ     45EBh ;OP1=OP1+1
_inct_ade               equ     45EFh ;ADE=ADE+1
_swapt_                 equ     45F3h ;swap AHL,BDE
_ex_ahl_bde             equ     45F3h ;swap AHL,BDE
_ldtir_ade              equ     45F7h ;load (HL),(ADE)
_ldtir_bde              equ     45FBh ;load (HL),(BDE)
_ldtir_ahl              equ     45FFh ;load (HL),(AHL)
_sttir_ade              equ     4603h ;load (ADE),(HL)
_sttir_bde              equ     4607h ;load (BDE),(HL)
_DATA_SIZE_TO_DE        equ     460Bh ;get size of var from VAT entry at HL in DE
_get_size_word          equ     460Bh ;get size of var from VAT entry at HL in DE
_LOAD_EDITSYM_ASIC      equ     460Fh ;load _P_editSym to HL
_SetXXOP1               equ     4613h ;convert hex # in A to fp in OP1 (max 99)
_SetXXOP2               equ     4617h ;convert hex # in A to fp in OP2 (max 99)
_SetXXXXOP2             equ     461Bh ;convert hex # in HL to fp in OP2
_ATODEC                 equ     461Fh ;A hex to BCD
_reset_currlast         equ     4623h ;load 0 to _currlastentry
_GETLASTENTRY           equ     4627h
_GETLASTENTRYPTR        equ     462Bh
_load_ram_ahl           equ     462Fh ;ABS(AHL) to ASIC(HL) and PAGE swapped
_abs_to_page_asic       equ     4633h ;ABS(AHL) to ASIC(HL) and PAGE(A)
_conv_ahl               equ     4633h ;ABS(AHL) to ASIC(HL) and PAGE(A)
_INC_PTR_AHL            equ     4637h ;AHL=AHL+1
_dec_ptr_ahl            equ     463Bh ;AHL=AHL-2
_INC_PTR_BDE            equ     463Fh ;BDE=BDE+1
_DEC_PTR_BDE            equ     4643h ;BDE=BDE-2
_SET_ABS_SRC_ADDR       equ     4647h ;AHL->(_ABS_SRC_ADDR)
_LOAD_FREE_MEM          equ     464Bh ;(_FREE_MEM)->AHL
_get_free_mem           equ     464Bh ;(_FREE_MEM)->AHL
_SET_MM_NUM_BYTES       equ     464Fh ;AHL->(_MM_NUM_BYTES)
_LJRND                  equ     4653h
_LJRND2A                equ     4657h
_round_OP1              equ     4657h
_LJUNDERF               equ     465Bh
_DelResLst              equ     465Fh
_Delres                 equ     4663h ;reset _STATVARS,PRegC,RegEq
_chkgraphprog           equ     4667h ;bit 1,(iy+graphFlags)
_eqn                    equ     466Bh
_exp                    equ     466Fh
_expName                equ     4673h
_defaultLimits          equ     4677h ;.db 8063FC10000000000000 (-1E99)
_ONE_E99                equ     467Bh ;.db 0063FC10000000000000 (1E99)
_XAXISDIF_TO_ACC        equ     467Fh
_YAXISDIF_TO_ACC        equ     4683h
_fldpicname             equ     4687h ;load "FldPic" to OP1
_chk_new_size           equ     468Bh ;get total memory var takes.  ld to OP1 and findsym first
_FIND_PARSE_FORMULA     equ     468Fh
_PARSE_FORMULA          equ     4693h
_GET_FORM_NUM           equ     4697h
_chktextcurs            equ     469Bh
_SYSEQNAM               equ     469Fh
_SYSEQNAM_HL            equ     46A3h
_POINT5                 equ     46A7h ;.db 00FFFB5000000000000000 (.5)
_ok_to_edit_prog        equ     46ABh ;check prog at bde; return nz if can be parsed for editing
_HAVE_EXTEND            equ     46AFh ;check prog at ahl if have extend token
_check_asm              equ     46AFh ;check prog at ahl if have extend token
_skip_asap_tok          equ     46B3h
_EXEC_ASAP_IND          equ     46B7h ;jump table  hl->table  _asap_ind=index
_jump_table             equ     46B7h ;jump table hl->table  _asap_ind=index
_MEMCHK                 equ     46BBh ;AHL = # bytes of free memory
_DEC_PTR_ADE            equ     46BFh ;ADE = ADE - 1
_GETB_AHL               equ     46C3h ;A = (AHL), HL=asic
_AHL_CMP_BDE            equ     46C7h ;cp AHL,BDE
_cp_ahl_bde             equ     46C7h ;cp AHL,BDE
_FINDSYM                equ     46CBh ;look up var OP1
_INSERTMEM              equ     46CFh
_INSERTMEMA             equ     46D3h
_copy_fwd               equ     46D3h
_CLEANALL               equ     46D7h ;delete all temp vars
_del_temp_vars          equ     46D7h ;delete all temp vars
_ENOUGHMEM              equ     46DBh ;check if enough free mem, hl=amount (deletes temp vars if needed, CF set if not enough memory)
_CTEMP5                 equ     46DFh
_CMPMEMNEED             equ     46E3h
_CREATEVAR3             equ     46E7h
_CREATEREAL             equ     46EBh ;create real var OP1
_CREATERCONST           equ     46EFh
_CREATECCONST           equ     46F3h
_CREATECPLX             equ     46F7h ;create cplx var OP1
_CREATETRVECT           equ     46FBh
_creatervect_temp       equ     46FBh
_CREATERVECT            equ     46FFh
_CREATETCVECT           equ     4703h
_createcvect_temp       equ     4703h
_CREATECVECT            equ     4707h
_CREATETRLIST           equ     470Bh
_createrlist_temp       equ     470Bh
_CREATERLIST            equ     470Fh
_CREATETCLIST           equ     4713h
_createclist_temp       equ     4713h
_CREATECLIST            equ     4717h
_CREATETRMAT            equ     471Bh
_creatermat_temp        equ     471Bh
_CREATERMAT             equ     471Fh
_CREATETCMAT            equ     4723h
_createcmat_temp        equ     4723h
_CREATECMAT             equ     4727h
_CREATETSTRNG           equ     472Bh
_createstrng_temp       equ     472Bh
_CREATESTRNG            equ     472Fh
_CREATETequ             equ     4733h
_createequ_temp         equ     4733h
_CREATEequ              equ     4737h
_CREATEPICT             equ     473Bh
_CREATESTDB             equ     473Fh
_CREATEPODB             equ     4743h
_CREATEPADB             equ     4747h
_CREATEDEDB             equ     474Bh
_CREATEPROG             equ     474Fh
_CHKDEL                 equ     4753h
_CHKDELA                equ     4757h
_DELMEMA                equ     475Bh
_copy_bkwd              equ     475Bh
_delvar                 equ     475Fh ;delete var
_DELVARIO               equ     4763h
_DELMEM                 equ     4767h
_DELVAR3D               equ     476Bh
_DELVAR3C               equ     476Fh
_update_VAT_ptrs        equ     476Fh
_DELVAR3C0              equ     4773h
_DELVAR3DC              equ     4777h
_datasizea              equ     477Bh
_get_size               equ     477Bh
_DATASIZE               equ     477Fh
_get_var_size           equ     477Fh
_PUSH2BOPER             equ     4783h ;push BC to OPS
_push_bc_OPS            equ     4783h ;push BC to OPS
_CHKMEM_PG1             equ     4787h ;check STACK mem, HL=memory needed
_check_STACK_mem        equ     4787h ;check STACK mem, HL=memory needed
_POP2BOPER              equ     478Bh ;pop BC off OPS
_pop_bc_OPS             equ     478Bh ;pop BC off OPS
_PUSHOPER               equ     478Fh ;push A to OPS (operation stack)
_push_a_OPS             equ     478Fh ;push A to OPS (operation stack)
_POPOPER                equ     4793h ;pop A off OPS
_pop_a_OPS              equ     4793h ;pop A off OPS
_POPOP5                 equ     4797h
_POPOP3                 equ     479Bh
_POPOP1                 equ     479Fh
_POPREALO6              equ     47A3h ;pop 10 bytes off FP stack to OP6
_POPREALO5              equ     47A7h ;pop 10 bytes off FP stack to OP5
_POPREALO4              equ     47ABh ;pop 10 bytes off FP stack to OP4
_POPREALO3              equ     47AFh ;pop 10 bytes off FP stack to OP3
_POPREALO2              equ     47B3h ;pop 10 bytes off FP stack to OP2
_POPREALO1              equ     47B7h ;pop 10 bytes off FP stack to OP1
_POPREAL                equ     47BBh ;pop 10 bytes off FP stack to (de)
_POPMCPLXO1             equ     47BFh
_POPMCPLX               equ     47C3h
_MOVCPLX                equ     47C7h
_FPOPCPLX               equ     47CBh
_sub_FPS_20             equ     47CBh
_FPOPREAL               equ     47CFh
_sub_FPS_10             equ     47CFh
_FPOPFPS                equ     47D3h
_sub_FPS_bc             equ     47D3h
_DEALLOCFPS             equ     47D7h
_DEALLOCFPS1            equ     47DBh
_PAGE_1_TO_ASIC         equ     47DFh
_RAM_PAGE_1             equ     47E3h ;swap RAM page 1 (FPS and OPS) to $8000-$BFFF area
_LOAD_OPS_ASIC          equ     47E7h
_load_ram_OPS           equ     47E7h
_LOAD_ES_ASIC           equ     47EBh
_load_ram_ES            equ     47EBh
_LOAD_FPS_ASIC          equ     47EFh
_load_ram_FPS           equ     47EFh
_RAM_PAGE_7             equ     47F3h ;swap RAM page 7 (symbol table) to $8000-$BFFF area
_ALLOCFPS               equ     47F7h
_ALLOCFPS1              equ     47FBh
_PUSHREALO6             equ     47FFh ;push 10 bytes OP6 onto FP stack
_PUSHREALO5             equ     4803h ;push 10 bytes OP5 onto FP stack
_PUSHREALO4             equ     4807h ;push 10 bytes OP4 onto FP stack
_PUSHREALO3             equ     480Bh ;push 10 bytes OP3 onto FP stack
_PUSHREALO2             equ     480Fh ;push 10 bytes OP2 onto FP stack
_PUSHREALO1             equ     4813h ;push 10 bytes OP1 onto FP stack
_PUSHREAL               equ     4817h ;push 10 bytes (HL) onto FP stack
_PUSHOP5                equ     481Bh
_PUSHOP3                equ     481Fh
_PUSHMCPLXO3            equ     4823h
_PUSHOP1                equ     4827h
_PUSHMCPLXO1            equ     482Bh
_PUSHMCPLX              equ     482Fh
_EXMCPLXO1              equ     4833h
_EXREALO1               equ     4837h
_CPYTO1FPS11            equ     483Bh
_CPYTO1FPS6             equ     483Fh
_CPYFPS6                equ     4843h
_CPYTO1FPS8             equ     4847h
_CPYCTO1FPS5            equ     484Bh
_CPYTO1FPS5             equ     484Fh
_CPYFPS5                equ     4853h
_CPYTO2FPS4             equ     4857h
_CPYTO2FPS5             equ     485Bh
_CPYTO6FPS3             equ     485Fh
_CPYTO6FPS2             equ     4863h
_CPYTO2FPS3             equ     4867h
_CPYCTO1FPS3            equ     486Bh
_CPYTO1FPS3             equ     486Fh
_CPYFPS3                equ     4873h
_CPYS3                  equ     4877h
_CPYTO1FPS4             equ     487Bh
_CPYTO3FPS2             equ     487Fh
_CPYTO5FPST             equ     4883h
_CPYTO6FPST             equ     4887h
_CPYTO4FPST             equ     488Bh
_CPYTO3FPST             equ     488Fh
_CPYTO2FPST             equ     4893h
_CPYTO1FPST             equ     4897h
_CPYFPST                equ     489Bh
_CPYSTOP                equ     489Fh
_CPYSTACK               equ     48A3h
_CPYCTO3FPS1            equ     48A7h
_CPYTO3FPS1             equ     48ABh
_CPYTO2FPS1             equ     48AFh
_CPYCTO1FPS1            equ     48B3h
_CPYTO1FPS1             equ     48B7h
_CPYFPS1                equ     48BBh
_CPYS1                  equ     48BFh
_CPYTO2FPS2             equ     48C3h
_CPYTO1FPS2             equ     48C7h
_CPYFPS2                equ     48CBh
_CPYS2                  equ     48CFh
_CPYO3TOFPST            equ     48D3h
_CPYO2TOFPST            equ     48D7h
_CPYO6TOFPST            equ     48DBh
_CPYO1TOFPST            equ     48DFh
_CPYTOFPST              equ     48E3h
_cpydetofpst            equ     48E3h
_CPYTOSTOP              equ     48E7h
_cpydetohlt             equ     48E7h
_CPYTOSTACK             equ     48EBh
_cpydetohlc             equ     48EBh
_CPYO5TOFPS1            equ     48EFh
_cpyo5tofps2            equ     48EFh
_CPYCO1TOFPS1           equ     48F3h
_cpyo2tofpsto1tofps1    equ     48F3h
_CPYO1TOFPS1            equ     48F7h
_CPYTOFPS1              equ     48FBh
_cpydetofps1            equ     48FBh
_CPYTOS1                equ     48FFh
_cpydetohl1             equ     48FFh
_CPYO2TOFPS2            equ     4903h
_CPYO3TOFPS2            equ     4907h
_CPYO6TOFPS2            equ     490Bh
_CPYO1TOFPS2            equ     490Fh
_CPYTOFPS2              equ     4913h
_cpydetofps2            equ     4913h
_CPYTOS2                equ     4917h
_cpydetohl2             equ     4917h
_CPYO5TOFPS3            equ     491Bh
_CPYCO1TOFPS3           equ     491Fh
_cpyo2tofps2o1tofps3    equ     491Fh
_CPYO1TOFPS3            equ     4923h
_CPYTOFPS3              equ     4927h
_cpydetofps3            equ     4927h
_CPYTOS3                equ     492Bh
_cpydetohl3             equ     492Bh
_CPYO1TOFPS4            equ     492Fh
_CPYTOFPS4              equ     4933h
_cpydetofps4            equ     4933h
_CPYTOS4                equ     4937h
_cpydetohl4             equ     4937h
_CPYO1TOFPS6            equ     493Bh
_CPYO1TOFPS7            equ     493Fh
_CPYO1TOFPS8            equ     4943h
_ERRNOTENOUGHMEM        equ     4947h ;ERROR 15 MEMORY if less than HL bytes free
_FPSMINUS10             equ     494Bh
_selfTest               equ     494Fh ;rom version, self test screen
_ask_self_test          equ     494Fh ;rom version, self test screen
_ROMTest                equ     4953h ;perform ROM test
_self_test              equ     4953h ;perform ROM test
_strLength              equ     4957h ;BC = length of string (HL)
_strlen                 equ     4957h ;BC = length of string (HL)
_strCopy                equ     495Bh ;copy ZT str (HL) -> (DE)
_strcpy                 equ     495Bh ;copy ZT str (HL) -> (DE)
_strCat                 equ     495Fh ;concatenate str (HL) and (DE)
_pstrCmp                equ     4963h ;compare length preceded strings (HL) and (DE)
_strcmp                 equ     4963h ;compare length preceded strings (HL) and (DE)
_ISINFORMSET            equ     4967h
_isInSet                equ     496Bh ;hl+=a/8 a=1<<(a%8)
_find_bit               equ     496Bh ;hl+=a/8 a=1<<(a%8)
___bank_call            equ     496Fh
___bank_ret             equ     4973h
___bank_jump            equ     4976h
___bank_entry           equ     4979h
_JcatApp                equ     497Dh ;CATALOG
_CustomInit             equ     4980h
_lcustom                equ     4984h
_curBlink               equ     4988h ;blink cursor
_cursorOff              equ     498Ch ;turn blinking cursor off
_hideCursor             equ     4990h ;hide cursor
_cursorOn               equ     4994h ;turn blinking cursor on
_showCursor             equ     4998h ;show cursor
_init_math_menu         equ     499Ch ;init MATH menu
_bkup_init_math         equ     49A0h
_reset_MATH             equ     49A0h
_RstrSmallText          equ     49A4h
_installAppMenu         equ     49A8h
_changeGrMenu           equ     49ACh
_installGrMenu          equ     49B0h
_disp_GRAPH             equ     49B0h
_menuPress              equ     49B4h
_installMenu            equ     49B8h
_loadGrMenu             equ     49BCh
_loadAppMenu            equ     49C0h
_loadMenu               equ     49C4h ;HL->menu structure
_ASAPloadmenu           equ     49C8h
_hideMenu               equ     49CCh
_popSys                 equ     49D0h
_popApp                 equ     49D4h
_ckNamesMenu            equ     49D8h
_flushAllMenus          equ     49DCh ;clear the menu keys stacks
_flushSys               equ     49E0h
_flushApp               equ     49E4h
_funcDisp               equ     49E8h
_disp_menu              equ     49E8h
_DispMenus              equ     49ECh
_computeWinBtm          equ     49F0h
_nextKey                equ     49F4h
_getMenuPtr             equ     49F7h
_getMenuPtrL7           equ     49FBh
_aGrMenu                equ     49FFh
_FindConvCode           equ     4A02h
_mStatNames             equ     4A06h
_exec_mode              equ     4A0Ah
_exec_pg1               equ     4A0Ah
_JmodeApp               equ     4A0Eh ;MODE
_JformtApp              equ     4A11h ;GRAPH:FORMT
_statplotmodeapp        equ     4A14h
_statplotsetdisp        equ     4A18h
_JTbl1ModeApp           equ     4A1Ch
_TblSet1Disp            equ     4A1Fh ;indpnt state "auto/ask"
_SetIndDep              equ     4A23h ;print "Indpnt:" at row a, col 0
_putmap                 equ     4A27h ;print char A
_putc                   equ     4A2Bh ;print char A, advance cursor
_putb                   equ     4A2Fh
_dispHL                 equ     4A33h ;print AHL dec
_puts                   equ     4A37h ;print string (HL)
_putps                  equ     4A3Bh ;print length preceded string (HL)
_putbuf                 equ     4A3Fh
_wputc                  equ     4A43h
_wputs                  equ     4A47h
_wputsEOL               equ     4A4Bh
_wdispEOL               equ     4A4Fh
_whomeUp                equ     4A53h
_setNumWindow           equ     4A57h
_checkCurRow            equ     4A5Bh
_newline                equ     4A5Fh ;move cursor to next line
_lineUp                 equ     4A63h
_moveDown               equ     4A67h
_scrollUp               equ     4A6Bh
_JshrinkWindow          equ     4A6Fh
_shrinkWindow           equ     4A72h
_moveUp                 equ     4A76h
_scrollDown             equ     4A7Ah
_clrLCD                 equ     4A7Eh ;clear video ram
_clrScrn                equ     4A82h ;clear video ram, clear _textShadow if it should
_clrWindow              equ     4A86h ;clear between _winTop and _winBtm
_eraseEOL               equ     4A8Ah ;clear until end of line
_JeraseEOW              equ     4A8Eh
_eraseEOW               equ     4A91h ;clear until end of window
_homeUp                 equ     4A95h ;cursor to top left of home screen
_getcurloc              equ     4A99h
_vputmap_g              equ     4A9Dh ;_vputmap, check for menus
_vputmap                equ     4AA1h ;print char A, small font, advance cursor
_vputs                  equ     4AA5h ;print string (HL), small font
_vputsn                 equ     4AA9h
_runIndicOn             equ     4AADh ;turn busy indicator on
_runIndicOff            equ     4AB1h ;turn busy indicator off
_saveCmdShadow          equ     4AB5h
_clrText                equ     4AB5h
_saveShadow             equ     4AB9h
_JsaveShadow            equ     4ABDh
_JrstrShadow            equ     4AC0h
_rstrShadow             equ     4AC3h ;copy text shadow to screen
_rstrPartial            equ     4AC7h
_rstrCurRow             equ     4ACBh
_rstrUnderMenu          equ     4ACFh
_saveTR                 equ     4AD3h
_restoretr              equ     4AD7h
_accent                 equ     4ADBh
_CSUB                   equ     4ADFh ;cplx SUB
_CADD                   equ     4AE3h ;cplx ADD
_CSQUARE                equ     4AE7h ;cplx SQUARE
_CMULT                  equ     4AEBh ;cplx MULT
_CDIV                   equ     4AEFh ;cplx DIV
_CABS                   equ     4AF3h ;cplx ABS
_CABS1                  equ     4AF7h
_CSQROOT                equ     4AFBh ;cplx SQROOT
_CACOSH                 equ     4AFFh ;cplx ACOSH
_CACOS                  equ     4B03h ;cplx ACOS
_CASIN                  equ     4B07h ;cplx ASIN
_CATANH                 equ     4B0Bh ;cplx ATANH
_CONJ                   equ     4B0Fh
_CMLTBYREAL             equ     4B13h
_CDIVBYREAL             equ     4B17h
_BINOPEXEC              equ     4B1Bh
_exec_pg2               equ     4B1Bh
_BINOPEXEC1             equ     4B1Fh
_SET2MVLPTRS            equ     4B23h
_SETMAT1                equ     4B27h
_SETUNOPMAT2            equ     4B2Bh
_CREATETLIST            equ     4B2Fh
_LIST_CONVERT           equ     4B33h
_UNOPEXEC               equ     4B37h
_MARKTEMPS              equ     4B3Bh
_THREEEXEC              equ     4B3Fh
_FOUREXEC               equ     4B43h
_FIVEEXEC               equ     4B47h
_EXMEAN1                equ     4B4Bh
_fofx3A                 equ     4B4Fh
_ADRLELE                equ     4B53h
_GETL1TOOP1             equ     4B57h
_GETL1TOP1A             equ     4B5Bh
_GETLTOOP1              equ     4B5Fh
_GETL1TOOP2             equ     4B63h
_GETL1TOP2A             equ     4B67h
_GETL2TOOP1             equ     4B6Bh
_GETL2TOP1A             equ     4B6Fh
_PUTTOLA                equ     4B73h
_PUTTOLA1               equ     4B77h
_PUTTOL                 equ     4B7Bh
_LLOW                   equ     4B7Fh
_LHIGH                  equ     4B83h
_LSUM                   equ     4B87h
_LISTCONST              equ     4B8Bh
_CLR_ALL_LST            equ     4B8Fh
_TOFRAC                 equ     4B93h ;convert OP1 to fraction OP1/OP2
_INTERPOLATE            equ     4B97h
_ARC                    equ     4B9Bh
_GFUDYDX                equ     4B9Fh
_GPODRDo                equ     4BA3h
_GPAD?DT                equ     4BA7h
_GPODYDX                equ     4BABh
_GPADYDX                equ     4BAFh
_ADRMROW                equ     4BB3h
_ADRMELE                equ     4BB7h
_GETMATOP1A             equ     4BBBh
_GETM1TOOP1             equ     4BBFh
_GETM1TOP1A             equ     4BC3h
_GETMTOOP1              equ     4BC7h
_GETM1TOP2A             equ     4BCBh
_GETMTOOP2              equ     4BCFh
_PUTTOM1A               equ     4BD3h
_PUTTOMA                equ     4BD7h
_PUTTOMA1               equ     4BDBh
_PUTTOMAT               equ     4BDFh
_STOSETNEXT             equ     4BE3h
_MOP1SET0               equ     4BE7h
_IDNMAT                 equ     4BEBh
_MINVOP1S               equ     4BEFh
_CYLTOREC               equ     4BF3h
_POLTOREC               equ     4BF7h
_SPHTOREC               equ     4BFBh
_MATCONST               equ     4BFFh
_MCKOP1FP0              equ     4C03h
_ROWECH_POLY            equ     4C07h
_FORSEQINIT             equ     4C0Bh
_INC_ERRNOLPAREN        equ     4C0Fh ;expect '(' at _P_CURPC
_set_curpc              equ     4C13h ;AHL->(_P_CURPC)
_PARENSYN               equ     4C17h
_PARENSYNB              equ     4C1Bh
_BC2NONREAL             equ     4C1Fh
_ERRNONREAL             equ     4C23h
_READ_LIST_ONLY         equ     4C27h
_READ_LIST_ONLY_P       equ     4C2Bh
_INTOP1                 equ     4C2Fh
_PARSEINC               equ     4C33h
_PARSEarg               equ     4C37h
_LOAD_ABS_DEST_ADDR     equ     4C3Bh ;(_ABS_DEST_ADDR)->AHL
_AHL_PLUS_2_PG3         equ     4C3Fh ;AHL=AHL+2
_GRPHPARS               equ     4C43h
_PLOTPARS               equ     4C47h
_exec_basic             equ     4C47h ;execute basic program in OP1
_CKPLOTRES              equ     4C4Bh
_EXECQPA                equ     4C4Fh
_PARSEON                equ     4C53h
_PARSEINP               equ     4C57h
_PARSEOFF               equ     4C5Bh
_PARSEAXIS              equ     4C5Fh
_PARSESCAN              equ     4C63h
_GETPARSE               equ     4C67h
_SAVEPARSE              equ     4C6Bh
_INITPFLGS              equ     4C6Fh
_INITFFLGS              equ     4C73h
_SET_CURPC_BDE          equ     4C77h ;BDE->(_P_CURPC)
_CKENDLINERR            equ     4C7Bh
_OP2SET60               equ     4C7Fh ;OP2=60
_FIND_SET_PTR1_3        equ     4C83h
_SET_PTR1_3             equ     4C87h
_SET_CHKDELPTR1_BDE     equ     4C8Bh ;BDE->(_CHKDELPTR1)
_RCLSTAT                equ     4C8Fh
_RCLCONST               equ     4C93h
_VARSYSADR              equ     4C97h
_stosystok              equ     4C9Bh
_STOANS                 equ     4C9Fh ;store OP1 to 'Ans' var
_STOQA                  equ     4CA3h
_STOOTHER_form          equ     4CA7h
_STOTHETA               equ     4CABh ;store OP1 to theta var
_STOR                   equ     4CAFh ;store OP1 to 'r' var
_STOY                   equ     4CB3h ;store OP1 to 'y' var
_STOT                   equ     4CB7h ;store OP1 to 't' var
_STOX                   equ     4CBBh ;store OP1 to 'x' var
_STOOTHER               equ     4CBFh
_VAR12A                 equ     4CC3h
_STOTYPE                equ     4CC7h
_STOTYPEC               equ     4CCBh
_SYSEQSEL               equ     4CCFh
_RCLQIA                 equ     4CD3h
_RCLQA                  equ     4CD7h
_RCLANS                 equ     4CDBh ;rcl 'Ans' to OP1
_RCLY                   equ     4CDFh ;rcl 'y' to OP1
_RCLX                   equ     4CE3h ;rcl 'x' to OP1
_RCLVARSYM              equ     4CE7h
_RCLSYSTOK              equ     4CEBh
_StMatEl                equ     4CEFh
_StLstVecEl             equ     4CF3h
_ERRIFBASE              equ     4CF7h
_INCCURPCERREND         equ     4CFBh
_COMMAERRF              equ     4CFFh
_COMMAERR               equ     4D03h
_STEQARG2               equ     4D07h
_STEQARG                equ     4D0Bh
_BRKINC                 equ     4D0Fh
_incfetch               equ     4D13h ;get byte from _P_CURPC to A
_get_token              equ     4D13h ;get byte from _P_CURPC to A
_TOPOP                  equ     4D17h
_CKFETCHVAR             equ     4D1Bh ;get var name from _P_CURPC (next byte in a, name in OP1)
_get_varname            equ     4D1Bh ;get var name from _P_CURPC (next byte in a, name in OP1)
_FETCHVARA              equ     4D1Fh
_fetchvar               equ     4D23h
_ckendlin               equ     4D27h ;set zf if end of command at _P_CURPC
_grvalvar1              equ     4D2Bh
_DEL_LIST_FORM          equ     4D2Fh
_HAVE_QIVAR             equ     4D33h
_HAVE_QVAR              equ     4D37h
_delPlusName            equ     4D3Bh
_disp                   equ     4D3Fh
_PAUSE                  equ     4D43h ;wait until ENTER pushed
_XYRNDBOTH              equ     4D47h
_XYRND                  equ     4D4Bh
_CheckTOP               equ     4D4Fh
_CheckXY                equ     4D53h
_DARKPNT                equ     4D57h
_CPointS                equ     4D5Bh
_WtoV                   equ     4D5Fh
_XItoF                  equ     4D63h
_YFtoI                  equ     4D67h
_XFtoI                  equ     4D6Bh
_PDspGrph               equ     4D6Fh
_HORIZCMD               equ     4D73h ;draw horiz line at OP1=y
_VERTCMD                equ     4D77h ;draw vert line at OP1=x
_LINECMD                equ     4D7Bh
_POINTCMD               equ     4D7Fh
_PIXELTEST              equ     4D83h
_PIXELCMD               equ     4D87h
_TANLNF                 equ     4D8Bh
_DRAWCMD                equ     4D8Fh
_SHADECMD               equ     4D93h
_INVCMD                 equ     4D97h
_CIRCCMD                equ     4D9Bh
_LOAD_XOUTDAT_GR        equ     4D9Fh
_LOAD_YOUTDAT_GR        equ     4DA3h
_LOAD_INPUTDAT_GR       equ     4DA7h
_putEquNo               equ     4DABh ;vput A in upper right corner, <100 only
_divAby10               equ     4DAFh ;A=A/10
_unpack_hex             equ     4DAFh ;a(hex) -> bc(dec)
_DISP_DREQ_PRMPT        equ     4DB3h
_JgraphStat             equ     4DB7h
_JgraphApp              equ     4DBAh
_TraceOff               equ     4DBDh
_GRAPH_DSP_CHAR         equ     4DC1h
_GRDECODA               equ     4DC5h
_LABCOOR                equ     4DC9h
_COORDISP               equ     4DCDh
_CKEVALEQ               equ     4DD1h
_EVALVARINIT            equ     4DD5h
_EVAL_DIF               equ     4DD9h
_GRLABELS               equ     4DDDh
_YPIXSET                equ     4DE1h
_XPIXSET                equ     4DE5h
_COPYRNG                equ     4DE9h
_VALCUR                 equ     4DEDh
_grputaway_mon          equ     4DF1h
_RSTGFLAGS              equ     4DF5h
_grReset                equ     4DF9h
_XYCENT                 equ     4DFDh
_ZOOMXYCMD              equ     4E01h
_ZFRNG                  equ     4E05h
_CKVALDELTA             equ     4E09h
_SETFUNCM               equ     4E0Dh ;set func graph mode
_SETPOLM                equ     4E11h ;set polar
_SETPARM                equ     4E15h ;set param
_SETDIFM                equ     4E19h ;set recur
_ZMDECML                equ     4E1Dh
_ZMPREV                 equ     4E21h
_ZMUSR                  equ     4E25h
_ZMSQUARE               equ     4E29h
_ZMTRIG                 equ     4E2Dh
_USERDEFAULT            equ     4E31h
_ZooDefault             equ     4E35h
_GRBUFCPY               equ     4E39h
_BLINKGCUR              equ     4E3Dh
_NBCURSOR               equ     4E41h
_STATMARK               equ     4E45h
_YBar                   equ     4E49h
_DARKLINE               equ     4E4Dh ;ld h,1 / _ILine
_ILine                  equ     4E51h ;draw an unclipped line from (B,C) to (D,E) H=0:res,1:set,2:xor
_POINTON                equ     4E55h ;ld d,1 / _IPoint
_IPoint                 equ     4E59h ;change point (B,C) on the screen D=0:res 1:set 2:xor 3:test 4:copy from buffer
_fstatname              equ     4E5Dh ;load 'fStat' var name to OP1
_yStatName              equ     4E61h ;load 'yStat' var name to OP1
_xStatName              equ     4E65h ;load 'xStat' var name to OP1
_STATNAME               equ     4E69h ;load a'Stat' var name to OP1 (a='3':x,'4':y,'5':f)
_EQNNAME                equ     4E6Dh
_GEQNAMEA               equ     4E71h ;OP1 = name of current EQ
_GEQNAME                equ     4E75h
_QPANAME                equ     4E79h
_QIANAME                equ     4E7Dh
_QANAME                 equ     4E81h
_RegeqName              equ     4E85h ;load 'RegEq' var name to OP1
_ReGraph                equ     4E89h
_REGRAPH1               equ     4E8Dh
_DOREFFLAGS02           equ     4E91h
_INITDNEQ               equ     4E95h
_INITDNEQ1              equ     4E99h
_INPUTDAT_TOOP2_GR      equ     4E9Dh
_NEXTTDIF               equ     4EA1h
_INPTRS_GR              equ     4EA5h
_SET_INPUTDAT_GR        equ     4EA9h
_ResMem                 equ     4EADh ;MEM:RESET:MEM
_ResDef                 equ     4EB0h ;MEM:RESET:DFLTS
_ResAll                 equ     4EB3h ;MEM:RESET:ALL
_DFMAX                  equ     4EB6h
_DFMIN                  equ     4EBAh
_difFEQRESET            equ     4EBEh
_diffeqset_trcup        equ     4EC2h
_DIFFEQSET              equ     4EC6h
_DIFFEQSET_trcp         equ     4ECAh
_set_qnews              equ     4ECEh
_DSOLVE                 equ     4ED2h
_CMPQA                  equ     4ED6h
_DRAW_SLOPE_F           equ     4EDAh
_DRAW_DIR_FIELD         equ     4EDEh
_NXTDIFVAL              equ     4EE2h
_NXT_cur_DIFVAL         equ     4EE6h
_EIGEN                  equ     4EEAh
_PUTTOAIJ               equ     4EEEh
_PUTTOAJJ               equ     4EF2h
_CPYTO2ES1              equ     4EF6h
_CPYTO1ES1              equ     4EFAh
_CPYES1                 equ     4EFEh
_CPYTO3ES2              equ     4F02h
_CPYTO2ES2              equ     4F06h
_CPYTO1ES2              equ     4F0Ah
_CPYTO2ES3              equ     4F0Eh
_CPYTO1ES3              equ     4F12h
_CPYTO3ES4              equ     4F16h
_CPYTO2ES4              equ     4F1Ah
_CPYTO1ES4              equ     4F1Eh
_CPYTO2ES5              equ     4F22h
_CPYTO1ES5              equ     4F26h
_CPYTO2EST              equ     4F2Ah
_CPYTO1EST              equ     4F2Eh
_CPYEST                 equ     4F32h
_CPYTO2ES6              equ     4F36h
_CPYTO1ES6              equ     4F3Ah
_CPYTO2ES7              equ     4F3Eh
_CPYTO1ES7              equ     4F42h
_CPYTO2ES8              equ     4F46h
_CPYTO1ES8              equ     4F4Ah
_CPYTO1ES9              equ     4F4Eh
_CPYTO1ES11             equ     4F52h
_CPYO1TOEST             equ     4F56h
_CPYO1TOES1             equ     4F5Ah
_CPYO1TOES2             equ     4F5Eh
_CPYO1TOES3             equ     4F62h
_CPYO1TOES4             equ     4F66h
_CPYO1TOES5             equ     4F6Ah
_CPYO1TOES6             equ     4F6Eh
_CPYO1TOES7             equ     4F72h
_GETVJ                  equ     4F76h
_EVALTOKEN              equ     4F7Ah
_INDEPVARNAME           equ     4F7Eh
_QUAD                   equ     4F82h
_GRAPHQUAD              equ     4F86h
_ITSOLVER               equ     4F8Ah
_gritsolver             equ     4F8Eh
_ITSOLVERB              equ     4F92h
_ITSOLVERNB             equ     4F96h
_SET_EDITCURSOR         equ     4F9Ah ;AHL -> (_P_EDITCURSOR)
_JconEdtApp             equ     4F9Eh ;CONS:EDIT
_SET10BNUM0_BDE         equ     4FA1h
_JInterEditApp          equ     4FA5h ;MATH:INTER
_setTitle               equ     4FA8h ;setup app title (HL)
_set_app_title          equ     4FA8h ;setup app title (HL)
_dispNumVars            equ     4FACh
_setEdit2nd             equ     4FB0h
_setEdit1st             equ     4FB4h
_RecalledOp1            equ     4FB8h
_RecallEd               equ     4FBCh
_PosEditCur             equ     4FC0h
_setupBuffer            equ     4FC4h
_createNumEditBuf       equ     4FC8h
_numHandleKey           equ     4FCCh
_CallCommon             equ     4FD0h
_CommonKeys             equ     4FD4h
_fDel                   equ     4FD7h
_ScrollUpifBot          equ     4FDBh
_ScrollDownIfTop        equ     4FDFh
_fClear                 equ     4FE3h
_fClearequ              equ     4FE7h
_fcondBlank             equ     4FEBh
_finsDisp               equ     4FEFh
_fcurAdjust             equ     4FF3h
_fHome                  equ     4FF7h
_setLeftEdge            equ     4FFBh
_setUpArrow             equ     4FFFh
_setDownArrow           equ     5003h
_rmvDownArrow           equ     5007h
_rmvThisArrow           equ     500Bh
_setIndicator           equ     500Fh
_fdispEOL               equ     5012h
_closeEditBufNoR        equ     5016h
_releaseBuffer          equ     501Ah
_nameToOP1              equ     501Eh ;copy symbol name to OP1 (_P_editSym->vat entry)
_nameToOP1hl            equ     5022h ;copy symbol name to OP1 (HL->vat entry)
_cursorBOL              equ     5026h
_cursorEOL              equ     5029h
_numPPutAway            equ     502Ch
_numPutAway             equ     5030h
_numSizeWind            equ     5034h
_NumRedisp              equ     5038h
_numerrorc              equ     503Ch
_JnumError              equ     5040h
_NumError               equ     5043h
_numError02             equ     5047h
_TableTbl               equ     504Ah
_POLYROOT3              equ     504Eh
_DOPEVAL                equ     5052h
_JpolyApp               equ     5056h
_notEnterpmt            equ     5059h
_outputsubnum           equ     505Ch
_JprgmEditApp           equ     5060h ;PRGM:EDIT
_ZifRclHandler          equ     5063h
_initNameBuffer         equ     5067h
_REINSTATE              equ     506Bh
_scrollPromptUp         equ     506Fh
_scrollPromptDown       equ     5073h
_rclKey                 equ     5077h
_validateName           equ     507Bh
_InitNamePrompt         equ     507Fh
_namePrompt2            equ     5083h
_promptKey              equ     5087h
_JgrRangeApp            equ     508Bh ;GRAPH:WIND
_JsimulApp              equ     508Eh
_SetupForStore          equ     5091h
_closematField          equ     5095h
_redispmatField         equ     5099h
_CommonSizeWind         equ     509Dh
_JSlistEdApp            equ     50A1h
_SpeedBump              equ     50A4h
_JstatCALCApp           equ     50A8h
_JStatCApp              equ     50ABh
_ShowStCmd              equ     50AEh
_jstatup                equ     50B2h
_jstatdown              equ     50B5h
_jstatsizelarger        equ     50B8h
_jstatsizesmaller       equ     50BBh
_JstatFCSTApp           equ     50BEh
_op1namtoedit           equ     50C1h
_dspnameOp1             equ     50C5h ;disp var name OP1, clear end of line
_DspItEol               equ     50C9h
_clrline                equ     50CDh ;clear text line
_ErrOp2Xstat            equ     50D1h
_ErrOp2Ystat            equ     50D5h
_chkop2fstat            equ     50D9h ;cp OP2,fStat
_CHKOP2YSTAT            equ     50DDh ;cp OP2,yStat
_CHKOP2XSTAT            equ     50E1h ;cp OP2,xStat
_CMP5OP2OP1             equ     50E5h
_Getname                equ     50E9h
_namError               equ     50EDh
_MAKE_FORM_NAME         equ     50F1h
_NAMEINOP1              equ     50F5h
_NAMEINOP1f             equ     50F9h
_JtolApp                equ     50FDh ;MEM:TOL
_JgrZfactApp            equ     5100h ;GRAPH:ZOOM:ZFACT
_CONVMCTOMR             equ     5103h
_REDIMMAT               equ     5107h
_DELCOLS                equ     510Bh
_INSCOLS                equ     510Fh
_INSROWS                equ     5113h
_DELROWS                equ     5117h
_CONVVRTOVC             equ     511Bh
_CONVMRTOMC             equ     511Fh
_CONVLRTOLC             equ     5123h
_INCLSTSIZE             equ     5127h
_insertlist             equ     512Bh
_dellistel              equ     512Fh
_EDITPROG               equ     5133h
_CLOSEPROG              equ     5137h
_CLRGRAPHREF            equ     513Bh
_FIXTEMPCNT             equ     513Fh
_SAVEDATA               equ     5143h
_RESTOREDATA            equ     5147h
_findAlphaUp            equ     514Bh
_findAlphaDn            equ     514Fh
_READ_SYM_NAME          equ     5153h ;copy VAT entry to op2, set CF if hidden var
_ADJTYPE                equ     5157h ;fixup var type in a
_formDisp               equ     515Bh ;display OP1 as a result
_dispOP1                equ     515Bh ;display OP1 as result
_dispDone               equ     515Fh ;print 'Done' right justified
_formMatrix             equ     5163h
_jwscrollpregcleft      equ     5167h
_jwscrollpregcright     equ     516Ah
_wscrollLeft            equ     516Dh
_JwscrollLeft           equ     5171h
_wscrollUp              equ     5174h
_JwscrollUp             equ     5178h
_wscrollDown            equ     517Bh
_JwscrollDown           equ     517Fh
_wscrollRight           equ     5182h
_JwscrollRight          equ     5186h
_FormRedispmore         equ     5189h
_formEReal              equ     518Dh ;_formReal, run indicator on
_formReal               equ     5191h ;format real OP1
_formdCplx              equ     5195h
_ClearScrollFlags       equ     5199h
_formScrollUp           equ     519Dh
_initwindow             equ     51A1h
_disarmScroll           equ     51A5h
_op1toedit              equ     51A9h
_RCLENTRYTOEDIT         equ     51ADh
_rclVarToEdit           equ     51B1h
_rclToEdit              equ     51B5h
_rclToQueue             equ     51B9h
_tokenize               equ     51BDh
_sSearch                equ     51C1h
_ASAPSEARCH             equ     51C5h
_textLen                equ     51C9h
_EqToSt                 equ     51CDh
_StToEq                 equ     51D1h
_fillSysPrompt          equ     51D5h
_detok                  equ     51D9h
_SET_EDITTAIL           equ     51DDh ;AHL -> (_P_editTail)
_setTokTemp             equ     51E1h
_COMPILE                equ     51E5h
_CLine                  equ     51E9h
_CLINES                 equ     51EDh
_YONOFF                 equ     51F1h
_ADJMEMPTR              equ     51F5h
_ADJfreemem             equ     51F9h
_ADJMATH                equ     51FDh
_ADJM7                  equ     5201h
_SET_CHKDELPTR2         equ     5205h ;AHL -> (_P_CHKDELPTR2)
_LOAD_ABS_SRC_ADDR      equ     5209h ;(_ABS_SRC_ADDR) -> AHL
_get_abs_src_addr       equ     5209h ;(_ABS_SRC_ADDR) -> AHL
_SET_INSDELPTR          equ     520Dh ;AHL -> (_P_INSDELPTR)
_LOAD_INSDELPTR         equ     5211h ;(_P_INSDELPTR) -> AHL
_LOAD_CHKDELPTR2        equ     5215h ;(_P_CHKDELPTR2) -> AHL
_LOAD_CHKDELPTR1        equ     5219h ;(_P_CHECKDELPTR1) -> AHL
_GET_DATA_SIZE_DE_INC   equ     521Dh ;(AHL)->DE  AHL=AHL+2
_get_word_ahl           equ     521Dh ;(AHL)->DE  AHL=AHL+2
_WRITE_SIZE_DE_INC      equ     5221h ;DE->(AHL)  AHL=AHL+2
_set_word_ahl           equ     5221h ;DE->(AHL)  AHL=AHL+2
_ADJSYMPTRS             equ     5225h
_ADJSYMP1               equ     5229h
_ADJPARSER              equ     522Dh
_SET_ENDPC              equ     5231h ;AHL -> (_P_ENDPC)
_ABS_MOV10TOOP1         equ     5235h
_ABS_MOV10TOOP1_noset   equ     5239h
_ABS_MOV10B_SET_D       equ     523Dh
_ABS_MOVFROP1_SET_D     equ     5241h
_ABS_MOVFROP1           equ     5245h
_ABS_MOV10B             equ     5249h
_MM_LDIR_SET_SIZE       equ     524Dh
_ABS_MOV10TOOP2_noset   equ     5251h
_load_imathptr1         equ     5255h ;(_P_IMATHPTR1) -> AHL
_LOAD_IMATHPTR2         equ     5259h ;(_P_IMATHPTR2) -> AHL
_LOAD_IMATHPTR3         equ     525Dh ;(_P_IMATHPTR3) -> AHL
_LOAD_IMATHPTR4         equ     5261h ;(_P_IMATHPTR4) -> AHL
_LOAD_IMATHPTR5         equ     5265h ;(_P_IMATHPTR5) -> AHL
_SET_IMATHPTR1          equ     5269h ;AHL -> (_P_IMATHPTR1)
_SET_IMATHPTR3          equ     526Dh ;AHL -> (_P_IMATHPTR3)
_SET_IMATHPTR4          equ     5271h ;AHL -> (_P_IMATHPTR4)
_SET_IMATHPTR5          equ     5275h ;AHL -> (_P_IMATHPTR5)
_BLOCK0                 equ     5279h
_BLOCK0LP               equ     527Dh
_SET_CHKDELPTR1         equ     5281h ;AHL -> (P_CHECKDELPTR1)
_SET_ABS_DEST_ADDR      equ     5285h ;AHL -> (_ABS_DEST_ADDR)
_NCR                    equ     5289h
_NPR                    equ     528Dh
_ZMINT                  equ     5291h
_RNDRANGE               equ     5295h
_ZMFIT                  equ     5299h
_STEPCK                 equ     529Dh
_GETRECT                equ     52A1h
_FINDT1                 equ     52A5h
_STPICTOK               equ     52A9h
_STPICGRPH              equ     52ADh
_RCPICTOK               equ     52B1h
_RCPICGRPH              equ     52B5h
_RCfieldpic             equ     52B9h
_STODBTOK               equ     52BDh
_STODBGRPH              equ     52C1h
_RCLDBTOK               equ     52C5h
_RCLDBGRPH              equ     52C9h
_VARFIND                equ     52CDh
_ALLEQ                  equ     52D1h
_FNDALLSELEQ            equ     52D5h
_FNDALLEQ               equ     52D9h
_FNDSELEQ               equ     52DDh
_initfpeq               equ     52E1h
_NEXTEQ                 equ     52E5h
_PREVEQ                 equ     52E9h
_MM_LDIR                equ     52EDh ;24 bit LDIR
_MM_LDDR                equ     52F1h ;24 bit LDDR
_ZMSTATS                equ     52F5h
_P_HISTPLOT             equ     52F9h
_P_XYLINPLOT            equ     52FDh
_P_SCATPLOT             equ     5301h
_P_BOXPLOT              equ     5305h
_P_MBOXPLOT             equ     5309h
_DRAWSPLOT              equ     530Dh
_INITNEWTRACEP          equ     5311h
_SPLOTCOORD             equ     5315h
_splotright             equ     5319h
_SPLOTLEFT              equ     531Dh
_BOX_AT_MIN             equ     5321h
_BOX_AT_MAX             equ     5325h
_CMPBOXINFO             equ     5329h
_nextplot               equ     532Dh
_NEXTPLOT1              equ     5331h
_PREVPLOT               equ     5335h
_CLRPREVPLOT            equ     5339h
_put_index_lst          equ     533Dh
_GET_INDEX_LST          equ     5341h
_HEAP_SORT              equ     5345h
_TWOVARSET              equ     5349h
_ONEVAR                 equ     534Dh
_ExCmdStat              equ     5351h
_SET_IMATHPTR2          equ     5355h
_FCSTX                  equ     5359h
_FCSTY                  equ     535Dh
_ORDSTAT                equ     5361h
_INIT_SORT_LIST_AD      equ     5365h
_rcl_statvar            equ     5369h ;load _STATVAR #A to OP1 (_STATVAR+10*A)
_get_statvar            equ     5369h ;load _STATVAR #A to OP1 (_STATVAR+10*A)
_GETSTATPTR             equ     536Dh
_GetKy                  equ     5371h ;A=key code OP2=row/col
_SEND1BERR              equ     5375h
_SENDACK                equ     5379h
_PRINTSCREEN            equ     537Dh
_KEYSCNLNK              equ     5381h
_IORESETALL             equ     5385h
_JLinkApp               equ     5389h
_GETVARCMD              equ     538Ch
_GETVARCMD1             equ     5390h
_SENDVARCMD             equ     5394h
_low_bat_disp           equ     5398h ;print low battery message
_low_battery            equ     5398h ;print low battery message
_msgcommon              equ     539Bh
_JvarsApp               equ     539Fh
_delvartok              equ     53A2h
_bufLeft                equ     53A6h
_bufRight               equ     53AAh
_bufInsert              equ     53AEh
_bufQueueChar           equ     53B2h
_bufQueueString         equ     53B6h
_bufReplace             equ     53BAh
_bufDelete              equ     53BEh
_bufPeek                equ     53C2h
_bufToBtm               equ     53C6h
_setupEditequ           equ     53CAh
_setupEditEquPtr        equ     53CEh
_bufToTop               equ     53D2h
_isEditFull             equ     53D6h
_isEditEmpty            equ     53DAh
_isAtTop                equ     53DEh
_isAtBtm                equ     53E2h
_bufClear               equ     53E6h
_JcursorFirst           equ     53EAh
_JcursorLast            equ     53EDh
_cursorLeft             equ     53F0h
_JcursorLeft            equ     53F4h
_JcursorRight           equ     53F7h
_cursorRight            equ     53FAh
_cursorUp               equ     53FEh
_JcursorUp              equ     5402h
_cursorDown             equ     5405h
_JcursorDown            equ     5409h
_cursorToOffset         equ     540Ch
_insertString           equ     5410h
_needPreBlank           equ     5414h
_condBlank              equ     5418h
_insDisp                equ     541Ch
_JinsDisp               equ     5420h
_JdispEOW               equ     5423h
_dispEOL                equ     5426h
_dispHead               equ     542Ah
_dispTail               equ     542Eh
_setupEditCmd           equ     5432h
_SET_EDITTOP            equ     5436h
_setEmptyEditequ        equ     543Ah
_setEmptyEditPtr        equ     543Eh
_SET_EDITBTM            equ     5442h
_setEditDetok           equ     5446h
_closeEditequ           equ     544Ah
_toggleIns              equ     544Eh
_JtoggleIns             equ     5452h
_JerrorApp              equ     5455h
_assertDec              equ     5458h ;number base error if any base is selected
_assertEnough           equ     545Ch ;memory error if less than hl free memory
_dispErrNum             equ     5460h
_MOV10OP2ADD            equ     5464h
_INTGR                  equ     5468h
_MOV10TO2SUB            equ     546Ch
_MINUS1                 equ     5470h
_FPSUB                  equ     5474h
_FPADD                  equ     5478h
_DTOR                   equ     547Ch
_RTOD                   equ     5480h
_TIMESPT5               equ     5484h
_FPSQUARE               equ     5488h
_FPMULT                 equ     548Ch
_INVOP1SC               equ     5490h
_invop1op2              equ     5490h
_INVOP1S                equ     5494h
_INVOP2S                equ     5498h
_FRAC                   equ     549Ch
_MLTDIV0                equ     54A0h
_FPRECIP                equ     54A4h
_FPDIV                  equ     54A8h
_SQROOT                 equ     54ACh
_SQROOTP                equ     54B0h
_CRNDGUARD              equ     54B4h
_CROUND                 equ     54B8h
_RNDGUARD               equ     54BCh
_ROUND                  equ     54C0h
_LNX                    equ     54C4h
_LNXP                   equ     54C8h
_LOGXP                  equ     54CCh
_LOGX                   equ     54D0h
_ETOX                   equ     54D4h
_TENX                   equ     54D8h
_SINCOSRAD              equ     54DCh
_SIN                    equ     54E0h
_COS                    equ     54E4h
_TAN                    equ     54E8h
_SINHCOSH               equ     54ECh
_TANH                   equ     54F0h
_COSH                   equ     54F4h
_SINH                   equ     54F8h
_ACOSRAD                equ     54FCh
_ATAN2RAD               equ     5500h
_ASINRAD                equ     5504h
_ACOS                   equ     5508h
_ACOSP                  equ     550Ch
_ATAN                   equ     5510h
_ASIN                   equ     5514h
_ATAN2                  equ     5518h
_ATANH                  equ     551Ch
_OnePlusMinusX          equ     5520h
_ASINH                  equ     5524h
_ACOSH                  equ     5528h
_PTOR                   equ     552Ch
_RTOP                   equ     5530h
_XROOTY                 equ     5534h
_YTOX                   equ     5538h
_RANDNRM                equ     553Ch
_RANDBIN                equ     5540h
_RANDINT                equ     5544h
_gammalog               equ     5548h
_gamma_fact             equ     554Ch
_cmdApp                 equ     5550h
_JcmdApp                equ     5554h
_cmdInit                equ     5557h
_cmdReset               equ     555Bh
_TOKREGEQ               equ     555Fh
_TOKX                   equ     5563h
_writeb_inc_ahl         equ     5567h ;ld (AHL),C  inc AHL
_TOKNUM                 equ     556Bh
_DREQUTOK               equ     556Fh
_CONVDIM                equ     5573h
_CONVOP1                equ     5577h ;OP1->de,a  max:9999
_EXMODE                 equ     557Bh
_exgstyletok            equ     557Fh
_DOPLOTTOK              equ     5583h
_DO_EXTEXT              equ     5587h
_graph_explr            equ     558Bh
_Axes                   equ     558Fh
_CEILING                equ     5593h
_exstregtok             equ     5597h
_STOLDIM                equ     559Bh
_STODIM                 equ     559Fh
_exasmcomp              equ     55A3h
_JRamApp                equ     55A7h ;MEM:RAM
_getkey                 equ     55AAh ;A = key value
_DECTOBASE              equ     55AEh
_BASETODEC              equ     55B2h
_EXBINMATH1             equ     55B6h
_FAND                   equ     55BAh
_FOR                    equ     55BEh
_FXOR                   equ     55C2h
_FNOT                   equ     55C6h
_ROTATER                equ     55CAh
_SHIFTR                 equ     55CEh
_ROTATEL                equ     55D2h
_SHIFTL                 equ     55D6h
_RANDOM                 equ     55DAh
_STORAND                equ     55DEh
_RANDINIT               equ     55E2h
_JstatEDITApp           equ     55E6h ;STAT:EDIT
_dispLstCol             equ     55E9h
_DELLSTNAME             equ     55EDh
_LSTNAMEVWHANDLEKEY     equ     55F1h
_hiCurLstField          equ     55F5h
_DspLstTitle            equ     55F9h
_dsplsts                equ     55FDh
_dspLstLabelM           equ     5601h
_DspModDec              equ     5605h
_clrcur                 equ     5609h
_DSP_LST_1_LINE         equ     560Ch
_SetUpDsp               equ     5610h
_closeEditBuf           equ     5614h
_DelAnyLst              equ     5618h
_MatPutAway             equ     561Ch
_StatPutAway            equ     5620h
_lstPutAway             equ     5624h
_resetstuff             equ     5628h
_resetfps               equ     562Ch
_parseEditBuf           equ     5630h
_EDITLST                equ     5634h
_JTableEditApp          equ     5638h
_DSPCURTBL              equ     563Bh
_TBDECODA               equ     563Fh
_VPUTBLANK              equ     5643h ;ld a,' ' _vputmap
_clearUserMenu          equ     5647h
_setUserMenuItem        equ     564Bh
_promptUserMenu         equ     564Fh
_resetUserMenu          equ     5653h
_StatSetApp             equ     5657h
_StatItemName           equ     565Ah
_JTableSetApp           equ     565Eh
_UpSetMenu              equ     5661h
_regraphstatifnecessary equ     5664h
_DownSPSetMenu          equ     5668h
_StatPlotInfoApp        equ     566Bh ;STAT:PLOT
_statplottypedisp       equ     566Eh
_statplotsetputaway     equ     5672h
_statplotinit           equ     5676h
_Showfield02            equ     567Ah
_MatVwHandleKey         equ     567Dh
_dspmattableall         equ     5681h
_dspmattable2           equ     5685h
_dspmattable            equ     5689h
_MatSizeWind            equ     568Dh
_setEditCurmatfield     equ     5691h
_matredisp              equ     5695h
_CONVERT                equ     5699h
_copycharmapl           equ     569Dh
_copycharmaps           equ     56A1h
_JmatEditApp            equ     56A5h ;MATRX:EDIT
_EditMVName             equ     56A8h
_matError               equ     56ACh
_rowedhandlekey         equ     56B0h
_coledhandlekey         equ     56B4h
_JsolverApp             equ     56B8h
_clearbal_asap          equ     56BBh
_deleteLftRt            equ     56BFh
_JvcEditApp             equ     56C3h ;VECTR:EDIT
_JequEdApp              equ     56C6h
_dispequ                equ     56C9h
_openEquField           equ     56CDh
_closeEquField          equ     56D1h
_AutoSelect             equ     56D5h
_EQSELUNSEL             equ     56D9h
_setStyleCode           equ     56DDh
_asap_to_ram            equ     56E1h
_load_asap_jump         equ     56E4h
_newcontext_asap        equ     56E7h
_on_script              equ     56EAh
_off_script             equ     56EDh
_asap_grphexpr          equ     56F0h
_chk_alt_parser         equ     56F3h
_chk_alt_execlop        equ     56F6h
_chk_alt_formdisp       equ     56F9h
_chk_alt_cmd_tok        equ     56FCh
_chk_alt_home           equ     56FFh
_chk_alt_getkey         equ     5702h
_chk_alt_regraph        equ     5705h
_EOSTOK_ENTRY           equ     5708h
_EXEC_ASAP_CMD          equ     570Bh
_asap_cmdtok            equ     570Eh
_ASAP_FUNC_EXEC         equ     5711h
_exec_pg3               equ     5714h ;rom page 3 asm jump table routine
_exec_pg4               equ     5718h ;rom page 4 asm jump table routine
_exec_io                equ     571Ch
_unopexec1              equ     5720h
_mode_exec              equ     5724h
_slot_number            equ     5728h
_menu_offset            equ     572Ch
_exec_assembly          equ     5730h ;asm module executor
_errsignchange          equ     5732h ;error 27 NO SIGN CHNG
_aclosefield            equ     5735h
_adispnumvars           equ     5738h
_adispvarval            equ     573Bh
_adispval               equ     573Eh
_aseteditcurrent        equ     5741h
_anumhandlekey          equ     5744h
_aredispfield           equ     5747h
_anumputaway            equ     574Ah
_anumsizewind           equ     574Dh
_ainitvars              equ     5750h
_alt_parse_end          equ     5753h
_newtoken               equ     5756h
_cmdret                 equ     5759h
_load_asap              equ     575Ch ;OP1=func name  bc=end of prog  de=sym table  hl=menu table
_amode_exec             equ     5760h

;Ram Locations
_RAMStart               equ     0C000h
_kbdScanCode            equ     0C000h ;key code
_kbdLGSC                equ     0C001h
_kbdPSC                 equ     0C002h ;previous scan code
_kbdWUR                 equ     0C003h
_kbdDebncCnt            equ     0C004h
_kbdkey                 equ     0C005h
_kbdGetKy               equ     0C006h
_keyextend              equ     0C007h
_contrast               equ     0C008h ;contrast (0-31)
_APDSubTimer            equ     0C009h ;APD sub-timer
_APDTimer               equ     0C00Ah ;APD timer
_APDWarmUp              equ     0C00Bh
_viet                   equ     0C00Ch
_curTime                equ     0C00Eh ;cursor time counter
_curRow                 equ     0C00Fh ;cursor row
_curCol                 equ     0C010h ;cursor column
_curUnder               equ     0C011h ;char under cursor
_undelBufLen            equ     0C012h ;undelete buffer length
_undelBuf               equ     0C013h ;undelete buffer
_P_tokVarPtr            equ     0C077h
_toklen                 equ     0C07Ah
_TOK_B3                 equ     0C07Ch
_DETOK_H3               equ     0C07Dh
_MEMPRE_H3              equ     0C07Eh
_indicMem               equ     0C07Fh
_indicCounter           equ     0C087h
_indicBusy              equ     0C088h ;busy indicator
_OP1                    equ     0C089h ;OP registers begin
_OP1EXPM                equ     0C08Ah ;
_OP1EXPL                equ     0C08Bh ;
_OP1M                   equ     0C08Ch ;
_OP1EXT                 equ     0C093h ; 
_LOGKP                  equ     0C094h ;
_OP2                    equ     0C094h ;op2
_OP2EXPM                equ     0C095h ;
_OP2EXPL                equ     0C096h ;
_OP2M                   equ     0C097h ;
_OP2EXT                 equ     0C09Eh ;
_OP3                    equ     0C09Fh ;op3
_LOGKM                  equ     0C09Fh ;
_OP3EXPM                equ     0C0A0h ;
_OP3EXPL                equ     0C0A1h ;
_OP3M                   equ     0C0A2h ;
_OP3EXT                 equ     0C0A9h ;
_CORDFLG1               equ     0C0AAh ;
_OP4                    equ     0C0AAh ;op4
_OP4EXPM                equ     0C0ABh ;
_OP4EXPL                equ     0C0ACh ;
_OP4M                   equ     0C0ADh ;
_OP4EXT                 equ     0C0B4h ;
_EK                     equ     0C0B5h ;
_CORDFLG                equ     0C0B5h ;
_OP5                    equ     0C0B5h ;op5
_OP5EXPM                equ     0C0B6h ;
_SF                     equ     0C0B6h ;
_EL                     equ     0C0B6h ;
_OP5EXPL                equ     0C0B7h ;
_EM                     equ     0C0B7h ;
_OP5M                   equ     0C0B8h ;
_EMM1                   equ     0C0B8h ;
_EITS                   equ     0C0B9h ;
_ENM2                   equ     0C0BAh ;
_ENA                    equ     0C0BBh ;
_EEN                    equ     0C0BCh ;
_OP5EXT                 equ     0C0BFh ;
_EN                     equ     0C0C0h ;
_OP6                    equ     0C0C0h ;op6
_EJ                     equ     0C0C1h ;
_OP6EXPM                equ     0C0C1h ;
_OP6EXPL                equ     0C0C2h ;
_EEI                    equ     0C0C2h ;
_OP6M                   equ     0C0C3h ;
_ELOW                   equ     0C0C5h ;
_EIGH                   equ     0C0C6h ;
_OP6EXT                 equ     0C0CAh ;OP registers end
_OP7                    equ     0C0CCh ;
_CPLXTRG                equ     0C0D7h
_IOFLAG                 equ     0C0D7h
_P_IMATHPTR1            equ     0C0D8h
_P_IMATHPTR2            equ     0C0DBh
_P_IMATHPTR3            equ     0C0DEh
_P_IMATHPTR4            equ     0C0E1h
_P_IMATHPTR5            equ     0C0E4h
_CHKDELPTR1             equ     0C0E7h
_P_CHKDELPTR1           equ     0C0E7h
_P_CHKDELPTR2           equ     0C0EAh
_P_INSDELPTR            equ     0C0EDh
_P_UPDOWNPTR            equ     0C0F0h
_STDRNGSGN              equ     0C0F3h
_POLRNGSGN              equ     0C0F4h
_PARRNGSGN              equ     0C0F5h
_DIFRNDSGN              equ     0C0F6h
_USRRNGSGN              equ     0C0F7h
_STATSGN                equ     0C0F8h
_textShadow             equ     0C0F9h ;text mem
_textShadCur            equ     0C1A1h ;_textShadow's cursors
_textShadTop            equ     0C1A3h ;
_textShadAlph           equ     0C1A4h ;
_textShadIns            equ     0C1A5h ;
_textAccent             equ     0C1A6h
_cxMain                 equ     0C1A7h
_cxPPutAway             equ     0C1A9h
_cxPutAway              equ     0C1ABh
_cxRedisp               equ     0C1ADh
_cxErrorEP              equ     0C1AFh
_cxSizeWind             equ     0C1B1h
_cxPage                 equ     0C1B3h
_CXCURAPP               equ     0C1B4h ;current calc state
_cxPrev                 equ     0C1B5h
_monQH                  equ     0C1C4h
_monQT                  equ     0C1C5h
_monQueue               equ     0C1C6h
_onSP                   equ     0C1D6h
_onCheckSum             equ     0C1D8h
_promptRow              equ     0C1DAh
_promptCol              equ     0C1DBh
_promptIns              equ     0C1DCh
_promptShift            equ     0C1DDh
_promptRet              equ     0C1DEh
_promptValid            equ     0C1E0h
_P_promptTop            equ     0C1E2h
_P_promptCursor         equ     0C1E5h
_P_promptTail           equ     0C1E8h
_P_promptBtm            equ     0C1EBh
_varType                equ     0C1EEh ;current variable type
_varCurrent             equ     0C1EFh ;current variable
_varFAFlags             equ     0C1F8h
_varClass               equ     0C1FAh
_catCurrent             equ     0C1FBh ;current catlg entry ptr
_menuActive             equ     0C1FDh ;number of active menus
_menu2Hilite            equ     0C1FEh ;selected entry (1-5)
_menuSingle             equ     0C1FFh
_menuAppStack           equ     0C201h ;stack of menu applications
_menuAppPtr             equ     0C20Dh ;pointer to end of stack
_menuAppDepth           equ     0C20Fh ;depth of menu app stack
_menuSysStack           equ     0C210h ;stack of menus
_menuSysPtr             equ     0C21Ch ;pointer to end of stack
_menuSysDepth           equ     0C21Eh ;depth (# of menus, limit=3)
_menuPrvStack           equ     0C21Fh
_menuPrvPtr             equ     0C22Bh
_menuPrvDepth           equ     0C22Dh
_m2i                    equ     0C22Eh ;active menu entries
_menuDyn1               equ     0C242h ;Dynamic menu entries
_menuDyn5               equ     0C26Ah ;ie, program menu and solver
_userMenu1              equ     0C274h
_userMenuTitle          equ     0C275h
_userMenu2              equ     0C27Ch
_userMenu3              equ     0C284h
_userMenu4              equ     0C28Ch
_userMenu5              equ     0C294h
_userMenuSA             equ     0C29Ch
_XSTATSAV               equ     0C31Ch
_ioPrompt               equ     0C324h
_YSTATSAV               equ     0C326h
_FSTATSAV               equ     0C330h
_IOSNDTYP               equ     0C33Ah
_SNDRECSTATE            equ     0C33Bh
_IOERRSTATE             equ     0C33Ch
_HEADER                 equ     0C33Dh
_IODATA                 equ     0C346h
_BAKHEADER              equ     0C352h
_TBLRNGSGN              equ     0C35Bh
_calc_id                equ     0C35Ch
_penCol                 equ     0C37Ch ;pen column
_penRow                 equ     0C37Dh ;pen row
_P_RCLQUEUE             equ     0C37Eh
_ERRNO                  equ     0C381h ;error number
_ERRSP                  equ     0C382h ;error stack pointer
_errOffset              equ     0C384h ;error offset
_ram_to_use             equ     0C386h
_offerr_sav_bc          equ     0C390h
_ABS_SRC_ADDR           equ     0C392h ;3 byte ABS address pointer
_ABS_DEST_ADDR          equ     0C395h ;3 byte ABS address pointer
_MM_NUM_BYTES           equ     0C398h ;3 byte block move counter
_mm_tmp1                equ     0C39Bh
_mm_tmp2                equ     0C39Dh
_mm_tmp3                equ     0C39Fh
_mm_tmp4                equ     0C3A1h
_mm_tmp5                equ     0C3A3h
_ram_cache              equ     0C3A5h
_Flags                  equ     0C3E5h ;system flags
_ram_to_use1            equ     0C40Ah ;user flags
_statReg                equ     0C414h
_STATVARS               equ     0C415h ;free if res statsvalid,(iy+statflags)
_STCounter              equ     0C555h ;self test counter
_curgstyle              equ     0C555h
_curGY                  equ     0C556h
_curGX                  equ     0C557h
_curGY2                 equ     0C558h
_curGX2                 equ     0C559h
_curgstyle_save         equ     0C55Ah
_curgstylesave          equ     0C55Bh
_plotflagsave           equ     0C55Ch
_XMINPTR                equ     0C55Dh ;pointers
_XMAXPTR                equ     0C55Fh ;
_XSCLPTR                equ     0C561h ;
_YMINPTR                equ     0C563h ;
_YMAXPTR                equ     0C565h ;
_YSCLPTR                equ     0C567h ;
_DIF1STCURINC           equ     0C569h
_TRACEPLOT              equ     0C56Bh
_BOXPLOTINFO            equ     0C56Ch
_SCURINC                equ     0C56Dh
_CURINC                 equ     0C56Fh
_YPIXEL                 equ     0C571h
_ORGXMIN                equ     0C572h
_PANSHIFT               equ     0C57Ch
_USRRNGSIZE             equ     0C586h
_UTHETMIN               equ     0C588h
_STSP                   equ     0C58Dh
_STRAMStart             equ     0C58Dh
_UTHETMAX               equ     0C592h
_UTHETSTEP              equ     0C59Ch
_UTPLOT                 equ     0C5A6h
_UTMIN                  equ     0C5B0h
_UTMAX                  equ     0C5BAh
_UTSTEP                 equ     0C5C4h
_UXMIN                  equ     0C5CEh
_UXMAX                  equ     0C5D8h
_UXSCL                  equ     0C5E2h
_UYMIN                  equ     0C5ECh
_UYMAX                  equ     0C5F6h
_UYSCL                  equ     0C600h
_UXRES                  equ     0C60Ah
_XRES_INT               equ     0C614h
_HDERIV                 equ     0C615h
_TOL                    equ     0C61Fh ;tol(tolerance)
_XFACT                  equ     0C629h
_YFACT                  equ     0C633h
_DELTAX                 equ     0C63Dh ;delta x
_DELTAY                 equ     0C647h ;delta y
_SHORTX                 equ     0C651h 
_SHORTY                 equ     0C65Bh
_FUNRNGSIZE             equ     0C665h
_FLAGSF                 equ     0C667h ;settings for Func Mode
_XMINF                  equ     0C668h ;
_XMAXF                  equ     0C672h ;
_XSCLF                  equ     0C67Ch ;
_YMINF                  equ     0C686h ;
_YMAXF                  equ     0C690h ;
_YSCLF                  equ     0C69Ah ;
_LOWER                  equ     0C6A4h ;
_UPPER                  equ     0C6AEh ;
_XRES                   equ     0C6B8h ;Func Mode settings end
_POLRNGSIZE             equ     0C6C2h
_FLAGSPOL               equ     0C6C4h ;settings for Pol Mode
_THETAMIN               equ     0C6C5h ;
_THETAMAX               equ     0C6CFh ;
_THETASTEP              equ     0C6D9h ;
_XMINPOL                equ     0C6E3h ;
_XMAXPOL                equ     0C6EDh ;
_XSCLPOL                equ     0C6F7h ;
_YMINPOL                equ     0C701h ;
_YMAXPOL                equ     0C70Bh ;
_YSCLPOL                equ     0C715h ;Pol Mode settings end
_PARRNGSIZE             equ     0C71Fh 
_FLAGSPAR               equ     0C721h ;settings for Param Mode
_TMINPAR                equ     0C722h ;
_TMAXPAR                equ     0C72Ch ;
_TSTEPPAR               equ     0C736h ;
_XMINPAR                equ     0C740h ;
_XMAXPAR                equ     0C74Ah ;
_XSCLPAR                equ     0C754h ;
_YMINPAR                equ     0C75Eh ;
_YMAXPAR                equ     0C768h ; 
_YSCLPAR                equ     0C772h ;Param mode settings end
_DIFRNGSIZE             equ     0C77Ch
_FLAGSDIF               equ     0C77Eh ;settings for DifEq Mode
_TOLERDIF               equ     0C77Fh ;
_TPLOTDIF               equ     0C789h ;
_TMINDIF                equ     0C793h ;
_TMAXDIF                equ     0C79Dh ;
_TSTEPDIF               equ     0C7A7h ;
_XMINDIF                equ     0C7B1h ;
_XMAXDIF                equ     0C7BBh ;
_XSCLDIF                equ     0C7C5h ;
_YMINDIF                equ     0C7CFh ;
_YMAXDIF                equ     0C7D9h ;
_YSCLDIF                equ     0C7E3h ;
_XAXISDIF               equ     0C7EDh ;
_YAXISDIF               equ     0C7EEh ;DifEq Settings end
_SLOPEF_equ             equ     0C7EFh
_DIRF_X                 equ     0C7F0h
_DIRF_Y                 equ     0C7F1h
_DIRF_TIME              equ     0C7F2h
_FRES                   equ     0C7FCh
_INTS                   equ     0C806h
_DNEQ                   equ     0C810h
_P_XOUTSYM              equ     0C811h
_P_XOUTDAT              equ     0C814h
_P_YOUTSYM              equ     0C817h
_P_YOUTDAT              equ     0C81Ah
_P_INPUTSYM             equ     0C81Dh
_P_INPUTDAT             equ     0C820h
_P_FOUTDAT              equ     0C823h
_PREVDATA               equ     0C826h
_PREVDATA_EXT           equ     0C862h
_P1TYPE                 equ     0C86Ch
_SavX1List              equ     0C86Dh
_SavY1List              equ     0C876h
_SavF1List              equ     0C87Fh
_P1FRQONOFF             equ     0C888h
_P2TYPE                 equ     0C889h
_SavX2List              equ     0C88Ah
_SavY2List              equ     0C893h
_SavF2List              equ     0C89Ch
_P2FRQONOFF             equ     0C8A5h
_P3TYPE                 equ     0C8A6h
_SavX3List              equ     0C8A7h
_SavY3List              equ     0C8B0h
_SavF3List              equ     0C8B9h
_P3FRQONOFF             equ     0C8C2h
_oldtype                equ     0C8C3h
_oldxlist               equ     0C8C4h
_oldylist               equ     0C8CDh
_oldflist               equ     0C8D6h
_uppery                 equ     0C8D6h
_oldonoff               equ     0C8DFh
_tblpsrow               equ     0C8E0h
_tblscroll              equ     0C8E1h
_INPUTDAT_PG0           equ     0C8E3h
_TblLine                equ     0C8EDh
_OldTblMin              equ     0C8F7h
_TBLRNGSIZE             equ     0C901h
_TblMin                 equ     0C903h
_TblStep                equ     0C90Dh
_TABLESGN               equ     0C917h
_TableYPtr              equ     0C918h
_curTblcol              equ     0C919h
_curTblrow              equ     0C91Ah
_dspTblcol              equ     0C91Bh
_dspTblrow              equ     0C91Ch
_higTblcol              equ     0C91Dh
_higTblrow              equ     0C91Eh
_TABLEXDATA             equ     0C920h
_TBLMATRIX              equ     0C920h
_TABLEYDATA             equ     0C95Ch
_TABLETEMPLATE          equ     0C9D4h
_SavedEqTok             equ     0C9D5h
_SavedEqNum1            equ     0C9D7h
_SavedEqTok1            equ     0C9D8h
_SaveAppFlags           equ     0C9DAh
_SaveCurFlags           equ     0C9DBh
_SaveCurGstyle          equ     0C9DCh
_SaveGraphFlags         equ     0C9DDh
_evalflevel             equ     0C9DEh
_TmpMatCols             equ     0C9DFh
_ES                     equ     0C9DFh
_TmpMatRows             equ     0C9E0h
_P_DERIVPTR             equ     0C9E1h
_DTMPThresh             equ     0C9E4h
_ELCPLXLCNT             equ     0C9E6h
_DERIVLEVEL             equ     0C9E8h
_P_DIFFEQPTR            equ     0C9E9h ;absolute address to diff eq
_P_DSOLVPTR             equ     0C9EBh ;absolute address to solver eq
_SOLVAR                 equ     0C9EEh ;length byte, current solver variable
_P_QUADPTR              equ     0C9F7h
_plotSScreen            equ     0C9FAh ;graph mem
_SEED1                  equ     0CDFAh
_SEED2                  equ     0CE04h
_PARSEVAR               equ     0CE0Eh ;last parsed variable
_P_BEGPC                equ     0CE18h
_P_CURPC                equ     0CE1Bh
_P_ENDPC                equ     0CE1Eh
_ELCNT                  equ     0CE21h ;end of line count
_COLCNT                 equ     0CE23h ;column count
_ROWCNT                 equ     0CE24h ;row count
_LCOUNT                 equ     0CE25h
_EOS_ASAP_2ND           equ     0CE27h
_EXEC_CONV_SAVE         equ     0CE28h
_LASTENTRYPTR           equ     0CE2Ah ;pointer to end of stack
_LASTENTRYSTK           equ     0CE2Ch ;last entry stack
_numlastentries         equ     0CEACh ;number of entries
_currlastentry          equ     0CEADh
_FREESAVEY              equ     0CEAEh
_FREESAVEX              equ     0CEAFh
_STRACESAVE_TYPE        equ     0CEB0h
_STRACESAVE             equ     0CEB1h
_TRACESAVE              equ     0CEB3h
_DIF_T_SAVE             equ     0CEB5h
_A_B_SAVE               equ     0CEBFh
_A_B_TYPE               equ     0CEC0h
_GS_DELX                equ     0CEC1h
_GS_D1_YINC             equ     0CEC2h
_GS_D2_YINC             equ     0CEC3h
_GS_DELY                equ     0CEC4h
_GS_MAX_Y_PIX           equ     0CEC5h
_CURRENT_STYLE          equ     0CEC6h ;current graph style
_CL_X1                  equ     0CEC7h
_CL_X2                  equ     0CEC8h
_CL_Y_DAT               equ     0CEC9h
_PREV_POINT             equ     0CECBh
_RESSAVE                equ     0CECDh
_DRequ_X                equ     0CECEh
_DRequ_XINIT            equ     0CECFh
_DRequ_Y                equ     0CED9h
_DRequ_YINIT            equ     0CEDAh
_DRequ_XLIST            equ     0CEE4h
_DRequ_YLIST            equ     0CEE7h
_DRequ_tLIST            equ     0CEEAh
_DRequ_COUNT            equ     0CEEDh
_GY1                    equ     0CEEFh
_GX1                    equ     0CF21h
_GR1                    equ     0CF53h
_GQ1                    equ     0CF85h
_equ_EDIT_SAVE          equ     0CF8Ah ;
_FORMULA_BITMAP         equ     0CF8Bh ;Solver buffer
_MENUCMD_M2I            equ     0CFABh ;menu shadow
_cmdShadow              equ     0CFABh ;_cmdShadow
_MENUCMD_ITEMS          equ     0CFC9h
_MENUCMD_NUMROWS        equ     0D041h
_MENUCMD_CURROW         equ     0D042h
_cmdShadCur             equ     0D053h ;shadow cursors
_cmdShadAlph            equ     0D055h ;
_cmdShadIns             equ     0D056h ;
_cmdCursor              equ     0D057h ;cursor
_P_editTop              equ     0D059h ;Program editor top
_P_EDITCURSOR           equ     0D05Ch ;program editor cursor
_P_editTail             equ     0D05Fh
_P_editBtm              equ     0D062h ;Program editor bottom
_curmatcol              equ     0D065h
_curmatrow              equ     0D066h
_curlstrow              equ     0D067h
_numedTbl               equ     0D069h
_curlistel              equ     0D069h
_curlstrowh             equ     0D06Ah
_higmatcol              equ     0D06Bh
_higmatrow              equ     0D06Ch
_higlstrow              equ     0D06Dh
_maxdsprow              equ     0D06Fh
_ForCurMat              equ     0D070h
_higlstrowh             equ     0D070h
_ForDspCol              equ     0D072h
_forerrornum            equ     0D074h
_P_editSym              equ     0D075h
_P_editDat              equ     0D078h
_DspMatCol              equ     0D07Bh
_DspMatRow              equ     0D07Ch
_TmpMatCol              equ     0D07Dh
_TmpMatRow              equ     0D07Eh
_numoflist              equ     0D07Fh ;num of lists in buffer
_num1stlist             equ     0D080h ;num of first list
_NumCurList             equ     0D081h ;num of current list
_STATED_CUT_COL         equ     0D082h
_listnamebuffer         equ     0D083h ;list buffer(numbered)
_LastName               equ     0D12Eh
_modeRoot               equ     0D137h
_modeCount              equ     0D139h
_modeItem               equ     0D13Ah
_modePtr                equ     0D13Bh
_winTop                 equ     0D13Dh ;first homescreen row
_winBtm                 equ     0D13Eh ;last homescreen row
_winLeftEdge            equ     0D13Fh 
_winLeft                equ     0D140h
_winAbove               equ     0D142h
_winRow                 equ     0D144h
_winCol                 equ     0D146h
_fmtDigits              equ     0D148h
_fmtString              equ     0D149h
_fmtConv                equ     0D18Ah
_fmtLeft                equ     0D19Eh
_fmtIndex               equ     0D1A0h
_P_fmtMatSym            equ     0D1A2h
_P_fmtMatMem            equ     0D1A5h
_EQS                    equ     0D1A8h
_LSTINDEX               equ     0D1AAh
_LSTSIZE                equ     0D1ACh
_equINDEX               equ     0D1AEh
_order                  equ     0D1B0h ;polynomial degree
_xnamesav               equ     0D1B1h
_ynamesav               equ     0D1BAh
_CustMType              equ     0D1C3h ;type=09(new) or 08(sub)
_MCustM                 equ     0D1C3h ; 
_CustMLen               equ     0D1C4h ;number of custmnames
_CustMSav               equ     0D1C5h ;pointers to names
_custmnames             equ     0D1E3h ;the menu entries
_VARSAVECNT             equ     0D279h
_DELADJAMT              equ     0D27Ah
_TEMPINPUT              equ     0D27Dh
_TSYMPTR1               equ     0D27Eh
_TSYMPTR2               equ     0D280h
_P_CHKDELPTR3           equ     0D282h
_P_CHKDELPTR4           equ     0D285h
_P_TEMPMEM              equ     0D288h
_FPBASE                 equ     0D28Bh
_FPS                    equ     0D28Dh ;fp stack pointer
_OPBASE                 equ     0D28Fh
_OPS                    equ     0D291h ;op stack pointer
_PTempCnt               equ     0D293h
_CLEANTMP               equ     0D295h
_P_PTEMP                equ     0D297h
_PTEMP_END              equ     0D29Ah
_FREE_MEM               equ     0D29Dh ;available user memory
_newdataptr             equ     0D2A0h
_SavBotRow              equ     0D2A3h
_curstatplot            equ     0D2B8h
_curstatplotprompt      equ     0D2B9h
_difeqfieldmode         equ     0D2BAh
_matedoldtype           equ     0D2BBh
_modesave1              equ     0D2BCh
_statansfirst           equ     0D2BDh
_statanslast            equ     0D2BFh
_statanscur             equ     0D2C1h
_charmap                equ     0D2C3h
_altcharmap             equ     0D2CBh
_toktmp1                equ     0D2D3h
_toktmp2                equ     0D2D4h
_IOSAVOP1               equ     0D2D5h
_DELVAR_SAV_F           equ     0D2DFh
_DEL_SAV_OP1            equ     0D2E0h
_alt_asm_exec_btm       equ     0D2EBh
_altlfontptr            equ     0D2EDh ;user font pointer
_altsfontptr            equ     0D2F0h
_altonptr               equ     0D2F3h
_altslinkptr            equ     0D2F6h
_alt_ret_status         equ     0D2F9h
_alt_ret_jmp_page       equ     0D2FAh
_alt_ret_jmp_addr       equ     0D2FBh
_alt_int_chksum         equ     0D2FDh ;checksum for interrupt
_alt_interrupt_exec     equ     0D2FEh ;interrupt
_alt_slink_chksum       equ     0D3C6h ;checksum for slink_exec
_alt_slink_exec         equ     0D3C7h ;slink
_alt_on_chksum          equ     0D48Fh ;checksum for on_exec
_alt_on_exec            equ     0D490h ;executed when turned on
_alt_off_chksum         equ     0D558h ;checksum for off_exec
_alt_off_exec           equ     0D559h ;executed when turned off
_altram_end             equ     0D621h
_asm_exec_btm           equ     0D621h
_ASAP_IND               equ     0D623h
_asm_reg_af             equ     0D624h ;saved registers start
_asm_reg_a              equ     0D625h ;
_asm_reg_l              equ     0D626h ;
_asm_reg_hl             equ     0D626h ;
_asm_reg_h              equ     0D627h ;
_asm_reg_bc             equ     0D628h ;
_asm_reg_c              equ     0D628h ;
_asm_reg_b              equ     0D629h ;
_asm_reg_de             equ     0D62Ah ;
_asm_reg_e              equ     0D62Ah ;
_asm_reg_d              equ     0D62Bh ;saved registers end
_mPrgmMATH              equ     0D62Ch
_mMath                  equ     0D64Ch ;pointers math menu entries
_mMath_asap1            equ     0D65Ah ;ptrs to math menu asaps
_mMath_asap2            equ     0D65Ch ;
_mMath_asap3            equ     0D65Eh ;
_iASAP1                 equ     0D66Ch ;space for asap entries
_iASAP2                 equ     0D678h ;
_iASAP3                 equ     0D684h ;
_iASAP4                 equ     0D690h ;
_iASAP5                 equ     0D69Ch ;
_iASAP6                 equ     0D6A8h ;
_iASAP7                 equ     0D6B4h ;
_iASAP8                 equ     0D6C0h ;
_iASAP9                 equ     0D6CCh ;end of space for asaps
_asapnames              equ     0D6D8h ;
_asap_nl1               equ     0D6D8h ;names of asap programs
_asap_nl2               equ     0D6E1h ;for tokens
_asap_nl3               equ     0D6EAh ;
_asapvar                equ     0D6FCh ;name of the current asm prog
_tokspell_asap1         equ     0D706h ;pointers to tables of
_tokspelltblptr         equ     0D706h ;token spelling
_tokspell_asap2         equ     0D708h ;
_tokspell_asap3         equ     0D70Ah ;
_numtokens              equ     0D70Eh ;number of tokens in each
_numtok_asap1           equ     0D70Eh ;asap
_numtok_asap2           equ     0D70Fh ;
_numtok_asap3           equ     0D710h ;
_eostblptr              equ     0D712h ; ponters to table of
_eostbl_asap1           equ     0D712h ; token code
_eostbl_asap2           equ     0D714h ;
_eostbl_asap3           equ     0D716h ;
_Amenu_offset           equ     0D71Ah
_reinstall_asap1        equ     0D722h
_reinstall_vec          equ     0D722h
_reinstall_asap2        equ     0D724h
_reinstall_asap3        equ     0D726h
_asap1_ram              equ     0D72Ah
_asap2_ram              equ     0D734h
_asap3_ram              equ     0D73Eh
_checkStart             equ     0D748h
_asm_exec_ram           equ     0D748h ;start address for all ASM programs


;Flags
trigflags               equ     $00
trigdeg                 equ     2

doneflags               equ     $00
donePrgm                equ     5

plotflags               equ     $02
plotloc                 equ     1
plotdisp                equ     2

grfmodeflags            equ     $02
grffuncm                equ     4
grfpolarm               equ     5
grfparamm               equ     6
grfrecurm               equ     7

graphflags              equ     $03
graphdraw               equ     0
graphcursor             equ     2

grfdbflags              equ     $04
grfdot                  equ     0
grfsimul                equ     1
grfgrid                 equ     2
grfpolar                equ     3
grfnocoord              equ     4
grfnoaxis               equ     5
grflabel                equ     6

textflags               equ     $05
textEraseBelow          equ     1
textScrolled            equ     2
textInverse             equ     3

parsflag                equ     $06

parsflag2               equ     $07
numop1                  equ     0

apdflags                equ     $08
apdable                 equ     2   ;enable APD counter
apdlock                 equ     3   ;lock APD
apdwarmstart            equ     4   ;APD has expired

onflags                 equ     $09
onRunning               equ     3
onInterrupt             equ     4

fmtflags                equ     $0A
fmtExponent             equ     0
fmtEng                  equ     1
fmtHex                  equ     2
fmtOct                  equ     3
fmtBin                  equ     4

curflags                equ     $0C
curAble                 equ     2
curOn                   equ     3
curLock                 equ     4

appflags                equ     $0D
appTextSave             equ     1
appAutoScroll           equ     2

indicflags              equ     $12
indicRun                equ     0
indicOnly               equ     2

shiftflags              equ     $12
shift2nd                equ     3
shiftAlpha              equ     4
shiftLwrAlph            equ     5
shiftALock              equ     6

asap_cmd_flag           equ     $16
ex_asap_cmd             equ     7

NewDispf                equ     $17
ProgramExecuting        equ     6

new_grf_flgs            equ     $18
textwrite               equ     6

statflags               equ     $1D
statsvalid              equ     6

anumeditflgs            equ     $1F
ex_asm_module           equ     2

exceptionflg            equ     $23
alt_font                equ     0   ;enable user font (5x7)
alt_vfont               equ     1   ;enable user font (variable width)
alt_int                 equ     2   ;enable user interrupt routine
alt_on                  equ     3   ;enable user on routine
alt_link                equ     4   ;enable user link routine
alt_sqrt                equ     5
alt_grphexpr            equ     6   ;enable (sqrt)EXPR
alt_off                 equ     7   ;enable user off routine

exceptionflg2           equ     $24
alt_parse               equ     0   ;enable (sqrt)PARSE
alt_form                equ     1   ;enable (sqrt)FORM
alt_exec                equ     2   ;enable (sqrt)EXEC
alt_home                equ     4   ;enable (sqrt)HOME
alt_cmdtok              equ     5   ;enable (sqrt)CMDTOK
alt_key                 equ     6   ;enable (sqrt)KEY
alt_grf                 equ     7   ;enable (sqrt)GRF

asm_flag1               equ     $28 ;unused flags for assembly programmers
asm_flag2               equ     $29   
asm_flag3               equ     $2A
asm_flag4               equ     $2B
asm_flag5               equ     $2C
asm_flag6               equ     $2D
asm_flag7               equ     $2E
                        
;Ports
scrnPort        equ     0   ;screen port
kbdPort         equ     1   ;keyboard port
intrptPort      equ     3   ;interrupt port
iall            equ     11  ;interrupts on, lcd on
bPort           equ     7   ;link port
D0lD1l          equ     $FC
D0lD1h          equ     $D4
D0hD1l          equ     $E8
D0hD1h          equ     $C0

;Calc States
sHome           equ     $01 ;home screen
sPoly           equ     $02 ;polynomial solver
sSimult         equ     $03 ;simultaneous equation solver
sCons           equ     $05 ;constant editor
sVectr          equ     $06 ;vector editor
sMatrx          equ     $07 ;matrix editor
sPrgm           equ     $08 ;program editor
sInter          equ     $0c ;interpolate/extrapolate editor
sList           equ     $0e ;list/stat editor
sTol            equ     $12 ;tolerance
sTable          equ     $19 ;table
sTblst          equ     $1A ;table setup
sLink           equ     $1B ;link
sMem            equ     $1D ;reset mem prompt
sDefaults       equ     $1E ;reset defaults prompt
sAll            equ     $1F ;reset all prompt
sRAM            equ     $20 ;RAM
sMode           equ     $21 ;mode/self test
sDelete         equ     $23 ;delete variables
sFunc           equ     $49 ;function editor (all modes)
sWind           equ     $4a ;window editor
sGraph          equ     $4c ;graph
sIntCond        equ     $53 ;initial conditions prompt
sForm           equ     $62 ;graph format
sCatlg          equ     $96 ;catlg-vars
sError          equ     $98 ;error message

;Miscellaneous
textend         equ     $8E
txasmcmp        equ     $28

lcdmem          equ     $fc00
lcdwide         equ     16
lcdtall         equ     64

;GetCSC Scan Codes
K_NOKEY         equ     00h
K_DOWN          equ     01h
K_LEFT          equ     02h
K_RIGHT         equ     03h
K_UP            equ     04h
K_ENTER         equ     09h
K_PLUS          equ     0Ah
K_MINUS         equ     0Bh
K_STAR          equ     0Ch
K_SLASH         equ     0Dh
K_RAISE         equ     0Eh
K_CLEAR         equ     0Fh
K_SIGN          equ     11h
K_3             equ     12h
K_6             equ     13h
K_9             equ     14h
K_RIGHTPAR      equ     15h
K_TAN           equ     16h
K_CUSTOM        equ     17h
K_DOT           equ     19h
K_2             equ     1Ah
K_5             equ     1Bh
K_8             equ     1Ch
K_LEFTPAR       equ     1Dh
K_COS           equ     1Eh
K_PRGM          equ     1Fh
K_DEL           equ     20h
K_0             equ     21h
K_1             equ     22h
K_4             equ     23h
K_7             equ     24h
K_EE            equ     25h
K_SIN           equ     26h
K_TABLE         equ     27h
K_XVAR          equ     28h
K_ON            equ     29h
K_STO           equ     2Ah
K_COMMA         equ     2Bh
K_SQUARE        equ     2Ch
K_LN            equ     2Dh
K_LOG           equ     2Eh
K_GRAPH         equ     2Fh
K_ALPHA         equ     30h
K_F5            equ     31h
K_F4            equ     32h
K_F3            equ     33h
K_F2            equ     34h
K_F1            equ     35h
K_SECOND        equ     36h
K_EXIT          equ     37h
K_MORE          equ     38h

;Getkey Codes
kRight          equ     001h
kLeft           equ     002h
kUp             equ     003h
kDown           equ     004h
kColon          equ     005h
kEnter          equ     006h
kExit           equ     007h
kClear          equ     008h
kDel            equ     009h
kIns            equ     00Ah
kNext           equ     00Bh
kAdd            equ     00Ch
kSub            equ     00Dh
kMul            equ     00Eh
kDiv            equ     00Fh
kExpon          equ     010h
kLParen         equ     011h
kRParen         equ     012h
kLBrack         equ     013h
kRBrack         equ     014h
kEqual          equ     015h
kStore          equ     016h
kRecall         equ     017h
kComma          equ     018h
kAng            equ     019h
kChs            equ     01Ah
kDecPnt         equ     01Bh
k0              equ     01Ch
k1              equ     01Dh
k2              equ     01Eh
k3              equ     01Fh
k4              equ     020h
k5              equ     021h
k6              equ     022h
k7              equ     023h
k8              equ     024h
k9              equ     025h
kEE             equ     026h
kSpace          equ     027h
kCapA           equ     028h
kCapB           equ     029h
kCapC           equ     02Ah
kCapD           equ     02Bh
kCapE           equ     02Ch
kCapF           equ     02Dh
kCapG           equ     02Eh
kCapH           equ     02Fh
kCapI           equ     030h
kCapJ           equ     031h
kCapK           equ     032h
kCapL           equ     033h
kCapM           equ     034h
kCapN           equ     035h
kCapO           equ     036h
kCapP           equ     037h
kCapQ           equ     038h
kCapR           equ     039h
kCapS           equ     03Ah
kCapT           equ     03Bh
kCapU           equ     03Ch
kCapV           equ     03Dh
kCapW           equ     03Eh
kCapX           equ     03Fh
kCapY           equ     040h
kCapZ           equ     041h
ka              equ     042h
kb              equ     043h
kc              equ     044h
kd              equ     045h
ke              equ     046h
kf              equ     047h
kg              equ     048h
kh              equ     049h
ki              equ     04Ah
kj              equ     04Bh
kk              equ     04Ch
kl              equ     04Dh
km              equ     04Eh
kn              equ     04Fh
ko              equ     050h
kp              equ     051h
kq              equ     052h
kr              equ     053h
ks              equ     054h
kt              equ     055h
ku              equ     056h
kv              equ     057h
kw              equ     058h
kx              equ     059h
ky              equ     05Ah
kz              equ     05Bh
kVarx           equ     05Ch
kAns            equ     05Dh
kPi             equ     05Eh
kInv            equ     05Fh
kSin            equ     060h
kASin           equ     061h
kCos            equ     062h
kACos           equ     063h
kTan            equ     064h
kATan           equ     065h
kSquare         equ     066h
kSqrt           equ     067h
kLn             equ     068h
kExp            equ     069h
kLog            equ     06Ah
kALog           equ     06Bh
kMath           equ     06Ch
kCplx           equ     06Dh
kString         equ     06Eh
kTest           equ     06Fh
kConv           equ     070h
kChar           equ     071h
kBase           equ     072h
kCustom         equ     073h
kVars           equ     074h
kCatalog        equ     075h
kQuit           equ     076h
kLastEnt        equ     077h
kLinkIO         equ     078h
kMem            equ     079h
kList           equ     07Ah
kVector         equ     07Bh
kConst          equ     07Ch
kMatrix         equ     07Dh
kPoly           equ     07Eh
kSimult         equ     07Fh
kStat           equ     080h
kGrMenu         equ     081h
kMode           equ     082h
kPrgm           equ     083h
kCalcu          equ     084h
kSolver         equ     085h
kTable          equ     086h
kBOL            equ     087h
kEOL            equ     088h
kCUSTM          equ     093h
kF1             equ     0C2h
kF2             equ     0C3h
kF3             equ     0C4h
kF4             equ     0C5h
kF5             equ     0C6h
kF6             equ     0C7h
kF7             equ     0C8h
kF8             equ     0C9h
kF9             equ     0CAh
kF10            equ     0CBh
kGRAPH          equ     0FFh

;Characters
Lbinary         equ     001h
Loctal          equ     002h
Ldecimal        equ     003h
Lhexadecimal    equ     004h
Lconvert        equ     005h
LsqUp           equ     006h
LsqDown         equ     007h
Lintegral       equ     008h
Lcross          equ     009h
LhexA           equ     00Ah
LhexB           equ     00Bh
LhexC           equ     00Ch
LhexD           equ     00Dh
LhexE           equ     00Eh
LhexF           equ     00Fh
Lroot           equ     010h
Linverse        equ     011h
Lsquare         equ     012h
Langle          equ     013h
Ldegree         equ     014h
Lradian         equ     015h
Ltranspose      equ     016h
LLE             equ     017h
LNE             equ     018h
LGE             equ     019h
Lneg            equ     01Ah
Lexponent       equ     01Bh
Lstore          equ     01Ch
Lten            equ     01Dh
LupArrow        equ     01Eh
LdownArrow      equ     01Fh
Lspace          equ     020h
Lexclam         equ     021h
Lquote          equ     022h
Lpound          equ     023h
Ldollar         equ     024h
Lpercent        equ     025h
Lampersand      equ     026h
Lapostrophe     equ     027h
LlParen         equ     028h
LrParen         equ     029h
Lasterisk       equ     02Ah
LplusSign       equ     02Bh
Lcomma          equ     02Ch
Ldash           equ     02Dh
Lperiod         equ     02Eh
Lslash          equ     02Fh
L0              equ     030h
L1              equ     031h
L2              equ     032h
L3              equ     033h
L4              equ     034h
L5              equ     035h
L6              equ     036h
L7              equ     037h
L8              equ     038h
L9              equ     039h
Lcolon          equ     03Ah
Lsemicolon      equ     03Bh
LLT             equ     03Ch
LEQ             equ     03Dh
LGT             equ     03Eh
Lquestion       equ     03Fh
LatSign         equ     040h
LcapA           equ     041h
LcapB           equ     042h
LcapC           equ     043h
LcapD           equ     044h
LcapE           equ     045h
LcapF           equ     046h
LcapG           equ     047h
LcapH           equ     048h
LcapI           equ     049h
LcapJ           equ     04Ah
LcapK           equ     04Bh
LcapL           equ     04Ch
LcapM           equ     04Dh
LcapN           equ     04Eh
LcapO           equ     04Fh
LcapP           equ     050h
LcapQ           equ     051h
LcapR           equ     052h
LcapS           equ     053h
LcapT           equ     054h
LcapU           equ     055h
LcapV           equ     056h
LcapW           equ     057h
LcapX           equ     058h
LcapY           equ     059h
LcapZ           equ     05Ah
LlBrack         equ     05Bh
Lbackslash      equ     05Ch
LrBrack         equ     05Dh
Lcaret          equ     05Eh
Lunderscore     equ     05Fh
Lbackquote      equ     060h
La              equ     061h
Lb              equ     062h
Lc              equ     063h
Ld              equ     064h
Le              equ     065h
Lf              equ     066h
Lg              equ     067h
Lh              equ     068h
Li              equ     069h
Lj              equ     06Ah
Lk              equ     06Bh
Ll              equ     06Ch
Lm              equ     06Dh
Ln              equ     06Eh
Lo              equ     06Fh
Lp              equ     070h
Lq              equ     071h
Lr              equ     072h
Ls              equ     073h
Lt              equ     074h
Lu              equ     075h
Lv              equ     076h
Lw              equ     077h
Lx              equ     078h
Ly              equ     079h
Lz              equ     07Ah
LlBrace         equ     07Bh
Lbar            equ     07Ch
LrBrace         equ     07Dh
Ltilde          equ     07Eh
LinvEQ          equ     07Fh
Lsub0           equ     080h
Lsub1           equ     081h
Lsub2           equ     082h
Lsub3           equ     083h
Lsub4           equ     084h
Lsub5           equ     085h
Lsub6           equ     086h
Lsub7           equ     087h
Lsub8           equ     088h
Lsub9           equ     089h
LcapAAcute      equ     08Ah
LcapAGrave      equ     08Bh
LcapACaret      equ     08Ch
LcapADier       equ     08Dh
LaAcute         equ     08Eh
LaGrave         equ     08Fh
LaCaret         equ     090h
LaDier          equ     091h
LcapEAcute      equ     092h
LcapEGrave      equ     093h
LcapECaret      equ     094h
LcapEDier       equ     095h
LeAcute         equ     096h
LeGrave         equ     097h
LeCaret         equ     098h
LeDier          equ     099h
LcapIAcute      equ     09Ah
LcapIGrave      equ     09Bh
LcapICaret      equ     09Ch
LcapIDier       equ     09Dh
LiAcute         equ     09Eh
LiGrave         equ     09Fh
LiCaret         equ     0A0h
LiDier          equ     0A1h
LcapOAcute      equ     0A2h
LcapOGrave      equ     0A3h
LcapOCaret      equ     0A4h
LcapODier       equ     0A5h
LoAcute         equ     0A6h
LoGrave         equ     0A7h
LoCaret         equ     0A8h
LoDier          equ     0A9h
LcapUAcute      equ     0AAh
LcapUGrave      equ     0ABh
LcapUCaret      equ     0ACh
LcapUDier       equ     0ADh
LuAcute         equ     0AEh
LuGrave         equ     0AFh
LuCaret         equ     0B0h
LuDier          equ     0B1h
LcapCCed        equ     0B2h
LcCed           equ     0B3h
LcapNTilde      equ     0B4h
LnTilde         equ     0B5h
Laccent         equ     0B6h
Lgrave          equ     0B7h
Ldieresis       equ     0B8h
LquesDown       equ     0B9h
LexclamDown     equ     0BAh
Lalpha          equ     0BBh
Lbeta           equ     0BCh
Lgamma          equ     0BDh
LcapDelta       equ     0BEh
Ldelta          equ     0BFh
Lepsilon        equ     0C0h
Ltheta          equ     0C1h
Llambda         equ     0C2h
Lmu             equ     0C3h
Lpi             equ     0C4h
Lrho            equ     0C5h
LcapSigma       equ     0C6h
Lsigma          equ     0C7h
Ltau            equ     0C8h
Lphi            equ     0C9h
LcapOmega       equ     0CAh
LxMean          equ     0CBh
LyMean          equ     0CCh
LsupX           equ     0CDh
Lellipsis       equ     0CEh
Lleft           equ     0CFh
Lblock          equ     0D0h
Lper            equ     0D1h
Lhyphen         equ     0D2h
Larea           equ     0D3h
Ltemp           equ     0D4h
Lcube           equ     0D5h
Lenter          equ     0D6h
LblockArrow     equ     0D7h
