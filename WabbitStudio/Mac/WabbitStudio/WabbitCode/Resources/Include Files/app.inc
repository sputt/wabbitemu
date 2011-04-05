; Fatal error macro. Displays message prefixed with "error" 
; and returns with error code
#define fatal(str,return) .error str
#macro defpage(page, appname)
#if page = 0
  #ifndef gpage
    #define free_space 0
    #define gpage 0
    #ifndef appname
      .echo "Warning: No name given, using \"Default\""
      #define appname "Default"
    #endif
    .org $4000
    .db $80,$0F,0,0,0,0
    .db $80,$12,1,4
    .db $80,$21,1
    .db $80,$31,1
    .db $80,$48

	#define gappname eval(appname)

_   .db appname
    #define apnamlen eval($ - -_)
    echo_fill(apnamlen,"-","------------------------------------------")
    .echo "Beginning APP \"",appname,"\", courtesy of SPASM macros"
    echo_fill(apnamlen,"-","------------------------------------------")

    #if apnamlen <= 8
      .fill 8-apnamlen,' '
    #else
      #define err "App name is ",apnamlen-8," character(s) too long."
      fatal(err,1)
    #endif

    .db $80,$81,0
    .db $80,$90
    .db $03,$26,$09,$04,$04,$06f,$1b,$80
    .db $02,$0d,$40
    .db $a1,$6b,$99,$f6,$59,$bc,$67 
    .db $f5,$85,$9c,$09,$6c,$0f,$b4,$03,$9b,$c9 
    .db $03,$32,$2c,$e0,$03,$20,$e3,$2c,$f4,$2d 
    .db $73,$b4,$27,$c4,$a0,$72,$54,$b9,$ea,$7c 
    .db $3b,$aa,$16,$f6,$77,$83,$7a,$ee,$1a,$d4 
    .db $42,$4c,$6b,$8b,$13,$1f,$bb,$93,$8b,$fc
    .db $19,$1c,$3c,$ec,$4d,$e5,$75 
    .db $80,$7F,0,0,0,0
    .dw 0,0,0,0,0,0,0,0
  #else
    fatal("There was already a page 0.",1)
  #endif
#else
  #if ($ & $0000FFFF) > $8000
   #define err "Page ",gpage," went over bounds by ",$-$8000," bytes."
   fatal(err, 1)
  #endif
  #if page <= gpage
    fatal("Your page numbers must increase.")
  #endif
  #if page > gpage+1
    .echo "Warning: Skipping page ",gpage+1," at user request."
  #endif
  #define free_space eval(free_space + $8000-($ & $0000FFFF))
  .echo "Page ",gpage," free space: ",$8000-($ & $0000FFFF)
  .block ($8000-($ & $0000FFFF) + ((page-gpage-1)*$4000))
  #define gpage eval(page)
  .org $4000+(gpage*$10000)
#endif
#endmacro
	
#macro validate
#if ($ & $0000FFFF) > $8000
  #define err "Page ",gpage," went over bounds by ",$-$8000," bytes."
  fatal(err, 1)
#else
  .echo "Page ",gpage," free space: ",$8000-($ & $0000FFFF)
  #define free_space eval(free_space + $8000 - ($ & $0000FFFF))
  .echo "Success: Page sizes validated and \"",gappname,"\" is ready for signing."
  .echo "           In ",gpage+1," page(s), ",free_space," bytes are available."
#endif
#endmacro

#macro echo_fill(times, char, base)
#if times > 0
 #define base base,char
 echo_fill(eval(times-1), char, base)
#else
 .echo base
#endif
#endmacro

#define .defpage defpage(
#define .validate validate