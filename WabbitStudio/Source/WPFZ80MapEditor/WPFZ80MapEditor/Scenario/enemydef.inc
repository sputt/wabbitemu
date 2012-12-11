#ifndef _ENEMYDEF_INC
#define _ENEMYDEF_INC

#include "mapdef.inc"

.echo "Processing enemydef.inc"

;Generic Enemy - An enemy with no default attributes
; x - X Position
; y - Y Position
; z - Z Position
; w - Width
; h - Height
; d - Depth
; health - Object ID
; flags - Flags
; gen - Direction
; cc - Com. ctr
; cp - Com. ptr
; ac - Anim. ctr
; ap - Anim. ptr
#macro generic_enemy(x, y, z, w, h, d, health, flags, gen, cc, cp, ac, ap)
  new_enemy(health, flags, x, w, y, h, z, d, gen, ac, ap, cc, cp)
#endmacro

;Octorok - A small shooting enemy
; x - X Position
; y - Y Position
#macro octorok(x, y)
  new_enemy(2, e_4dir | et_octo, x, 12, y, 12, 0, 12, d_down, 2, octo_com+1, 2, octorok_anim+1)
#endmacro

;Sliding Trap - A trap which slides until it hits a wall
; x - X Position
; y - Y Position
; g - Direction
#macro sliding_trap(x, y, g)
  wall_hug(x, y, trap_w, trap_h, g, TRUE)

  new_enemy(1, e_invunerable | et_trap, x+ox, trap_w, y+oy, trap_h, 0, 8, g, 2, fast_trap_com+1, 255, trap_anim+1)
#endmacro

;Trigger Trap - A trap which slides when the player walks in its path
; x - X Position
; y - Y Position
; g - Direction
#macro trigger_trap(x, y, g)
  wall_hug(x, y, trap_w, trap_h, g, TRUE)

  new_enemy(1, e_invunerable | et_trap, x+ox, trap_w, y+oy, trap_h, 0, 8, g, 255, trigger_trap_detect_com+1, 255, trap_anim+1)
#endmacro

;Wall Trap - A trap which moves along a room's wall
; x - X Position
; y - Y Position
; g - Direction
#macro trigger_trap(x, y, g)
  wall_hug(x, y, trap_w, trap_h, g, TRUE)

  new_enemy(1, e_invunerable | et_trap, x+ox, trap_w, y+oy, trap_h, 0, 8, g, 255, wall_trap_com+1, 255, trap_anim+1)
#endmacro

;Chu - A gelatinous jumping blob
; x - X Position
; y - Y Position
#macro chu(x, y)
  new_enemy(3, et_chu, x, chu_w, y, chu_h, 0, 8, d_down, 2, enemy_jump_com+1, 2, chu_anim+1)
#endmacro

;Small Chu - A gelatinous jumping blob
; x - X Position
; y - Y Position
#macro small_chu(x, y)
  new_enemy(1, et_small_chu, x, 7, y, 7, 0, 5, d_down, 20, enemy_jump_com+1, 4, small_chu_anim+1)
#endmacro

;Bat - Fluttering hellspawn
; x - X Position
; y - Y Position
#macro bat(x, y)
  new_enemy(1, et_bat, x, bat_w, y, bat_h, 0, 16, d_down, 10, bat_com+1, 10, bat_anim+1)
#endmacro

;Platform - Moving platform
; x - X Position
; y - Y Position
; g - Direction
; m - Associated misc index
#macro platform(x, y, g, m)
  nearest_tile(x, y)
  new_enemy(m*misc_width+1, e_nodir | e_invunerable | e_noharm, x+ox,16,y+oy,16,0,0,g,255,platform_com+1,255,platform_anim+1)
#endmacro

#endif
