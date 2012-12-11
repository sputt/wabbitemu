;; Mapdef.inc, December 2009
#ifndef _MAPDEF_INC
#define _MAPDEF_INC

.echo "DOING MAPDEF.INC"
#include "edef.inc"

; Global offsets
#define ox 0
#define oy 0

; Global direction name
#define dname ""

; Current section
#define current_section -1

#ifndef zerror
  #define zerror(zstr) 
#endif

#define section_animate 0
#define section_object 	1
#define section_enemy 	2
#define section_misc 	3

#ifdef _MAPEDITOR
d_down = 0
d_left = 1
d_right = 2
d_up = 3
d_none = 4
#endif

#macro animate_section()
	#define section_line eval(__line)
	#define current_section eval(section_animate)
	clr()
	wr("_:")
	run()
	.dw +_ - $ - 2
#endmacro

#macro object_section()
	#define section_line eval(__line)
	#define current_section eval(section_object)
	clr()
	wr("_:")
	run()
	.dw +_ - $ - 2
#endmacro

#macro enemy_section()
	#define section_line eval(__line)
	#define current_section eval(section_enemy)
	#if ($ - -_ - 2) / object_width > object_amount
	  .echo "Zelda: ",__file,":",section_line,": ",($ - -_ - 2) / object_width - object_amount," too many objects"
	#endif
	
	clr()
	wr("_:")
	run()
	.dw +_ - $ - 2
#endmacro

#macro misc_section()
	#define section_line eval(__line)
	#define current_section eval(section_misc)
	#if ($ - -_ - 2) % enemy_width != 0
	  .echo "Zelda: ",__file,":",section_line,": Enemy section was off by ",($ - -_ - 2) % enemy_width
	#endif
	
	clr()
	wr("_:")
	run()
	.dw +_ - $ - 2
#endmacro

#macro end_section()
	#define current_section -1
	clr()
	wr("_:")
	run()
#endmacro

#macro new_object(type, zflags, x, w, y, h, z, d, ac, ap)
  #if current_section != section_object
    zerror("Object created in wrong section!")
  #endif

  #if x > (16*16-w)
    #define x eval(16*16-w)
  #endif

  #if y > (16*16-h)
    #define y eval(16*16-h)
  #endif

  #if x < 0
    #define x 0
  #endif

  #if y < 0
    #define y 0
  #endif
  
  #ifndef ap
    zerror("Not enough arguments passed to new_object")
  #else
    #ifndef type
      .db kNo_action
    #else
    .db type
    #endif
    .db zflags, x, w, y, h, z, d, ac
    .dw ap
  #endif
#endmacro

#macro nearest_tile(xx, yy)
  #define ox ((xx+8)/16*16 - xx)
  #define oy ((yy+8)/16*16 - yy)
#endmacro

#macro center_on_tile(xx, yy, ww, hh)
  nearest_tile(xx, yy)
  #define ox ox + ((16-ww)/2)
  #define oy oy + ((16-hh)/2)
#endmacro

#macro nearest_half_tile(xx, yy)
  #define ox ((xx+4)/8*8 - xx)
  #define oy ((yy+4)/8*8 - yy)
#endmacro

; returns offset in ox and oy
; use those to determine the change in position
; for the final object
#macro wall_hug(xx, yy, ww, hh, gg, wh_use_half_tile)

  #ifdef wh_use_half_tile
    nearest_half_tile(xx, yy)
  #else
    nearest_tile(xx, yy)
  #endif

  #if gg = d_down
    #define ox ox + ((16-ww)/2)
  #endif
  #if gg = d_right
    #define oy oy + ((16-hh)/2)
  #endif
  #if gg = d_left
    #define ox ox + (16-ww))
    #define oy oy + ((16-hh)/2)
  #endif
  #if gg = d_up
    #define ox ox + ((16-ww)/2)
    #define oy oy + (16-hh)
  #endif
#endmacro

; Centers an object in the middle of a tile
#macro center_in_tile(xx, yy, ww, hh)
  .echo "CENTER IN TILE"
  nearest_tile(xx, yy)
  .echo "ox = ", ox, " oy = ", oy

  #define ox ox + ((16-ww)/2)
  #define oy oy + ((16-hh)/2)
  .echo "ox = ", ox, " oy = ", oy
#endmacro

; Given a gen state, set a global variable for direction name
#macro set_dname(gg)
  #if gg = d_down
    #define dname "down"
  #endif
  #if gg = d_left
    #define dname "left"
  #endif
  #if gg = d_right
    #define dname "right"
  #endif
  #if gg = d_up
    #define dname "up"
  #endif
  #if gg = d_none
    #define dname "none"
  #endif
#endmacro

; ANIMATE
#macro new_animate(aa, xx, ww, yy, hh, ac, sp, ap)
  #if current_section != section_animate
    zerror("Animated tile created in the wrong section!")
  #endif
  
  #ifndef ap
    zerror("Not enough arguments passed to new_animate")
  #else
    .db aa, xx, ww, yy, hh, ac
    .dw sp, ap
  #endif
#endmacro

#include "animatedef.inc"


; OBJECTS
#include "objectdef.inc"


; ENEMIES
#macro new_enemy(aa, ff, xx, ww, yy, hh, zz, dd, gg, cc, cp, ac, ap)
  #if current_section != section_enemy
    zerror("Enemy created in wrong section!")
  #endif
  
  #ifndef cp
    zerror("Not enough arguments passed to new_enemy")
  #else
    .db aa, ff, xx, ww, yy, hh, zz, dd, gg, cc
    .dw cp
    .db ac
    .dw ap
  #endif
#endmacro

#include "enemydef.inc"


; MISC
#macro new_misc(aa, xx, ww, yy, hh, cc, ax) 
  #if current_section != section_misc
    zerror("Misc created in wrong section!")
  #endif
  
  .db aa, xx, ww, yy, hh
  .dw cc
	
  #ifndef ax
	.dw 00
  #else
	.dw ax
  #endif
#endmacro

#include "miscdef.inc"

#endif