
; MirageOS Development Include File
;
; Ver. 1.0 - Last Updated: 07/15/00
; By Dan Englender and Jason Kovacs

.nolist
#define bcall(label) RST 28h \ .dw label
#define bjump(label) call 50h \ .dw label
#define errhandon(label) ld hl,label \ call 59h
#define errhandoff() call 5Ch

;=================================;
;======== Ion Subroutines ========;
;=================================;

iversion              = $4083
irandom               = $4086
isprite               = $4089
ilsprite              = $408C
igetpix               = $408F
ifastcopy             = $4092
idetect               = $4095
idecomp               = $4098

;=================================;
;====== MirageOS Subroutines =====;
;=================================;

directin              = $409B
sendbytetios          = $409E
getbytetios           = $40A1
version               = $40A4
setvputs              = $40A7
setpixel              = $40AA
fastcopys             = $40AD
delayb                = $40B0
multhe                = $40B3
multhl                = $40B6
quittoshell           = $40B9
fastline              = $40BC
pixelonhl             = $40BF
pixeloff              = $40C2
pixelxor              = $40C5
pixeltest             = $40C8
pixeloffhl            = $40CB
pixelxorhl            = $40DE
pixeltesthl           = $40D1
fastlineb             = $40D4
fastlinew             = $40D7
fastlinex             = $40DA
pointonc              = $40DD
pointoffc             = $40E0
pointxorc             = $40E3
centertext            = $40E6
cphlbc                = $40E9
putsprite8            = $40EC
fastcopyb             = $40EF
vputsc                = $40F2
scrolld7              = $40F5
vnewline              = $40F8
rand127               = $40FB
disprle               = $40FE
cphlde                = $4101
screentopic           = $4104
fastlined             = $4107
disprlel              = $410A
getnextgoodprog       = $410D
getprevgoodprog       = $4110
getnext               = $4113
getprev               = $4116
compstrs              = $4119
nextstr               = $411C
getinfo               = $411F
fastrectangle         = $4122
gettext               = $4125
gettextv              = $4128
FastRectangle_Save    = $412B
vputa                 = $412E
runprog               = $4131
isgoodprog            = $4134
existfold             = $4137
delprog               = $413A
filledrectangle       = $413D
nextfolder            = $4140
delfolder             = $4143
moveall               = $4149
curfoldname           = $4149
curfoldnamea          = $414C
createfolder          = $414F
compstrsn             = $4152
folder_menu_start     = $4155
options_screen        = $4158
put_folder_name_top   = $415B
general_key_routine   = $415E
find_num_good_progs   = $4161
put_scrollbar         = $4164
invert_lines          = $4167
invert_1_line         = $416A
right_align_value     = $416D
put_mirageos_header   = $4170
put_size_graphic      = $4173
sendprog              = $4176
hideprog              = $4179
arcprog               = $417C
filledrectangle_save  = $417F
getbytetiosw          = $4182
vatswap               = $4185
renameprog            = $4188
renamefolder          = $418B
sysmain               = $418E
setupint              = $4191
move_gui_prog         = $4194
largespritehl         = $4197
Update_Scrollbar      = $419A
Initial_Scrollbar     = $419D
sortallfolds          = $41A0
dofoldsort            = $41A3
getfoldsort           = $41A6
setfoldsort           = $41A9
Increase_Cur_Element  = $41AC
Decrease_Cur_Element  = $41AF
Increase_Max_Elements = $41B2
Decrease_Max_Elements = $41B5
Add_A_To_Cur_Element  = $41B8
Sub_A_From_Cur_Element = $41BB
Add_A_To_Max_Elements = $41BE
Sub_A_From_Max_Elements = $41C1
Skip_Forward_B_From_Top = $41C4
Get_Curgoodprog_Ptr   = $41C7
getchecksum           = $41CA
freearc               = $41CD
swapram               = $41D0
hideall               = $41D3

;=================================;
;====== MirageOS Ram Equates =====;
;=================================;

mlinebitmap           = $FFFF-399
timer1                = $8a3a
timer1max             = $8a3b
timer2                = $8a3c
timer2max             = $8a3d
timer3                = $8a3e
custintaddr           = $966F

saferam1              = SavesScreen
saferam2              = AppBackUpScreen
saferam3              = TempSwapArea

mirflags              = $21
rightalign            = $07
lcdinstport           = $10
lcddataport           = $11

numscrollpixels       = CmdShadow+104
scrollbarcoords       = CmdShadow+105
old_element           = CmdShadow+107
cur_element           = CmdShadow+109
old_max_elem          = CmdShadow+111
max_elements          = CmdShadow+113

; =================================
; ===== Various Other Equates =====
; =================================

gbuf                  = PlotsScreen
LargeSprite           = ilsprite
SmallSprite           = isprite
FastCopy              = ifastcopy

.list
.end
