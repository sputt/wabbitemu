


typedef struct {
	char name[32];
	int address;
} bcall_t;


bcall_t bcalls[] = {
 { "_JErrorNo" , 0x4000 }, 
 { "_FontHook" , 0x4003 }, 
 { "_LocalizeHook" , 0x4006 }, 
 { "_LdHLInd" , 0x4009 }, 
 { "_LdHLInd" , 0x4009 }, 
 { "_CpHLDE" , 0x400C }, 
 { "_DivHLBy10" , 0x400F }, 
 { "_DivHLByA" , 0x4012 }, 
 { "_KbdScan" , 0x4015 }, 
 { "_GetCSC" , 0x4018 }, 
 { "_coorMon" , 0x401B }, 
 { "_Mon" , 0x401E }, 
 { "_MonForceKey" , 0x4021 }, 
 { "_JForceCmdNoChar" , 0x4027 }, 
 { "_JForceCmd" , 0x402A },
 { "_NewContext0" , 0x4033 }, 
 { "_PutAway" , 0x4039 }, 
 { "_callMain" , 0x4045 }, 
 { "_AppInit" , 0x404B }, 
 { "_LCD_BUSY" , 0x4051 }, 
 { "_Min" , 0x4054 }, 
 { "_Max" , 0x4057 }, 
 { "_AbsO1PAbsO2" , 0x405A }, 
 { "_Intgr" , 0x405D }, 
 { "_Trunc" , 0x4060 }, 
 { "_InvSub" , 0x4063 }, 
 { "_Times2" , 0x4066 }, 
 { "_Plus1" , 0x4069 }, 
 { "_Minus1" , 0x406C }, 
 { "_FPSub" , 0x406F }, 
 { "_FPAdd" , 0x4072 }, 
 { "_DToR" , 0x4075 }, 
 { "_RToD" , 0x4078 }, 
 { "_Cube" , 0x407B }, 
 { "_TimesPt5" , 0x407E }, 
 { "_FPSquare" , 0x4081 }, 
 { "_FPMult" , 0x4084 }, 
 { "_LJRND" , 0x4087 }, 
 { "_InvOP1SC" , 0x408A }, 
 { "_InvOP1S" , 0x408D }, 
 { "_InvOP2S" , 0x4090 }, 
 { "_Frac" , 0x4093 }, 
 { "_FPRecip" , 0x4096 }, 
 { "_FPDiv" , 0x4099 }, 
 { "_SqRoot" , 0x409C }, 
 { "_RndGuard" , 0x409F }, 
 { "_RnFx" , 0x40A2 }, 
 { "_Int" , 0x40A5 }, 
 { "_Round" , 0x40A8 }, 
 { "_LnX" , 0x40AB }, 
 { "_LogX" , 0x40AE }, 
 { "_LJNORND" , 0x40B1 }, 
 { "_EToX" , 0x40B4 }, 
 { "_TenX" , 0x40B7 }, 
 { "_SinCosRad" , 0x40BA }, 
 { "_Sin" , 0x40BD }, 
 { "_Cos" , 0x40C0 }, 
 { "_Tan" , 0x40C3 }, 
 { "_SinHCosH" , 0x40C6 }, 
 { "_TanH" , 0x40C9 }, 
 { "_CosH" , 0x40CC }, 
 { "_SinH" , 0x40CF }, 
 { "_ACosRad" , 0x40D2 }, 
 { "_ATanRad" , 0x40D5 }, 
 { "_ATan2Rad" , 0x40D8 }, 
 { "_ASinRad" , 0x40DB }, 
 { "_ACos" , 0x40DE }, 
 { "_ATan" , 0x40E1 }, 
 { "_ASin" , 0x40E4 }, 
 { "_ATan2" , 0x40E7 }, 
 { "_ATanH" , 0x40EA }, 
 { "_ASinH" , 0x40ED }, 
 { "_ACosH" , 0x40F0 }, 
 { "_PToR" , 0x40F3 }, 
 { "_RToP" , 0x40F6 }, 
 { "_HLTimes9" , 0x40F9 }, 
 { "_CkOP1Cplx" , 0x40FC }, 
 { "_CkOP1Real" , 0x40FF }, 
 { "_Angle" , 0x4102 }, 
 { "_COP1Set0" , 0x4105 }, 
 { "_CpOP4OP3" , 0x4108 }, 
 { "_Mov9OP2Cp" , 0x410B }, 
 { "_AbsO1O2Cp" , 0x410E }, 
 { "_CpOP1OP2" , 0x4111 }, 
 { "_OP3ToOP4" , 0x4114 }, 
 { "_OP1ToOP4" , 0x4117 }, 
 { "_OP2ToOP4" , 0x411A }, 
 { "_OP4ToOP2" , 0x411D }, 
 { "_OP3ToOP2" , 0x4120 }, 
 { "_OP1ToOP3" , 0x4123 }, 
 { "_OP5ToOP2" , 0x4126 }, 
 { "_OP5ToOP6" , 0x4129 }, 
 { "_OP5ToOP4" , 0x412C }, 
 { "_OP1ToOP2" , 0x412F }, 
 { "_OP6ToOP2" , 0x4132 }, 
 { "_OP6ToOP1" , 0x4135 }, 
 { "_OP4ToOP1" , 0x4138 }, 
 { "_OP5ToOP1" , 0x413B }, 
 { "_OP3ToOP1" , 0x413E }, 
 { "_OP6ToOP5" , 0x4141 }, 
 { "_OP4ToOP5" , 0x4144 }, 
 { "_OP3ToOP5" , 0x4147 }, 
 { "_OP2ToOP5" , 0x414A }, 
 { "_OP2ToOP6" , 0x414D }, 
 { "_OP1ToOP6" , 0x4150 }, 
 { "_OP1ToOP5" , 0x4153 }, 
 { "_OP2ToOP1" , 0x4156 }, 
 { "_Mov11B" , 0x4159 }, 
 { "_Mov10B" , 0x415C }, 
 { "_Mov9B" , 0x415F }, 
 { "_Mov8B" , 0x4165 }, 
 { "_Mov7B" , 0x4168 }, 
 { "_OP2ToOP3" , 0x416E }, 
 { "_OP4ToOP3" , 0x4171 }, 
 { "_OP5ToOP3" , 0x4174 }, 
 { "_OP4ToOP6" , 0x4177 }, 
 { "_Mov9ToOP1" , 0x417A }, 
 { "_Mov9OP1OP2" , 0x417D }, 
 { "_Mov9ToOP2" , 0x4180 }, 
 { "_MovFrOP1" , 0x4183 }, 
 { "_OP4Set1" , 0x4186 }, 
 { "_OP3Set1" , 0x4189 }, 
 { "_OP2Set8" , 0x418C }, 
 { "_OP1toOP2" , 0x418E }, 
 { "_OP2Set5" , 0x418F }, 
 { "_OP2SetA" , 0x4192 }, 
 { "_OP2Set4" , 0x4195 }, 
 { "_OP2Set3" , 0x4198 }, 
 { "_OP1Set1" , 0x419B }, 
 { "_OP1Set4" , 0x419E }, 
 { "_OP1Set3" , 0x41A1 }, 
 { "_OP3Set2" , 0x41A4 }, 
 { "_OP1Set2" , 0x41A7 }, 
 { "_OP2Set2" , 0x41AA }, 
 { "_OP2Set1" , 0x41AD }, 
 { "_Zero16D" , 0x41B0 }, 
 { "_OP5Set0" , 0x41B3 }, 
 { "_OP4Set0" , 0x41B6 }, 
 { "_OP3Set0" , 0x41B9 }, 
 { "_OP2Set0" , 0x41BC }, 
 { "_OP1Set0" , 0x41BF }, 
 { "_SetNum0" , 0x41C2 }, 
 { "_ZeroOP1" , 0x41C5 }, 
 { "_ZeroOP2" , 0x41C8 }, 
 { "_ZeroOP3" , 0x41CB }, 
 { "_ZeroOP" , 0x41CE }, 
 { "_ClrLp" , 0x41D1 }, 
 { "_ShRAcc" , 0x41D4 }, 
 { "_ShLAcc" , 0x41D7 }, 
 { "_OP2ExOP6" , 0x4207 }, 
 { "_OP5ExOP6" , 0x420A }, 
 { "_OP1ExOP5" , 0x420D }, 
 { "_OP1ExOP6" , 0x4210 }, 
 { "_OP2ExOP4" , 0x4213 }, 
 { "_OP2ExOP5" , 0x4216 }, 
 { "_OP1ExOP3" , 0x4219 }, 
 { "_OP1ExOP4" , 0x421C }, 
 { "_OP1ExOP2" , 0x421F }, 
 { "_ExLp" , 0x4222 }, 
 { "_CkOP1C0" , 0x4225 }, 
 { "_CkOP1FP0" , 0x4228 }, 
 { "_CkOP2FP0" , 0x422B }, 
 { "_PosNo0Int" , 0x422E }, 
 { "_CkPosInt" , 0x4231 }, 
 { "_CkInt" , 0x4234 }, 
 { "_CkOdd" , 0x4237 }, 
 { "_GETCONOP1" , 0x423D }, 
 { "_GETCONOP2" , 0x4240 }, 
 { "_ExpToHex" , 0x424F }, 
 { "_OP1ExpToDec" , 0x4252 }, 
 { "_CkOP2Pos" , 0x4255 }, 
 { "_CkOP1Pos" , 0x4258 }, 
 { "_ClrOP2S" , 0x425B }, 
 { "_ClrOP1S" , 0x425E }, 
 { "_FDiv100" , 0x4261 }, 
 { "_FDiv10" , 0x4264 }, 
 { "_DecO1Exp" , 0x4267 }, 
 { "_IncO1Exp" , 0x426A }, 
 { "_CkValidNum" , 0x4270 }, 
 { "_HTimesL" , 0x4276 }, 
 { "_EOP1NotReal" , 0x4279 }, 
 { "_ThetaName" , 0x427C }, 
 { "_RName" , 0x427F }, 
 { "_RegEqName" , 0x4282 }, 
 { "_nName" , 0x4285 }, 
 { "_XName" , 0x4288 }, 
 { "_YName" , 0x428B }, 
 { "_TName" , 0x428E }, 
 { "_RealName" , 0x4291 }, 
 { "_MarkTempDirty" , 0x4297 }, 
 { "_IsA2ByteTok" , 0x42A3 }, 
 { "_Equ_or_NewEqu" , 0x42C4 }, 
 { "_ErrD_OP1NotPos" , 0x42C7 }, 
 { "_ErrD_OP1Not_R" , 0x42CA }, 
 { "_ErrD_OP1NotPosInt" , 0x42CD }, 
 { "_ErrD_OP1_LE_0" , 0x42D0 }, 
 { "_ErrD_OP1_0" , 0x42D3 }, 
 { "_Rcl_StatVar" , 0x42DC }, 
 { "_CkOP2Real" , 0x42DF }, 
 { "_MemChk" , 0x42E5 }, 
 { "_ChkFindSym" , 0x42F1 }, 
 { "_FindSym" , 0x42F4 }, 
 { "_InsertMem" , 0x42F7 }, 
 { "_InsertMemNoUpdateVAT" , 0x42FA }, 
 { "_EnoughMem" , 0x42FD }, 
 { "_CreateCplx" , 0x430C }, 
 { "_CreateReal" , 0x430F }, 
 { "_CreateTempRList" , 0x4312 }, 
 { "_CreateRList" , 0x4315 }, 
 { "_CreateTempCList" , 0x4318 }, 
 { "_CreateCList" , 0x431B }, 
 { "_CreateTempRMat" , 0x431E }, 
 { "_CreateRMat" , 0x4321 }, 
 { "_CreateTempString" , 0x4324 }, 
 { "_CreateStrng" , 0x4327 }, 
 { "_Create0Equ" , 0x432A }, 
 { "_CreateTempEqu" , 0x432D }, 
 { "_CreateEqu" , 0x4330 }, 
 { "_CreatePict" , 0x4333 }, 
 { "_CreateGDB" , 0x4336 }, 
 { "_CreateProg" , 0x4339 }, 
 { "_AdjMath" , 0x4345 }, 
 { "_DelVar" , 0x4351 }, 
 { "_DelMem" , 0x4357 }, 
 { "_DelVar3DC" , 0x4360 }, 
 { "_IsFixedName" , 0x4363 }, 
 { "_DelVarEntry" , 0x4366 }, 
 { "_DataSizeA" , 0x4369 }, 
 { "_DataSize" , 0x436C }, 
 { "_PopMCplxO1" , 0x436F }, 
 { "_PopOP5" , 0x4378 }, 
 { "_PopOP3" , 0x437B }, 
 { "_PopOP1" , 0x437E }, 
 { "_PopRealO6" , 0x4381 }, 
 { "_PopRealO5" , 0x4384 }, 
 { "_PopRealO4" , 0x4387 }, 
 { "_PopRealO3" , 0x438A }, 
 { "_PopRealO2" , 0x438D }, 
 { "_PopRealO1" , 0x4390 }, 
 { "_PopReal" , 0x4393 }, 
 { "_DeallocFPS" , 0x439F }, 
 { "_DeallocFPS1" , 0x43A2 }, 
 { "_AllocFPS" , 0x43A5 }, 
 { "_AllocFPS1" , 0x43A8 }, 
 { "_PushRealO6" , 0x43AB }, 
 { "_PushRealO5" , 0x43AE }, 
 { "_PushRealO4" , 0x43B1 }, 
 { "_PushRealO3" , 0x43B4 }, 
 { "_PushRealO2" , 0x43B7 }, 
 { "_PushRealO1" , 0x43BA }, 
 { "_PushReal" , 0x43BD }, 
 { "_PushOP5" , 0x43C0 }, 
 { "_PushOP3" , 0x43C3 }, 
 { "_PushMCplxO3" , 0x43C6 }, 
 { "_PushOP1" , 0x43C9 }, 
 { "_PushMCplxO1" , 0x43CF }, 
 { "_Exch9" , 0x43D5 }, 
 { "_CpyTo1FPS11" , 0x43D8 }, 
 { "_CpyTo2FPS5" , 0x43DB }, 
 { "_CpyTo1FPS5" , 0x43DE }, 
 { "_CpyTo2FPS6" , 0x43E1 }, 
 { "_CpyTo1FPS6" , 0x43E4 }, 
 { "_CpyTo2FPS7" , 0x43E7 }, 
 { "_CpyTo1FPS7" , 0x43EA }, 
 { "_CpyTo1FPS8" , 0x43ED }, 
 { "_CpyTo2FPS8" , 0x43F0 }, 
 { "_CpyTo1FPS10" , 0x43F3 }, 
 { "_CpyTo1FPS9" , 0x43F6 }, 
 { "_CpyTo2FPS4" , 0x43F9 }, 
 { "_CpyTo6FPS3" , 0x43FC }, 
 { "_CpyTo6FPS2" , 0x43FF }, 
 { "_CpyTo2FPS3" , 0x4402 }, 
 { "_CpyTo1FPS3" , 0x4408 }, 
 { "_CpyTo1FPS4" , 0x440E }, 
 { "_CpyTo3FPS2" , 0x4411 }, 
 { "_CpyTo5FPST" , 0x4414 }, 
 { "_CpyTo6FPST" , 0x4417 }, 
 { "_CpyTo4FPST" , 0x441A }, 
 { "_CpyTo3FPST" , 0x441D }, 
 { "_CpyTo2FPST" , 0x4420 }, 
 { "_CpyTo1FPST" , 0x4423 }, 
 { "_CpyStack" , 0x4429 }, 
 { "_CpyTo3FPS1" , 0x442C }, 
 { "_CpyTo2FPS1" , 0x442F }, 
 { "_CpyTo1FPS1" , 0x4432 }, 
 { "_CpyTo2FPS2" , 0x4438 }, 
 { "_CpyTo1FPS2" , 0x443B }, 
 { "_CpyO3ToFPST" , 0x4441 }, 
 { "_CpyO2ToFPST" , 0x4444 }, 
 { "_CpyO6ToFPST" , 0x4447 }, 
 { "_CpyO1ToFPST" , 0x444A }, 
 { "_CpyToFPST" , 0x444D }, 
 { "_CpyToStack" , 0x4450 }, 
 { "_CpyO3ToFPS1" , 0x4453 }, 
 { "_CpyO5ToFPS1" , 0x4456 }, 
 { "_CpyO2ToFPS1" , 0x4459 }, 
 { "_CpyO1ToFPS1" , 0x445C }, 
 { "_CpyToFPS1" , 0x445F }, 
 { "_CpyO2ToFPS2" , 0x4462 }, 
 { "_CpyO3ToFPS2" , 0x4465 }, 
 { "_CpyO6ToFPS2" , 0x4468 }, 
 { "_CpyO1ToFPS2" , 0x446B }, 
 { "_CpyToFPS2" , 0x446E }, 
 { "_CpyO5ToFPS3" , 0x4471 }, 
 { "_CpyO2ToFPS3" , 0x4474 }, 
 { "_CpyO1ToFPS3" , 0x4477 }, 
 { "_CpyToFPS3" , 0x447A }, 
 { "_CpyO1ToFPS6" , 0x447D }, 
 { "_CpyO1ToFPS7" , 0x4480 }, 
 { "_CpyO1ToFPS5" , 0x4483 }, 
 { "_CpyO2ToFPS4" , 0x4486 }, 
 { "_CpyO1ToFPS4" , 0x4489 }, 
 { "_ErrNotEnoughMem" , 0x448C }, 
 { "_HLMinus9" , 0x4492 }, 
 { "_ErrOverflow" , 0x4495 }, 
 { "_ErrDivBy0" , 0x4498 }, 
 { "_ErrSingularMat" , 0x449B }, 
 { "_ErrDomain" , 0x449E }, 
 { "_ErrIncrement" , 0x44A1 }, 
 { "_ErrNon_Real" , 0x44A4 }, 
 { "_ErrSyntax" , 0x44A7 }, 
 { "_ErrDataType" , 0x44AA }, 
 { "_ErrArgument" , 0x44AD }, 
 { "_ErrDimMismatch" , 0x44B0 }, 
 { "_ErrDimension" , 0x44B3 }, 
 { "_ErrUndefined" , 0x44B6 }, 
 { "_ErrMemory" , 0x44B9 }, 
 { "_ErrInvalid" , 0x44BC }, 
 { "_ErrBreak" , 0x44BF }, 
 { "_ErrStat" , 0x44C2 }, 
 { "_ErrSignChange" , 0x44C5 }, 
 { "_ErrIterations" , 0x44C8 }, 
 { "_ErrBadGuess" , 0x44CB }, 
 { "_ErrTolTooSmall" , 0x44CE }, 
 { "_ErrStatPlot" , 0x44D1 }, 
 { "_ErrLinkXmit" , 0x44D4 }, 
 { "_JError" , 0x44D7 }, 
 { "_StrCopy" , 0x44E3 }, 
 { "_StrCat" , 0x44E6 }, 
 { "_OffPageJump" , 0x44F2 }, 
 { "_PutMap" , 0x4501 }, 
 { "_PutC" , 0x4504 }, 
 { "_DispHL" , 0x4507 }, 
 { "_PutS" , 0x450A }, 
 { "_PutPSB" , 0x450D }, 
 { "_PutPS" , 0x4510 }, 
 { "_WPutSEOL" , 0x4522 }, 
 { "_NewLine" , 0x452E }, 
 { "_ClrLCDFull" , 0x4540 }, 
 { "_ClrLCD" , 0x4543 }, 
 { "_ClrScrnFull" , 0x4546 }, 
 { "_ClrScrn" , 0x4549 }, 
 { "_ClrTxtShd" , 0x454C }, 
 { "_ClrWindow" , 0x454F }, 
 { "_EraseEOL" , 0x4552 }, 
 { "_EraseEOW" , 0x4555 }, 
 { "_HomeUp" , 0x4558 }, 
 { "_VPutMap" , 0x455E }, 
 { "_VPutS" , 0x4561 }, 
 { "_VPutSN" , 0x4564 }, 
 { "_RunIndicOn" , 0x456D }, 
 { "_RunIndicOff" , 0x4570 }, 
 { "_GetTokLen" , 0x4591 }, 
 { "_Get_Tok_Strng" , 0x4594 }, 
 { "_DispEOL" , 0x45A6 }, 
 { "_DispDone" , 0x45B5 }, 
 { "_CursorOff" , 0x45BE }, 
 { "_CursorOn" , 0x45C4 }, 
 { "_KeyToString" , 0x45CA }, 
 { "_PullDownChk" , 0x45CD }, 
 { "_PrevContext" , 0x4600 }, 
 { "_CompareContext" , 0x4603 }, 
 { "_AdrMRow" , 0x4606 }, 
 { "_AdrMEle" , 0x4609 }, 
 { "_GetMToOP1" , 0x4615 }, 
 { "_PutToMat" , 0x461E }, 
 { "_AdrLEle" , 0x462D }, 
 { "_GetLToOP1" , 0x4636 }, 
 { "_PutToL" , 0x4645 }, 
 { "_ToFrac" , 0x4657 }, 
 { "_BinOPExec" , 0x4663 }, 
 { "_UnOPExec" , 0x4672 }, 
 { "_ThreeExec" , 0x4675 }, 
 { "_FourExec" , 0x467B }, 
 { "_FiveExec" , 0x467E }, 
 { "_CloseEditBufNoR" , 0x476E }, 
 { "_ReleaseBuffer" , 0x4771 }, 
 { "_Load_SFont" , 0x4783 }, 
 { "_SFont_Len" , 0x4786 }, 
 { "_SetXXOP1" , 0x478C }, 
 { "_SetXXOP2" , 0x478F }, 
 { "_SetXXXXOP2" , 0x4792 }, 
 { "_UCLineS" , 0x4795 }, 
 { "_CLine" , 0x4798 }, 
 { "_CLineS" , 0x479B }, 
 { "_XRootY" , 0x479E }, 
 { "_YToX" , 0x47A1 }, 
 { "_ZmStats" , 0x47A4 }, 
 { "_StoGDB2" , 0x47CE }, 
 { "_RclGDB2" , 0x47D1 }, 
 { "_CircCmd" , 0x47D4 }, 
 { "_GrphCirc" , 0x47D7 }, 
 { "_Mov18B" , 0x47DA }, 
 { "_DarkLine" , 0x47DD }, 
 { "_ILine" , 0x47E0 }, 
 { "_IPoint" , 0x47E3 }, 
 { "_DarkPnt" , 0x47F2 }, 
 { "_CPointS" , 0x47F5 }, 
 { "_VtoWHLDE" , 0x47FB }, 
 { "_Xitof" , 0x47FE }, 
 { "_YftoI" , 0x4801 }, 
 { "_XftoI" , 0x4804 }, 
 { "_SetFuncM" , 0x4840 }, 
 { "_SetSeqM" , 0x4843 }, 
 { "_SetPolM" , 0x4846 }, 
 { "_SetParM" , 0x4849 }, 
 { "_ZmInt" , 0x484C }, 
 { "_ZmDecml" , 0x484F }, 
 { "_ZmPrev" , 0x4852 }, 
 { "_ZmUsr" , 0x4855 }, 
 { "_ZmFit" , 0x485B }, 
 { "_ZmSquare" , 0x485E }, 
 { "_ZmTrig" , 0x4861 }, 
 { "_ZooDefault" , 0x4867 }, 
 { "_GrBufCpy" , 0x486A }, 
 { "_RestoreDisp" , 0x4870 }, 
 { "_AllEq" , 0x4876 }, 
 { "_Regraph" , 0x488E }, 
 { "_PutXY" , 0x489D }, 
 { "_PDspGrph" , 0x48A3 }, 
 { "_HorizCmd" , 0x48A6 }, 
 { "_VertCmd" , 0x48A9 }, 
 { "_LineCmd" , 0x48AC }, 
 { "_UnLineCmd" , 0x48AF }, 
 { "_PointCmd" , 0x48B2 }, 
 { "_PixelTest" , 0x48B5 }, 
 { "_TanLnF" , 0x48BB }, 
 { "_DrawCmd" , 0x48C1 }, 
 { "_InvCmd" , 0x48C7 }, 
 { "_CloseEditBuf" , 0x48D3 }, 
 { "_PutSM" , 0x48D9 }, 
 { "_IsEditEmpty" , 0x492D }, 
 { "_DispEOW" , 0x4957 }, 
 { "_DispHead" , 0x495A }, 
 { "_DispTail" , 0x495D }, 
 { "_PutTokString" , 0x4960 }, 
 { "_CloseEditEqu" , 0x496C }, 
 { "_GetKey" , 0x4972 }, 
 { "_FormDisp" , 0x497E }, 
 { "_FormEReal" , 0x4990 }, 
 { "_FormDCplx" , 0x4996 }, 
 { "_FormReal" , 0x4999 }, 
 { "_OP1toEdit" , 0x49A5 }, 
 { "_GetEndVar" , 0x49D2 }, 
 { "_DispForward" , 0x49D5 }, 
 { "_DispErrorScreen" , 0x49DE }, 
 { "_CheckSplitFlag" , 0x49F0 }, 
 { "_SetNorm_Vals" , 0x49FC }, 
 { "_SetYOffset" , 0x49FF }, 
 { "_ConvKeyToTok" , 0x4A02 }, 
 { "_TokToKey" , 0x4A0B }, 
 { "_SendSkipExitPacket" , 0x4A0E }, 
 { "_GetVarCmd" , 0x4A11 }, 
 { "_SendVarCmd" , 0x4A14 }, 
 { "_SendScreenshot" , 0x4A17 }, 
 { "_keyscnlnk" , 0x4A1A }, 
 { "_DeselectAllVars" , 0x4A1D }, 
 { "_DelRes" , 0x4A20 }, 
 { "_ConvLcToLr" , 0x4A23 }, 
 { "_RedimMat" , 0x4A26 }, 
 { "_IncLstSize" , 0x4A29 }, 
 { "_InsertList" , 0x4A2C }, 
 { "_DelListEl" , 0x4A2F }, 
 { "_EditProg" , 0x4A32 }, 
 { "_CloseProg" , 0x4A35 }, 
 { "_ClrGraphRef" , 0x4A38 }, 
 { "_FixTempCnt" , 0x4A3B }, 
 { "_FindAlphaUp" , 0x4A44 }, 
 { "_FindAlphaDn" , 0x4A47 }, 
 { "_CmpSyms" , 0x4A4A }, 
 { "_CleanAll" , 0x4A50 }, 
 { "_MoveToNextSym" , 0x4A53 }, 
 { "_ConvLrToLc" , 0x4A56 }, 
 { "_ZIfRclHandler" , 0x4A65 }, 
 { "_ZIfRclkApp" , 0x4A68 }, 
 { "_RclKey" , 0x4A6B }, 
 { "_RclRegEq" , 0x4A71 }, 
 { "_CatalogChk" , 0x4A7D }, 
 { "_ErrNonReal" , 0x4A8C }, 
 { "_DisplayEquSmall" , 0x4A8F }, 
 { "_ParseInp" , 0x4A9B }, 
 { "_OP2Set60" , 0x4AB0 }, 
 { "_StoSysTok" , 0x4ABC }, 
 { "_StoAns" , 0x4ABF }, 
 { "_StoTheta" , 0x4AC2 }, 
 { "_StoR" , 0x4AC5 }, 
 { "_StoY" , 0x4AC8 }, 
 { "_StoN" , 0x4ACB }, 
 { "_StoT" , 0x4ACE }, 
 { "_StoX" , 0x4AD1 }, 
 { "_StoOther" , 0x4AD4 }, 
 { "_RclAns" , 0x4AD7 }, 
 { "_RclY" , 0x4ADA }, 
 { "_RclN" , 0x4ADD }, 
 { "_RclX" , 0x4AE0 }, 
 { "_RclVarSym" , 0x4AE3 }, 
 { "_RclSysTok" , 0x4AE6 }, 
 { "_StMatEl" , 0x4AE9 }, 
 { "_ConvOP1" , 0x4AEF }, 
 { "_Find_Parse_Formula" , 0x4AF2 }, 
 { "_CreatePair" , 0x4B0D }, 
 { "_ConvDim" , 0x4B43 }, 
 { "_ConvDim00" , 0x4B46 }, 
 { "_AnsName" , 0x4B52 }, 
 { "_IncFetch" , 0x4B73 }, 
 { "_Random" , 0x4B79 }, 
 { "_StoRand" , 0x4B7C }, 
 { "_RandInit" , 0x4B7F }, 
 { "_ResetStacks" , 0x4B82 }, 
 { "_Factorial" , 0x4B85 }, 
 { "_OneVar" , 0x4BA3 }, 
 { "_OutputExpr" , 0x4BB2 }, 
 { "_CentCursor" , 0x4BB5 }, 
 { "_GrBufClr" , 0x4BD0 }, 
 { "_DispAxes" , 0x4BE5 }, 
 { "_DispOP1A" , 0x4BF7 }, 
 { "_SetTblGraphDraw" , 0x4C00 }, 
 { "_MemClear" , 0x4C30 }, 
 { "_MemSet" , 0x4C33 }, 
 { "_ReloadAppEntryVecs" , 0x4C36 }, 
 { "_PointOn" , 0x4C39 }, 
 { "_ExecuteNewPrgm" , 0x4C3C }, 
 { "_StrLength" , 0x4C3F }, 
 { "_VPutMapRec" , 0x4C42 }, 
 { "_GetCurrentPageSub" , 0x4C45 }, 
 { "_FindAppUp" , 0x4C48 }, 
 { "_FindAppDn" , 0x4C4B }, 
 { "_FindApp" , 0x4C4E }, 
 { "_ExecuteApp" , 0x4C51 }, 
 { "_MonReset" , 0x4C54 }, 
 { "_IBounds" , 0x4C60 }, 
 { "_IOffset" , 0x4C63 }, 
 { "_DrawCirc2" , 0x4C66 }, 
 { "_CanAlphIns" , 0x4C69 }, 
 { "_GetBaseVer" , 0x4C6F }, 
 { "_AppGetCbl" , 0x4C75 }, 
 { "_AppGetCalc" , 0x4C78 }, 
 { "_SaveDisp" , 0x4C7B }, 
 { "_DisableApd" , 0x4C84 }, 
 { "_EnableApd" , 0x4C87 }, 
 { "_ApdSetup" , 0x4C93 }, 
 { "_Get_NumKey" , 0x4C96 }, 
 { "_HandleLinkActivity" , 0x4C9C }, 
 { "_LoadPattern" , 0x4CB1 }, 
 { "_SStringLength" , 0x4CB4 }, 
 { "_DoNothing" , 0x4CBD }, 
 { "_ClearRow" , 0x4CED }, 
 { "_ErrCustom1" , 0x4D41 }, 
 { "_AppStartMouse" , 0x4D47 }, 
 { "_AppEraseMouse" , 0x4D53 }, 
 { "_ClearRect" , 0x4D5C }, 
 { "_InvertRect" , 0x4D5F }, 
 { "_FillRect" , 0x4D62 }, 
 { "_AppUpdateMouse" , 0x4D65 }, 
 { "_DrawRectBorder" , 0x4D7D }, 
 { "_EraseRectBorder" , 0x4D86 }, 
 { "_FillRectPattern" , 0x4D89 }, 
 { "_DrawRectBorderClear" , 0x4D8C }, 
 { "_VerticalLine" , 0x4D95 }, 
 { "_IBoundsFull" , 0x4D98 }, 
 { "_DisplayImage" , 0x4D9B }, 
 { "_CPoint" , 0x4DC8 }, 
 { "_DeleteApp" , 0x4DCB }, 
 { "_HorizontalLine" , 0x4E67 }, 
 { "_CreateAppVar" , 0x4E6A }, 
 { "_CreateProtProg" , 0x4E6D }, 
 { "_CreateVar" , 0x4E70 }, 
 { "_AsmComp" , 0x4E73 }, 
 { "_GetAsmSize" , 0x4E76 }, 
 { "_SquishPrgm" , 0x4E79 }, 
 { "_ExecutePrgm" , 0x4E7C }, 
 { "_ChkFindSymAsm" , 0x4E7F }, 
 { "_ParsePrgmName" , 0x4E82 }, 
 { "_CSub" , 0x4E85 }, 
 { "_CAdd" , 0x4E88 }, 
 { "_CSquare" , 0x4E8B }, 
 { "_CMult" , 0x4E8E }, 
 { "_CRecip" , 0x4E91 }, 
 { "_CDiv" , 0x4E94 }, 
 { "_CAbs" , 0x4E97 }, 
 { "_CSqRoot" , 0x4E9D }, 
 { "_CLN" , 0x4EA0 }, 
 { "_CLog" , 0x4EA3 }, 
 { "_CTenX" , 0x4EA6 }, 
 { "_CEtoX" , 0x4EA9 }, 
 { "_CXrootY" , 0x4EAC }, 
 { "_CYtoX" , 0x4EB2 }, 
 { "_Conj" , 0x4EB5 }, 
 { "_CMltByReal" , 0x4EB8 }, 
 { "_CDivByReal" , 0x4EBB }, 
 { "_CTrunc" , 0x4EBE }, 
 { "_CFrac" , 0x4EC1 }, 
 { "_CIntgr" , 0x4EC4 }, 
 { "_SendHeaderPacket" , 0x4EC7 }, 
 { "_CancelTransmission" , 0x4ECA }, 
 { "_SendScreenContents" , 0x4ECD }, 
 { "_SendRAMVarData" , 0x4ED0 }, 
 { "_SendDataByte" , 0x4ED3 }, 
 { "_SendPacket" , 0x4ED6 }, 
 { "_ReceiveAck" , 0x4ED9 }, 
 { "_Send4BytePacket" , 0x4EDC }, 
 { "_Send4Bytes" , 0x4EE2 }, 
 { "_SendAByte" , 0x4EE5 }, 
 { "_SendCByte" , 0x4EE8 }, 
 { "_GetSmallPacket" , 0x4EEB }, 
 { "_GetDataPacket" , 0x4EEE }, 
 { "_SendAck" , 0x4EF1 }, 
 { "_Get4Bytes" , 0x4EF4 }, 
 { "_Get3Bytes" , 0x4EF7 }, 
 { "_Rec1stByte" , 0x4EFA }, 
 { "_Rec1stByteNC" , 0x4EFD }, 
 { "_RecAByteIO" , 0x4F03 }, 
 { "_ReceiveVar" , 0x4F06 }, 
 { "_ReceiveVarData" , 0x4F0C }, 
 { "_SrchVLstUp" , 0x4F0F }, 
 { "_SrchVLstDn" , 0x4F12 }, 
 { "_SendVariable" , 0x4F15 }, 
 { "_Get4BytesCursor" , 0x4F18 }, 
 { "_Get4BytesNC" , 0x4F1B }, 
 { "_SendDirectoryContents" , 0x4F21 }, 
 { "_SendReadyPacket" , 0x4F24 }, 
 { "_SendApplication" , 0x4F30 }, 
 { "_SendOSHeader" , 0x4F33 }, 
 { "_SendOSPage" , 0x4F36 }, 
 { "_SendOS" , 0x4F39 }, 
 { "_FlashWriteDisable" , 0x4F3C }, 
 { "_SendCmd" , 0x4F3F }, 
 { "_Disp" , 0x4F45 }, 
 { "_SendGetKeyPress" , 0x4F48 }, 
 { "_ContinueGetByte" , 0x4F4B }, 
 { "_CheckLinkLines" , 0x4F4E }, 
 { "_GetHookByte" , 0x4F51 }, 
 { "_GetBytePaged" , 0x4F54 }, 
 { "_EnableCursorHook" , 0x4F60 }, 
 { "_EnableLibraryHook" , 0x4F63 }, 
 { "_EnableRawKeyHook" , 0x4F66 }, 
 { "_ResetHookBytes" , 0x4F72 }, 
 { "_AdjustAllHooks" , 0x4F75 }, 
 { "_EnableGetCSCHook" , 0x4F7B }, 
 { "_EnableLinkActivityHook" , 0x4F84 }, 
 { "_EnableCatalog2Hook" , 0x4F8D }, 
 { "_EnableLocalizeHook" , 0x4F93 }, 
 { "_EnableTokenHook" , 0x4F99 }, 
 { "_Bit_VertSplit" , 0x4FA8 }, 
 { "_EnableHomescreenHook" , 0x4FAB }, 
 { "_EnableWindowHook" , 0x4FB1 }, 
 { "_EnableGraphHook" , 0x4FB7 }, 
 { "_DelVarArc" , 0x4FC6 }, 
 { "_DelVarNoArc" , 0x4FC9 }, 
 { "_SetAllPlots" , 0x4FCC }, 
 { "_EnableYEquHook" , 0x4FCF }, 
 { "_Arc_Unarc" , 0x4FD8 }, 
 { "_ArchiveVar" , 0x4FDB }, 
 { "_UnarchiveVar" , 0x4FDE }, 
 { "_EnableFontHook" , 0x4FE4 }, 
 { "_EnableRegraphHook" , 0x4FEA }, 
 { "_RunGraphingHook" , 0x4FF0 }, 
 { "_EnableTraceHook" , 0x4FF3 }, 
 { "_RunTracerHook" , 0x4FF9 }, 
 { "_JForceGraphNoKey" , 0x5002 }, 
 { "_JForceGraphKey" , 0x5005 }, 
 { "_PowerOff" , 0x5008 }, 
 { "_GetKeyRetOff" , 0x500B }, 
 { "_FindGroupSym" , 0x500E }, 
 { "_FillBasePageTable" , 0x5011 }, 
 { "_ArcChk" , 0x5014 }, 
 { "_FlashToRam" , 0x5017 }, 
 { "_LoadDEIndPaged" , 0x501A }, 
 { "_LoadCIndPaged" , 0x501D }, 
 { "_SetupPagedPtr" , 0x5020 }, 
 { "_PagedGet" , 0x5023 }, 
 { "_EnableParserHook" , 0x5026 }, 
 { "_EnableAppChangeHook" , 0x502C }, 
 { "_EnableGraphicsHook" , 0x5032 }, 
 { "_ILineNoHook" , 0x503B }, 
 { "_EnableCatalog1Hook" , 0x5044 }, 
 { "_EnableHelpHook" , 0x504A }, 
 { "_RunCatalog1Hook" , 0x505C }, 
 { "_DispMenuTitle" , 0x5065 }, 
 { "_EnablecxRedispHook" , 0x506B }, 
 { "_BufCpy" , 0x5071 }, 
 { "_BufClr" , 0x5074 }, 
 { "_EnableMenuHook" , 0x5083 }, 
 { "_GetBCOffsetIX" , 0x5089 }, 
 { "_ForceFullScreen" , 0x508F }, 
 { "_GetVariableData" , 0x5092 }, 
 { "_FindSwapSector" , 0x5095 }, 
 { "_CopyFlashPage" , 0x5098 }, 
 { "_FindAppNumPages" , 0x509B }, 
 { "_HLMinus5" , 0x509E }, 
 { "_SendArcPacket" , 0x50A1 }, 
 { "_ForceGraphKeypress" , 0x50A4 }, 
 { "_FormBase" , 0x50AA }, 
 { "_IsFragmented" , 0x50B0 }, 
 { "_Chk_Batt_Low" , 0x50B3 }, 
 { "_GetAppBasePage" , 0x50BC }, 
 { "_SetExSpeed" , 0x50BF }, 
 { "_GroupAllVars" , 0x50C5 }, 
 { "_EnableSilentLinkHook" , 0x50CE }, 
 { "_ResSilentLink" , 0x50D1 }, 
 { "_GetSysInfo" , 0x50DD }, 
 { "_NZIf83Plus" , 0x50E0 }, 
 { "_LinkStatus" , 0x50E3 }, 
 { "_KeyboardGetKey" , 0x50E9 }, 
 { "_RunAppLib" , 0x50EC }, 
 { "_GetVarCmdUSB" , 0x50FB }, 
 { "_GetVarVersion" , 0x510A }, 
 { "_InvertTextInsMode" , 0x5122 }, 
 { "_DispHeader" , 0x512E }, 
 { "_JForceGroup" , 0x5131 }, 
 { "_getDate" , 0x514F }, 
 { "_getDtFmt" , 0x5155 }, 
 { "_getTime" , 0x515B }, 
 { "_getTmFmt" , 0x5161 }, 
 { "_SetZeroOne" , 0x5167 }, 
 { "_IsOneTwoThree" , 0x516D }, 
 { "_IsOP112or24" , 0x5173 }, 
 { "_chkTimer0" , 0x5176 }, 
 { "_ClrWindowAndFlags" , 0x517F }, 
 { "_SetMachineID" , 0x5182 }, 
 { "_ResetLists" , 0x5185 }, 
 { "_ExecLib" , 0x5191 }, 
 { "_OpenLib" , 0x519D }, 
 { "_WaitEnterKey" , 0x51A0 }, 
 { "_IsOP1ResID" , 0x51B8 }, 
 { "_DispAboutScreen" , 0x51C7 }, 
 { "_ChkHelpHookVer" , 0x51CA }, 
 { "_Disp32" , 0x51CD }, 
 { "_DrawTableEditor" , 0x51DC }, 
 { "_MatrixName" , 0x51F1 }, 
 { "_Chk_Batt_Level" , 0x5221 }, 
 { "_GoToLastRow" , 0x5233 }, 
 { "_RectBorder" , 0x5236 }, 
 { "_LoadA5" , 0x5242 }, 
 { "_NamedListToOP1" , 0x5248 }, 
 { "_SetVertGraphActive" , 0x5284 }, 
 { "_ClearVertGraphActive" , 0x5287 }, 
 { "_EnableUSBActivityHook" , 0x528A }, 
 { "_GraphLine" , 0x529C }, 
 { "_ZifTableEditor" , 0x52B1 }, 
 { "_DeleteInvalidApps" , 0x52ED }, 
 { "_DeleteApp_Link" , 0x52F0 }, 
 { "_SetAppRestrictions" , 0x52F6 }, 
 { "_RemoveAppRestrictions" , 0x52F9 }, 
 { "_SetupHome" , 0x5302 }, 
 { "_PolarEquToOP1" , 0x5314 }, 
 { "_ClearGraphStyles" , 0x5329 }, 
 { "_MD5Final" , 0x8018 }, 
 { "_RSAValidate" , 0x801B }, 
 { "_BigNumCompare" , 0x801E }, 
 { "_WriteAByte" , 0x8021 }, 
 { "_EraseFlash" , 0x8024 }, 
 { "_FindFirstCertificateField" , 0x8027 }, 
 { "_ZeroToCertificate" , 0x802A }, 
 { "_GetCertificateEnd" , 0x802D }, 
 { "_FindGroupedField" , 0x8030 }, 
 { "_Mult8By8" , 0x8042 }, 
 { "_Mult16By8" , 0x8045 }, 
 { "_Div16By8" , 0x8048 }, 
 { "_Divide16By16" , 0x804B }, 
 { "_LoadAIndPaged" , 0x8051 }, 
 { "_FlashToRAM2" , 0x8054 }, 
 { "_GetCertificateStart" , 0x8057 }, 
 { "_GetFieldSize" , 0x805A }, 
 { "_FindSubField" , 0x805D }, 
 { "_EraseCertificateSector" , 0x8060 }, 
 { "_CheckHeaderKey" , 0x8063 }, 
 { "_Load_LFontV2" , 0x806C }, 
 { "_Load_LFontV" , 0x806F }, 
 { "_ReceiveOS" , 0x8072 }, 
 { "_FindOSHeaderSubField" , 0x8075 }, 
 { "_FindNextCertificateField" , 0x8078 }, 
 { "_GetByteOrBoot" , 0x807B }, 
 { "_GetCalcSerial" , 0x807E }, 
 { "_EraseFlashPage" , 0x8084 }, 
 { "_WriteFlashUnsafe" , 0x8087 }, 
 { "_DispBootVer" , 0x808A }, 
 { "_MD5Init" , 0x808D }, 
 { "_MD5Update" , 0x8090 }, 
 { "_MarkOSInvalid" , 0x8093 }, 
 { "_MarkOSValid" , 0x8099 }, 
 { "_CheckOSValidated" , 0x809C }, 
 { "_SetupAppPubKey" , 0x809F }, 
 { "_SigModR" , 0x80A2 }, 
 { "_TransformHash" , 0x80A5 }, 
 { "_IsAppFreeware" , 0x80A8 }, 
 { "_FindAppHeaderSubField" , 0x80AB }, 
 { "_Div32By16" , 0x80B1 }, 
 { "_GetBootVer" , 0x80B7 }, 
 { "_GetHWVer" , 0x80BA }, 
 { "_XorA" , 0x80BD }, 
 { "_bignumpowermod17" , 0x80C0 }, 
 { "_ProdNrPart1" , 0x80C3 }, 
 { "_WriteAByteSafe" , 0x80C6 }, 
 { "_WriteFlash" , 0x80C9 }, 
 { "_SetupDateStampPubKey" , 0x80CC }, 
 { "_SetFlashLowerBound" , 0x80CF }, 
 { "_LowBatteryBoot" , 0x80D2 }, 
 { "_DisplayBootMessage" , 0x80E7 }, 
 { "_NewLineWrap" , 0x80EA }, 
 { "_DispOSPercentage" , 0x80F9 }, 
 { "_ResetCalc" , 0x80FC }, 
 { "_SetupOSPubKey" , 0x80FF },
 { "" , -1 }
};
