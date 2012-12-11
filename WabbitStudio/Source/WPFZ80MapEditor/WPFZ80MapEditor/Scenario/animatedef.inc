#ifdef CORE_APP
#if ($ >> 16) = 0
#ifndef _ANIMATEDEF_DATA
#define _ANIMATEDEF_DATA

;animate_keys:

torch_key:
	.db 6
	.dw 0
	.db 6
	.dw 32
	.db 0
	.dw torch_key
	
animate_slow:
	.db 12
	.dw 0
	.db 12
	.dw 32
	.db 0
	.dw animate_slow

flower_key:
	.db 25
	.dw 0
	.db 25
	.dw 32
	.db 25
	.dw 2
	.db 25
	.dw 64
	.db 0
	.dw flower_key

grass_key:
	.db 10
	.dw 0
	.db 10
	.dw 32
	.db 10
	.dw 64
	.db 0
	.dw grass_key

#endif
#endif
#endif

#ifndef _ANIMATEDEF_INC
#define _ANIMATEDEF_INC

#include "mapdef.inc"
#define tile(xx) tile_table+(xx*32)
#ifdef _MAPEDITOR
#define ANIM_TILE_COL ($80 * 32)
#else
#define ANIM_TILE_COL 0
#endif

;animate_types:
;Upper Level Torch - Dungeon torch for the upper level
#macro upper_torch(x, y)
  nearest_tile(x, y)
  new_animate(1, x+ox, 16, y+oy, 16, rand(6)+1, torch_key+1, tile(79) | ANIM_TILE_COL)
#endmacro

;Lower Left Torch - Dungeon torch for the left lower wall
#macro lower_left_torch(x, y)
  nearest_tile(x, y)
  new_animate(1, x+ox, 16, y+oy, 16, rand(6)+1, torch_key+1, tile(81) | ANIM_TILE_COL)
#endmacro

;Lower right Torch - Dungeon torch for the right lower wall
#macro lower_right_torch(x, y)
  nearest_tile(x, y)
  new_animate(1, x+ox, 16, y+oy, 16, rand(6)+1, torch_key+1, tile(83) | ANIM_TILE_COL)
#endmacro

;Swaying flower - Animated flower
#macro sway_flower(x, y)
  nearest_tile(x, y)
  new_animate(1, x+ox, 16, y+oy, 16, 2, flower_key+1, tile(37))
#endmacro

;Swaying grass - Animated grass
#macro sway_grass(x, y)
  nearest_tile(x, y)
  new_animate(1, x+ox, 16, y+oy, 16, 2, grass_key+1, tile(12))
#endmacro

;Town torch - A torch on a stand
#macro town_torch(x, y)
  nearest_tile(x, y)
  new_animate(1, x+ox, 16, y+oy, 16, rand(6)+1, torch_key+1, tile(56) | ANIM_TILE_COL)
#endmacro

#endif
