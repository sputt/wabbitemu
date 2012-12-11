;BEGIN OBJECT CODE TABLES
#define script(lab) lab | $8000
#macro objz(name, zcode) 
  #define newname concat("\"",name,"\"")
  #define newzcode concat("\"",zcode,"\"")
  wr(newname, " = ($-oct)/2")
  wr(".dw ", newzcode)
#endmacro
#define .obj objz(
clr()
oct:
.dw 0

.obj kRupee,		pick_up_object
.obj kHeart,		pick_up_object
.obj kKey,		pick_up_object
.obj kHeart_container,	get_heart_container
.obj kBow_obtain,	get_bow
.obj kShield_obtain,	get_shield

.obj kPush_object1,	push_object
.obj kPush_object2,	push_object
.obj kFreeBlock,	push_object
.obj kPush_object4,	push_object
.obj kEyeBlockDown,	d1_2_eyeblock_down
.obj kEyeBlockRight,	d1_2_eyeblock_right
.obj kMemoryBlock,	push_memory_object

; Objects before this are checked for collisions
#define collidable_count kDestroy

.obj kDestroy,		destroy_object
.obj kSword,		get_sword

.obj kLocked_door,	unlock_door1
.obj kBoss_door,	unlock_boss_door

.obj kNo_action,	no_action2

.obj kSwitch1,		script(d1_0_cover_hole_script)
.obj kSwitch2,		script(d1_0_unlock_door_script)
.obj kSwitch3,		script(d1_1_remove_block_script)
.obj kSwitch4,		script(d1_5_stop_traps_script)

.obj kButton1,		script(d1_1_cover_upper_hole_script)
.obj kButton2,		script(d1_4_remove_block_script)
.obj kButton3,		script(d1_5_cover_hole_script)

.obj kButton4Up,	script(d1_5_close_bottom_doors_script)
.obj kButton4Down,	script(d1_5_close_top_doors_script)

.obj kPeteButton1,	script(p_0_turn_on_trap_script)
.obj kPeteButton2,	script(p_0_open_door_script)
.obj kPeteButton3,	script(p_1_cover_stair_hole_script)
.obj kPeteButton4,	script(p_1_open_door_script)

.obj kJimButton1, 	script(j_1_platform_script)
.obj kJimButton2,	script(j_4_traps_script)

.obj kEye1,		script(d1_1_cover_big_hole_script)
.obj kEye2,		script(d1_1_shoot_over_chest_script)
.obj kEye3,		script(d1_2_stop_traps_script)
.obj kEye4,		create_path
.obj kEye5,		script(d1_4_disable_close_script)

.obj kSign1,		script(d1_5_sign_script)
.obj kSign2,		script(p_0_sign_script)
.obj kSign3,		script(forest4_sign_script)

.obj kChestKey,		open_chest1
.obj kChestBow,		open_chest_bow
.obj kChestShield,	open_chest_shield
.obj kChestRupee,	open_chest3
.obj kChestPetePots,	open_chest_pete
.obj kChestBossKey,	open_chest_boss_key

.obj kMoving_block,	monitor_blockmove
.obj kLockscreen,	lockscreen_house

.obj kNpc_talk,		npc_talk
.obj kNpc_talk2,	npc_talk

.obj kMarin_start,	script(marin_intro_script)
.obj kWitch,		script(witch_house_script)
.obj kKaepora,		0

.obj kBuy_heart_container, script(town_shop_heart_container_script)
.obj kBuy_bomb,		script(town_shop_bomb_script)
.obj kBuy_heart,	script(town_shop_heart_script)

run()