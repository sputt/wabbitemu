#ifndef _OBJECTDEF_INC
#define _OBJECTDEF_INC

#include "mapdef.inc"
#ifdef _MAPEDITOR
#include "objects.asm"
#endif

;Generic Object - An object with no default attributes
; x - X Position
; y - Y Position
; z - Z Position
; w - Width
; h - Height
; d - Depth
; ztype - Object ID
; zflags - Flags
; ac - Anim. ctr
; ap - Anim. ptr
#macro generic_object(x, y, z, w, h, d, ztype, zflags, ac, ap)
  #ifndef z\ #define z 0\ #endif
  #ifndef w\ #define w 16\ #endif
  #ifndef h\ #define h 16\ #endif
  #ifndef d\ #define d 16\ #endif
  #ifndef ztype\ #define ztype kNo_Action\ #endif
  #ifndef zflags\ #define zflags 0\ #endif
  #ifndef ac\ #define ac 0\ #endif
  #ifndef ap\ #define ap 0\ #endif
  new_object(ztype, zflags, x, w, y, h, z, d, ac, ap)
#endmacro

;Eye - A switch that only an arrow can trigger
; x - X Position
; y - Y Position
; g - Direction
; ztype - Object ID
#macro eye(x, y, g, ztype)
  #ifndef g
    #define g d_down
  #endif
  
  #ifndef ztype
    #define ztype kNo_action
  #endif

  set_dname(g)
  
  clr()
  wr("#define this_image eye_",dname,"_gfx")
  wr("#define this_width eye_",dname,"_gfx_width")
  wr("#define this_height eye_",dname,"_gfx_height")
  run()

  .echo "Doing EYE ", dname, " height: ", this_height

  ; hug the wall opposite of the way you are facing
  wall_hug(x, y, this_width-1, this_height-1, 3-g, TRUE)

  new_object(ztype,o_destroyable,x+ox,this_width,y+oy,this_height,0,16,4,this_image)
#endmacro

;Pot - Breakable pot with items in it
; x - X Position
; y - Y Position
; w - Width [optional]
; h - Height [optional]
#macro pot(x, y, w, h)
  #ifndef w\ #define ww 12\ #else\ #define ww eval(w)\ #endif
  #ifndef h\ #define hh 12\ #else\ #define hh eval(h)\ #endif
  wall_hug(x, y, ww, hh, d_down)
  new_object(kDestroy, o_noaccess | o_destroyable, x+ox, ww, y+oy, hh, 0, 12, 0, pot_gfx)
  #undef ww
  #undef hh
#endmacro

;General Block - A block which can move to same tiles
; x - X Position
; y - Y Position
#macro general_block(x, y)
  nearest_tile(x, y)
  new_object(kPush_object1,0,x+ox,16,y+oy,16,0,0,0,block_gfx)
#endmacro

;Free Block - Free blocks can be pushed anywhere
; x - X Position
; y - Y Position
#macro free_block(x, y)
  nearest_tile(x, y)
  new_object(kFreeBlock,0,x+ox,16,y+oy,16,0,1,0,block_gfx)
#endmacro

;Plant - Destroyable plant with items in it
; x - X Position
; y - Y Position
#macro plant(x, y)
  new_object(kDestroy, o_noaccess | o_destroyable, x, 12, y, 12, 0, 12, 0, plant_gfx)
#endmacro

;Pillar - Overhead top of a pillar
; x - X Position
; y - Y Position
#macro pillar(x, y)
  nearest_tile(x, y)
  new_object(kNo_action, 0, x+ox, 16, y+oy, 16, 16, 0, 0, pillar_gfx)
#endmacro

;Scary Tree - Overhead top of the graveyard tree
; x - X Position
; y - Y Position
#macro scary_tree(x, y)
  nearest_tile(x, y)
  new_object(kNo_action, 0, x+ox, 32, y+4+oy, 12, 16, 0, 0, tree_gfx)
#endmacro

