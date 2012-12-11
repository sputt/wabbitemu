#define with .option

;; Global graphics options
.option bm_pad = 1
.option bm_min_w = 16

;; Most of the batch has these settings
.option bm_hdr, bm_msk

images_start:

trap_gfx
#include "trap.bmp"

#ifdef _MAPEDITOR
link_gfx with bm_map = 2x4
#include "link walk mapeditor.bmp"
link_fart_gfx = link_gfx

link_push_gfx with bm_map = 2x4
#include "link push.bmp"

#endif

heart_container_gfx with bm_map = 1x4
#include "heart container.bmp"

;; ------------------------------------ ;;
;; Objects
generic_gfx
#include "generic.bmp"

.dw pot_explode
pot_gfx with bm_map = 1x3
#include "pot.bmp"
pot_explode_gfx1 = pot_gfx2
pot_explode_gfx2 = pot_gfx3

;.dw plant_explode
;plant_gfx with bm_map = 1x3
;#include "plant.bmp"
;plant_explode_gfx1 = plant_gfx2
;plant_explode_gfx2 = plant_gfx3

plant_gfx
#include "planttest.bmp"

eye_left_gfx with bm_map = 1x2
#include "eye left.bmp"
eye_left_closed_gfx = eye_left_gfx2

eye_right_gfx with bm_map = 1x2
#include "eye right.bmp"
eye_right_closed_gfx = eye_right_gfx2

eye_down_gfx with bm_map = 1x2
#include "eye down.bmp"
eye_down_closed_gfx = eye_down_gfx2

crystal_switch_gfx with bm_map = 1x2
#include "crystal switch.bmp"
crystal_switch_dark_gfx = crystal_switch_gfx2

button_gfx with bm_map = 1x2
#include "button.bmp"
button_pressed_gfx = button_gfx2

crack_gfx
#include "crack.bmp"

.dw explode_anim+4
lock_gfx
#include "lock.bmp"

key_gfx
#include "key.bmp"

heart_gfx
#include "heart.bmp"

rupee_gfx
#include "rupee.bmp"

triforce_gfx with bm_map = 2x2
#include "triforce.bmp"

clock_gfx
#include "clock.bmp"

sword_pedestal_gfx
#include "sword pedestal.bmp"

bomb_gfx
#include "bomb.bmp"

bow_gfx
#include "bow.bmp"

shield_gfx
#include "shield.bmp"

boss_key_gfx
#include "boss key.bmp"

doortoph_gfx
#include "doortoph.bmp"

doortopv_gfx
#include "doortopv.bmp"

walltop_gfx
#include "walltop.bmp"

pillar_gfx
#include "pillar.bmp"

.dw pot_explode
block_gfx
#include "block.bmp"

tree_gfx
#include "tree.bmp"

chest_gfx
#include "chest.bmp"

sign_gfx
#include "sign.bmp"

platform_gfx
#include "platform.bmp"

;; ------------------------------------ ;;
;; Link
link_attack_gfx with bm_map = 2x4
#include "link attack.bmp"

link_dead_gfx
#include "link dead.bmp"

link_present_gfx
#include "link present.bmp"
 
;Sword images
sword_r_gfx
#include "sword\sword r.bmp"
sword_tr_gfx
#include "sword\sword tr.bmp"
sword_t_gfx
#include "sword\sword t.bmp"
sword_tl_gfx
#include "sword\sword tl.bmp"
sword_l_gfx
#include "sword\sword l.bmp"
sword_bl_gfx
#include "sword\sword bl.bmp"
sword_b_gfx
#include "sword\sword b.bmp"
sword_br_gfx
#include "sword\sword br.bmp"

;; ------------------------------------ ;;
;; Enemies

bat_gfx with bm_map = 1x2
#include "bat.bmp"

octorok_gfx with bm_map = 2x4
#include "octorok.bmp"

shot_gfx
#include "shot.bmp"

mirrorman_gfx with bm_map = 2x4
#include "mirrorman.bmp"

spider_gfx with bm_map = 1x2
#include "spider.bmp"

chu_gfx with bm_map = 1x2
#include "chu.bmp"

small_chu_gfx
#include "chu small.bmp"

boss_bird_gfx
#include "boss bird land.bmp"

boss_bird_fly_gfx with bm_map = 1x2
#include "boss bird fly.bmp"

peahat_gfx with bm_map = 1x2
#include "peahat.bmp"

shooter_gfx
#include "shooter.bmp"

ganon_gfx with bm_map = 2x2
#include "ganon.bmp"

explode_gfx with bm_map = 1x3
#include "explode.bmp"

;; ------------------------------------ ;;
;; NPCs

marin_gfx with bm_map = 2x4
#include "marin.bmp"

kid_gfx with bm_map = 2x4
#include "kid.bmp"

fairy_gfx with bm_map = 1x2
#include "fairy.bmp"

greatfairy_gfx
#include "greatfairy.bmp"

kaepora_gfx with bm_map = 1x3
#include "kaepora.bmp"
kaepora_blink_gfx = kaepora_gfx2
kaepora_turn_gfx = kaepora_gfx3

kaepora_fly_gfx
#include "kaepora fly.bmp"

gardener_gfx with bm_map = 1x2
#include "gardener.bmp"

witch_gfx with bm_map = 1x2
#include "witch.bmp"


;; ------------------------------------ ;;
;; Effects

spark_gfx
#include "spark.bmp"

dizzy_gfx with bm_map = 1x3
#include "dizzy.bmp"


;; ------------------------------------ ;;
;; Titlescreen images
title_sword_gfx:
#include "title sword.bmp"

title_gfx with bm_map = 2x1, bm_pad = 0
#include "title.bmp"
title_left_gfx = title_gfx1
title_right_gfx = title_gfx2

title_zcross_gfx
#include "title zcross.bmp"

#undef with