#ifndef _MISCDEF_INC
#define _MISCDEF_INC

#include "mapdef.inc"

;Generic Misc - A misc with custom code
; x - X Position
; y - Y Position
; w - Width
; h - Height
; code - Code
#macro generic_misc(x, y, w, h, zcode)
  new_misc(1, x, w, y, h, zcode, 0)
#endmacro

;Simple Hole - A hole which replaces the player at the point of fall
; x - X Position
; y - Y Position
; w - Width
; h - Height
#macro hole(x, y, w, h)
  new_misc(1, x+4, w-8, y+4, h-8, fall_in, 0)
#endmacro

;Replace Hole - A hole which replaces the player a specific point
; x - X Position
; y - Y Position
; w - Width
; h - Height
; rep_x - X Replace
; rep_y - Y Replace
#macro hole_replace(x, y, w, h, rep_x, rep_y)
  new_misc(1, x+4, w-8, y+4, h-8, fall_in_replace, rep_y << 8 | rep_x)
#endmacro

;Extend Enemy Screen - A misc which extends the range at which enemies are handled
; x - X Position
; y - Y Position
; w - Width
; h - Height
#macro extend_enemy_screen(x, y, w, h)
  new_misc(1, x, w, y, h, extend_screen_e, 0)
#endmacro

;Script Once - Runs a script then prevents itself
; x - X Position
; y - Y Position
; w - Width
; h - Height
; scr - Script label
#macro script_once(x, y, w, h, scr)
  new_misc(1, x, w, y, h, misc_script_once, scr)
#endmacro

#endif