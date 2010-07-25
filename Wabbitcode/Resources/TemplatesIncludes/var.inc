.nolist
;Variable allocation macro to SPASM assembler
;by Galandros

;Example usage:
; varloc(tempswaparea)
;player_x = var(1)
;player_y = var(1)
;buffer_bullets = var(128)
;saveSP = var(2)
; varloc(textshadow)
;attack = var(1)
;deffence = var(1)

#macro var(var.size)
 #define var.counter eval(var.counter+var.inc)
 #define var.inc eval(var.size)
 var.counter
 #ifdef var.end
   #if var.counter>var.end
    .error "Variable allocation overflow!"
   #endif
 #endif
#endmacro

#macro localvar(var.size)
 #define var.local eval(var.counter+var.local+var.inc)
 #define var.inc eval(var.size)
 var.counter+var.local
 #ifdef var.end
   #if var.counter+var.local>var.end
    .error "Variable allocation overflow!"
   #endif
 #endif
#endmacro


#macro varloc(varloc.value,varloc.size)
 #define var.counter eval(varloc.value)
 #define var.inc 0
 #define var.local 0
 #ifdef varloc.size
   #define var.end eval(varloc.value+varloc.size-1)
 #endif
#endmacro

#macro resetlocals()
 #define var.local 0
#endmacro
.list