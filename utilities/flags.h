
typedef struct {
	TCHAR name[32];
	int bit;
} bits_t;

typedef struct {
	TCHAR name[32];
	int flag;
	bits_t bits[8];
} flag_t;


flag_t flags83p[] = {
	{ _T("trigFlags"), 0x00, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("trigDeg"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("kbdFlags"), 0x00, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("kbdSCR"), 3 },
		{ _T("kbdKeyPress"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("doneFlags"), 0x00,
		{ { _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("donePrgm"), 5 },

		{ _T(""), -1 },
		{ _T(""), -1 } }
	},
	{ _T("ioDelFlag"), 0x00, {
		{ _T("inDelete"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("editFlags"), 0x01, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("editOpen"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("monFlags"), 0x01, {
		{ _T(""), -1 },
		 { _T(""), -1 },
		 { _T(""), -1 },
		 { _T(""), -1 },
		 { _T("monAbandon"), 4 },
		 { _T(""), -1 },
		 { _T(""), -1 },
		 { _T(""), -1 }
	} },
	{ _T("plotFlags"), 0x02, {
		{ _T(""), -1 },
		{ _T("plotLoc"), 1 },
		{ _T("plotDisp"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("grfModeFlags"), 0x02, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("grfFuncM"), 4 },
		{ _T("grfPolarM"), 5 },
		{ _T("grfParamM"), 6 },
		{ _T("grfRecurM"), 7 }
	} },
	{ _T("graphFlags"), 0x03, {
		{ _T("graphDraw"), 0 },
		{ _T(""), -1 },
		{ _T("graphCursor"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("grfDBFlags"), 0x04, {
		{ _T("grfDot"), 0 },
		{ _T("grfSimul"), 1 },
		{ _T("grfGrid"), 2 },
		{ _T("grfPolar"), 3 },
		{ _T("grfNoCoord"), 4 },
		{ _T("grfNoAxis"), 5 },
		{ _T("grfLabel"), 6 },
		{ _T(""), -1 }
	} },
	{ _T("textFlags"), 0x05, {
		{ _T(""), -1 },
		{ _T("textEraseBelow"), 1 },
		{ _T("textScrolled"), 2 },
		{ _T("textInverse"), 3 },
		{ _T("textInsMode"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("ParsFlag2"), 0x07, {
		{ _T("numOP1"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("newDispF"), 0x08, {
		{ _T("preClrForMode"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("apdFlags"), 0x08, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("apdAble"), 2 },
		{ _T("apdRunning"), 3 },
		{ _T("apdWarmStart"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("onFlags"), 0x09, {
		{ _T(""), -1 },
		{ _T("parseInput"), 1 },
		{ _T(""), -1 },
		{ _T("onRunning"), 3 },
		{ _T("onInterrupt"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("statFlags"), 0x09, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("statsValid"), 6 },
		{ _T("statANSDISP"), 7 }
	} },
	{ _T("fmtFlags"), 0x0A, {
		{ _T("fmtExponent"), 0 },
		{ _T("fmtEng"), 1 },
		{ _T("fmtHex"), 2 },
		{ _T("fmtOct"), 3 },
		{ _T("fmtBin"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("numMode"), 0x0A, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("fmtReal"), 5 },
		{ _T("fmtRect"), 6 },
		{ _T("fmtPolar"), 7 }
	} },
	{ _T("fmtOverride"), 0x0B, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("fmtEditFlags"), 0x0C, {
		{ _T("fmtEdit"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("curFlags"), 0x0C, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("curAble"), 2 },
		{ _T("curOn"), 3 },
		{ _T("curLock"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("appFlags"), 0x0D, {
		{ _T("appWantIntrpt"), 0 },
		{ _T("appTextSave"), 1 },
		{ _T("appAutoScroll"), 2 },
		{ _T("appMenus"), 3 },
		{ _T("appLockMenus"), 4 },
		{ _T("appCurGraphic"), 5 },
		{ _T("appCurWord"), 6 },
		{ _T("appExit"), 7 }
	} },
	{ _T("rclFlag"), 14, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("enableQueue"), 7 }
	} },
	{ _T("seqFlags"), 0x0F, {
		{ _T("webMode"), 0 },
		{ _T("webVert"), 1 },
		{ _T("sequv"), 2 },
		{ _T("seqvw"), 3 },
		{ _T("sequw"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("promptFlags"), 0x11, {
		{ _T("promptEdit"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("indicFlags"), 0x12, {
		{ _T("indicRun"), 0 },
		{ _T("indicInUse"), 1 },
		{ _T("indicOnly"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("shiftFlags"), 0x12, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("shift2nd"), 3 },
		{ _T("shiftAlpha"), 4 },
		{ _T("shiftLwrAlph"), 5 },
		{ _T("shiftALock"), 6 },
		{ _T("shiftKeepAlph"), 7 }
	} },
	{ _T("tblFlags"), 0x13, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("autoFill"), 4 },
		{ _T("autoCalc"), 5 },
		{ _T("reTable"), 6 },
		{ _T(""), -1 }
	} },
	{ _T("sGrFlags"), 0x14, {
		{ _T("grfSplit"), 0 },
		{ _T("vertSplit"), 1 },
		{ _T("grfSChanged"), 2 },
		{ _T("grfSplitOverride"), 3 },
		{ _T("write_on_graph"), 4 },
		{ _T("g_style_active"), 5 },
		{ _T("cmp_mod_box"), 6 },
		{ _T("textWrite"), 7 }
	} },
	{ _T("newIndicFlags"), 0x15, {
		{ _T("extraIndic"), 0 },
		{ _T("saIndic"), 1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("newFlags2"), 0x16, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("noRestores"), 5 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("interruptFlags"), 0x16, {
		{ _T("secondTimerEnabled"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("batteryFlags"), 0x16, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("batteriesGood"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("smartFlags"), 0x17, {
		{ _T("smartGraph"), 0 },
		{ _T("smartGraph_inv"), 1 },
		{ _T(""), -1 },
		{ _T("smarter_mask"), 3 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("statFlags2"), 0x1A, {
		{ _T("statDiagnosticsOn"), 0 },
		{ _T(""), -1 },
		{ _T("No_Del_Stat"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("linkFlags"), 0x1B, {
		{ _T(""), -1 },
		{ _T("IDis95h"), 1 },
		{ _T("IDis82h"), 2 },
		{ _T("IDis83h"), 3 },
		{ _T("IDis03h"), 4 },
		{ _T("USBenabled"), 5 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("asm_Flag1"), 0x21, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("asm_Flag2"), 0x22, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("asm_Flag3"), 0x23, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("getSendFlg"), 0x24, {
		{ _T(""), -1 },
		{ _T("comFailed"), 1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("selfTestFlg"), 0x24, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("resetOnPowerOn"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
		} },
	{ _T("appLwrCaseFlag"), 0x24, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("lwrCaseActive"), 3 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("contextFlags"), 0x25, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("nocxPutAway"), 5 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("groupFlags"), 0x26, {
		{ _T(""), -1 },
		{ _T("inGroup"), 1 },
		{ _T("noCompletionByte"), 2 },
		{ _T("noDataWrite"), 3 },
		{ _T(""), -1 },
		{ _T("writeSizeBytesOnly"), 5 },
		{ _T(""), -1 },
		{ _T(""), -1 }
		} },
	{ _T("apiFlag"), 0x28, {
		{ _T("AppAllowContext"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("appRunning"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("appRetOffKey"), 7 }
	} },
	{ _T("apiFlg2"), 0x29, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
		} },
	{ _T("apiFlg3"), 0x2A, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("apiFlg4"), 0x2B, {
		{ _T(""), -1 },
		{ _T("cellOverride"), 1 },
		{ _T("fullScrnDraw"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("xapFlag0"), 0x2E, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
		} },
	{ _T("xapFlag1"), 0x2F, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
		} },
	{ _T("xapFlag2"), 0x30, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("xapFlag3"), 0x31, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("fontFlags"), 0x32, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("fracDrawLFont"), 2 },
		{ _T("fracTallLFont"), 3 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("customFont"), 7 }
	} },
	{ _T("scriptFlag"), 0x33, {
		{ _T("alt_On"), 0 },
		{ _T("alt_Off"), 1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("rclFlag2"), 0x33, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("useRclQueueEnd"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("hookflags1"), 0x33, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("ignoreBPLink"), 3 },
		{ _T("bPLinkOn"), 4 },
		{ _T("enableKeyEcho"), 5 },
		{ _T("noTempDelete"), 6 },
		{ _T(""), -1 }
	} },
	{ _T("sysHookFlg"), 0x34, {
		{ _T("getCSCHookActive"), 0 },
		{ _T("libraryHookActive"), 1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("homescreenHookActive"), 4 },
		{ _T("rawKeyHookActive"), 5 },
		{ _T("catalog2HookActive"), 6 },
		{ _T("cursorHookActive"), 7 }
	} },
	{ _T("sysHookFlg1"), 0x35, {
		{ _T("tokenHookActive"), 0 },
		{ _T("localizeHookActive"), 1 },
		{ _T("windowHookActive"), 2 },
		{ _T("graphHookActive"), 3 },
		{ _T("yEquHookActive"), 4 },
		{ _T("fontHookActive"), 5 },
		{ _T("regraphHookActive"), 6 },
		{ _T("drawingHookActive"), 7 }
	} },
	{ _T("sysHookFlag2"), 0x36, {
		{ _T("traceHookActive"), 0 },
		{ _T("parserHookActive"), 1 },
		{ _T("appChangeHookActive"), 2 },
		{ _T("catalog1HookActive"), 3 },
		{ _T("helpHookActive"), 4 },
		{ _T("cxRedispHookActive"), 5 },
		{ _T("menuHookActive"), 6 },
		{ _T("silentLinkHookActive"), 7 }
	} },
	{ _T("plotFlag3"), 0x3C, {
		{ _T("bufferOnly"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("useFastCirc"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
		} },
	{ _T("dBKeyFlags"), 0x3D, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("keyDefaultsF"), 6 },
		{ _T(""), -1 }
	} },
	{ _T("silentLinkFlags"), 0x3E, {
		{ _T("silentLinkActive"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("extraHookFlags"), 0x3E, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("checkCatalog2HookVer"), 3 },
		{ _T("openLibActive"), 4 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("clockFlags"), 0x3F, {
		{ _T("notMDYMode"), 0 },
		{ _T("isYMDMode"), 1 },
		{ _T("is24Hour"), 2 },
		{ _T("inAfternoon"), 3 },
		{ _T("useTokensInString"), 4 },
		{ _T("displayClock"), 5 },
		{ _T("clockOn"), 6 },
		{ _T(""), -1 }
	} },
	{ _T("USBFlag1"), 0x40, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("usbReceiveZone1"), 2 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("USBFlag2"), 0x41, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T("noUSBHostInit"), 3 },
		{ _T(""), -1 },
		{ _T("usbDataWaiting"), 5 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T("USBFlag3"), 0x42, {
		{ _T("usbTimeoutError"), 0 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
	{ _T(""), -1, {
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 },
		{ _T(""), -1 }
	} },
};
