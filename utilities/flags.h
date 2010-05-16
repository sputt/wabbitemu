
typedef struct {
	char name[32];
	int bit;
} bits_t;

typedef struct {
	char name[32];
	int flag;
	bits_t bits[8];
} flag_t;


flag_t flags83p[] = {
 {"trigFlags",0,{ { "" , -1 } , { "" , -1 } , { "trigDeg" , 2 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"kbdFlags",0,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "kbdSCR" , 3 } , { "kbdKeyPress" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"doneFlags",0,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "donePrgm" , 5 } , { "" , -1 } , { "" , -1 } } },
 {"ioDelFlag",0,{ { "inDelete" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"editFlags",1,{ { "" , -1 } , { "" , -1 } , { "editOpen" , 2 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"monFlags",1,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "monAbandon" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"plotFlags",2,{ { "" , -1 } , { "plotLoc" , 1 } , { "plotDisp" , 2 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"grfModeFlags",2,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "grfFuncM" , 4 } , { "grfPolarM" , 5 } , { "grfParamM" , 6 } , { "grfRecurM" , 7 } } },
 {"graphFlags",3,{ { "graphDraw" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"grfDBFlags",4,{ { "grfDot" , 0 } , { "grfSimul" , 1 } , { "grfGrid" , 2 } , { "grfPolar" , 3 } , { "grfNoCoord" , 4 } , { "grfNoAxis" , 5 } , { "grfLabel" , 6 } , { "" , -1 } } },
 {"textFlags",5,{ { "" , -1 } , { "textEraseBelow" , 1 } , { "textScrolled" , 2 } , { "textInverse" , 3 } , { "textInsMode" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"ParsFlag2",7,{ { "numOP1" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"newDispF",8,{ { "preClrForMode" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"apdFlags",8,{ { "" , -1 } , { "" , -1 } , { "apdAble" , 2 } , { "apdRunning" , 3 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"onFlags",9,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "onRunning" , 3 } , { "onInterrupt" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"statFlags",9,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "statsValid" , 6 } , { "statANSDISP" , 7 } } },
 {"fmtFlags",10,{ { "fmtExponent" , 0 } , { "fmtEng" , 1 } , { "fmtHex" , 2 } , { "fmtOct" , 3 } , { "fmtBin" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"numMode",10,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "fmtReal" , 5 } , { "fmtRect" , 6 } , { "fmtPolar" , 7 } } },
 {"fmtOverride",11,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"fmtEditFlags",12,{ { "fmtEdit" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"curFlags",12,{ { "" , -1 } , { "" , -1 } , { "curAble" , 2 } , { "curOn" , 3 } , { "curLock" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"appFlags",13,{ { "appWantIntrpt" , 0 } , { "appTextSave" , 1 } , { "appAutoScroll" , 2 } , { "appMenus" , 3 } , { "appLockMenus" , 4 } , { "appCurGraphic" , 5 } , { "appCurWord" , 6 } , { "appExit" , 7 } } },
 {"seqFlags",15,{ { "webMode" , 0 } , { "webVert" , 1 } , { "sequv" , 2 } , { "seqvw" , 3 } , { "sequw" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"promptFlags",17,{ { "promptEdit" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"indicFlags",18,{ { "indicRun" , 0 } , { "indicInUse" , 1 } , { "indicOnly" , 2 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"shiftFlags",18,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "shift2nd" , 3 } , { "shiftAlpha" , 4 } , { "shiftLwrAlph" , 5 } , { "shiftALock" , 6 } , { "shiftKeepAlph" , 7 } } },
 {"tblFlags",19,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "autoFill" , 4 } , { "autoCalc" , 5 } , { "reTable" , 6 } , { "" , -1 } } },
 {"sGrFlags",20,{ { "grfSplit" , 0 } , { "vertSplit" , 1 } , { "grfSChanged" , 2 } , { "grfSplitOverride" , 3 } , { "write_on_graph" , 4 } , { "g_style_active" , 5 } , { "cmp_mod_box" , 6 } , { "textWrite" , 7 } } },
 {"newIndicFlags",21,{ { "extraIndic" , 0 } , { "saIndic" , 1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"newFlags2",22,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "noRestores" , 5 } , { "" , -1 } , { "" , -1 } } },
 {"smartFlags",23,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"more_Flags",26,{ { "" , -1 } , { "" , -1 } , { "No_Del_Stat" , 2 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"linkFlags",27,{ { "" , -1 } , { "IDis95h" , 1 } , { "IDis82h" , 2 } , { "IDis83h" , 3 } , { "IDis03h" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"asm_Flag1",33,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"asm_Flag2",34,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"asm_Flag3",35,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"getSendFlg",36,{ { "" , -1 } , { "comFailed" , 1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"appLwrCaseFlag",36,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "lwrCaseActive" , 3 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"apiFlag",40,{ { "AppAllowContext" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "appRunning" , 4 } , { "" , -1 } , { "" , -1 } , { "appRetOffKey" , 7 } } },
 {"apiFlg3",42,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"apiFlg4",43,{ { "" , -1 } , { "" , -1 } , { "fullScrnDraw" , 2 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"groupFlags",38,{ { "" , -1 } , { "inGroup" , 1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"xapFlag0",46,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"xapFlag1",47,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"xapFlag2",48,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"xapFlag3",49,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"fontFlags",50,{ { "" , -1 } , { "" , -1 } , { "fracDrawLFont" , 2 } , { "fracTallLFont" , 3 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "customFont" , 7 } } },
 {"plotFlag3",60,{ { "bufferOnly" , 0 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "useFastCirc" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"scriptFlag",51,{ { "alt_On" , 0 } , { "alt_Off" , 1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"rclFlag2",51,{ { "" , -1 } , { "" , -1 } , { "useRclQueueEnd" , 2 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"backGroundLink",51,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "ignoreBPLink" , 3 } , { "bPLinkOn" , 4 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } },
 {"sysHookFlg",52,{ { "getCSCHookActive" , 0 } , { "libraryHookActive" , 1 } , { "" , -1 } , { "" , -1 } , { "homescreenHookActive" , 4 } , { "rawKeyHookActive" , 5 } , { "catalog2HookActive" , 6 } , { "cursorHookActive" , 7 } } },
 {"sysHookFlg1",53,{ { "tokenHookActive" , 0 } , { "localizeHookActive" , 1 } , { "windowHookActive" , 2 } , { "graphHookActive" , 3 } , { "yEquHookActive" , 4 } , { "fontHookActive" , 5 } , { "regraphHookActive" , 6 } , { "drawingHookActive" , 7 } } },
 {"sysHookFlag2",54,{ { "traceHookActive" , 0 } , { "parserHookActive" , 1 } , { "appChangeHookActive" , 2 } , { "catalog1HookActive" , 3 } , { "helpHookActive" , 4 } , { "cxRedispHookActive" , 5 } , { "menuHookActive" , 6 } , { "silentLinkHookActive" , 7 } } },
 {"dBKeyFlags",61,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "keyDefaultsF" , 6 } , { "" , -1 } } },
 {"",-1,{ { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } , { "" , -1 } } }
};
