#ifndef _EDEF_INC
#define _EDEF_INC

.echo "processing edef.inc"

#DEFINE enemy_health		  	-2
#DEFINE enemy_flags		  	-1
#DEFINE enemy_x			  	0
#DEFINE enemy_xc		  	enemy_x
#DEFINE enemy_w			  	1
#DEFINE enemy_y			  	2
#DEFINE enemy_yc		  	enemy_y
#DEFINE enemy_h			  	3
#define enemy_z				4
#define enemy_d				5
#DEFINE enemy_gen		  	6
#DEFINE enemy_g			  	enemy_gen
#DEFINE enemy_gen_state		  	enemy_gen
#DEFINE enemy_com_ctr		  	7
#DEFINE enemy_com_ptr		  	8
#DEFINE enemy_anim_ctr		  	10
#DEFINE enemy_anim_ptr		  	11

#DEFINE ef_invunerable	  7
#DEFINE e_invunerable	  (1<<ef_invunerable)
#define ef_invulnerable	 	7
#DEFINE e_invulnerable	  (1<<ef_invulnerable)

#DEFINE ef_invincible	  ef_invunerable
#DEFINE e_invincible	  e_invunerable
#DEFINE ef_inverted	  6
#DEFINE e_inverted	  (1<<ef_inverted)
#DEFINE ef_invert	  ef_inverted
#DEFINE e_invert	  e_inverted
#DEFINE ef_animation	  5
#DEFINE ef_anim		  ef_animation
#DEFINE e_4dir		  (1<<ef_animation)
#DEFINE e_nodir		  0
#DEFINE e_animation	  (1<<ef_anim)
#DEFINE e_anim		  e_animation
#DEFINE ef_noharm	  4
#DEFINE e_noharm	  (1<<ef_noharm)
#DEFINE et_mask		  %00001111

#define e_passive (e_noharm | e_invincible)

#DEFINE et_shot		  0
#DEFINE et_octo		  1
#DEFINE et_octorok	  1
#DEFINE et_fairy	1
#DEFINE octo_h		  10
#DEFINE octo_w		  10
#DEFINE octorok_h	  16
#DEFINE octorok_w	  16
#DEFINE et_leever	  2
#DEFINE et_burrower	  2
#DEFINE et_big_chu	  3
#define et_chu		  3
#define et_greatfairy		3
#DEFINE et_bigchu	  3
#DEFINE big_chu_h	  8
#define chu_h		  8
#DEFINE big_chu_w	  15
#define chu_w		  15
#DEFINE et_small_chu	  4
#DEFINE et_smallchu	  4
#DEFINE small_chu_h	  5
#DEFINE small_chu_w	  9
#DEFINE et_spider	  5
#DEFINE et_bat		  6
#DEFINE bat_h		  7
#DEFINE bat_w		  7
#DEFINE et_miniboss1	  7
#DEFINE et_mini_boss1	  7
#DEFINE miniboss1_h	  20
#DEFINE miniboss1_w	  32
#DEFINE et_horz_arrow	8
#DEFINE et_vert_arrow	9
#DEFINE et_zombie	10
#DEFINE zombie_h	16
#DEFINE zombie_w	16
#DEFINE et_mirror_man	11
#DEFINE mirror_man_h	16
#DEFINE mirror_man_w	16
#define et_peahat	12
#define peahat_h	8
#define peahat_w	8
#DEFINE et_ganon	13
#DEFINE ganon_w		32
#DEFINE ganon_h		25

#define et_dizzy_octo	14


#DEFINE eye_w		11
#DEFINE eye_h		16

#DEFINE trap_w		12
#DEFINE trap_h		12
#DEFINE et_trap		5

#DEFINE object_ID			-2
#DEFINE object_flags		  	-1
#DEFINE object_x		  	0
#DEFINE object_xc		  	object_x
#DEFINE object_w		  	1
#DEFINE object_y		  	2
#DEFINE object_yc		  	object_y
#DEFINE object_h		  	3
#define object_z			4
#define object_d			5
#DEFINE object_anim_ctr		 	6
#DEFINE object_anim_ptr		  	7

#DEFINE of_touchable		  7
#DEFINE o_touchable 		  (1<<of_touchable)
#DEFINE of_talkable		  of_touchable
#DEFINE of_active	6			;don't touch
#DEFINE o_active 			(1<<of_active) 
#DEFINE of_noaccess		  5
#DEFINE o_noaccess		  (1<<of_noaccess)
#DEFINE of_destroyable		  4
#DEFINE o_destroyable		  (1<<of_destroyable)
#DEFINE of_destroy		  of_destroyable
#define of_sparkle	3
#define o_sparkle	(1<<of_sparkle)

#DEFINE msc_alive		-1
#define misc_alive		-1
#DEFINE msc_x			0
#DEFINE msc_w			1
#DEFINE msc_y			2
#DEFINE msc_h			3
#DEFINE msc_com_ptr		4
#define msc_code		4
#define msc_aux			6

#include "structures.inc"

#endif