;Crystal Switch - A switch that is attacked to trigger an event
; x - X Position
; y - Y Position
; type - Object ID [optional]
#macro crystal_switch(x, y, type)
  #define crystal_switch_w 6
  #define crystal_switch_h 6
  #ifndef type
    #define type kNo_action
  #endif
  wall_hug(x, y, 6, 6, d_down)
  new_object(type, o_destroyable | o_noaccess, x+ox,6,y+oy+4,6,0, 16, 0, crystal_switch_gfx)
#endmacro

;Button - A switch that is activated by stepping
; x - X Position
; y - Y Position
; type - Object ID
#macro button(x, y, type)
  .echo "DOING BUTTON"
  #ifndef type
    #define type kNo_action
  #endif
  center_in_tile(x, y, 2, 8)
  .echo "x = ", x, " y = ", y
  new_object(type, 0, x + ox, 2, y + oy, 8, 0, 0, 0, button_gfx)
#endmacro

;Memory Block - A block which can move to same tiles
; x - X Position
; y - Y Position
#macro memory_block(x, y)
  nearest_tile(x, y)
  new_object(kMemoryBlock, 0, x+ox, 16, y+oy, 16, 0, 0, 0, block_gfx)
#endmacro

;Locked Block - A block which can move to same tiles
; x - X Position
; y - Y Position
#macro locked_door(x, y)
  nearest_tile(x, y)
  new_object(kLocked_door, o_touchable | o_noaccess, x+ox, 16, y+oy, 16, 0, 16, 0, lock_gfx)
#endmacro

;Boss Door - Door which only boss key can open
; x - X Position
; y - Y Position
#macro boss_door(x, y)
  nearest_tile(x, y)
  new_object(kBoss_door, o_touchable | o_noaccess,x+ox,16,y+oy,16,0,16,0,lock_gfx)
#endmacro

;Chest - A treasure chest containing an item
; x - X Position
; y - Y Position
; ztype - Object ID
#macro chest(x, y, ztype)
  #ifndef ztype
    #define ztype kNo_action
  #endif
  nearest_tile(x, y)
  new_object(ztype, o_touchable | o_noaccess, x+ox, 16, y+oy, 16, 0, 16, 0, chest_gfx)
#endmacro

;Sign - Link can read by touching
; x - X Position
; y - Y Position
; ztype - Object ID
#macro sign(x, y, ztype)
  #ifndef ztype
    #define ztype kNo_action
  #endif
  center_on_tile(x, y, 6, 6)
  new_object(ztype, o_touchable|o_noaccess, x+ox, 6, y+oy-2, 6, 0, 16, 0, sign_gfx)
#endmacro

;Horizontal Door Top - Door top which appears over Link
; x - X Position
; y - Y Position
#macro doortoph(x, y)
  nearest_tile(x, y)
  new_object(kNo_action,0,x+ox-4,24,y+oy,24,20,0,0,doortoph_gfx)
#endmacro

;Vertical Door Top - Door top which appears over Link
; x - X Position
; y - Y Position
#macro doortopv(x, y)
  nearest_tile(x, y)
  new_object(kNo_action,0,x+ox-4,24,y+oy-2,22,20,0,0,doortopv_gfx)
#endmacro

;Wall Top - Overhead arch for wall entrances
; x - X Position
; y - Y Position
#macro walltop(x, y)
  new_object(kNo_action,0,x,16,y+20,20,20,0,0,walltop_gfx)
#endmacro

;Heart Pickup - A collectible heart
; x - X Position
; y - Y Position
#macro heart_pickup(x, y)
  new_object(kHeart, o_destroyable | o_sparkle, x, 16, y, 15, 0, 16, 0, heart_gfx)
#endmacro

;Heart Container - A collectible heart container
; x - X Position
; y - Y Position
#macro heart_container(x, y)
  new_object(kHeart_container, 0, x, 10, y, 15, 0, 16, 10, heart_container_anim+1)
#endmacro

#endif
