;; Assembly export for Swordless Dungeon

#ifdef MAP_EDITOR_DATA
test_map:
	.db $02,$FF,$FF,$FF,$02,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$02,$FF,$02,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$02,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$02,$FF,$02,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $02,$FF,$FF,$FF,$02,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
	.db $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
#endif
#if pageof($)=5
test_compressed_map:
dm2:
	.db $00,$8F,$F4,$05,$00,$40,$50,$44,$15,$02,$3F,$D0,$34,$19,$0A,$42
	.db $10,$B4,$2D,$00,$42,$50,$D4,$31,$07,$44,$51,$44,$55,$16,$45,$D1
	.db $84,$65,$1A,$46,$D1,$C4,$75,$1E,$45,$BF,$F0

#endif
#if pageof($)=3

test_defaults:
animate_section()
object_section()
enemy_section()
misc_section()
end_section()

#endif
#ifdef MAP_EDITOR_DATA
pete2_map:
	.db $02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$9E,$CF,$9B,$CF,$9F,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$99,$00,$07,$00,$9A,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$99,$0B,$00,$0A,$9A,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$9C,$BD,$27,$BE,$9D,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$D7,$AF,$2F,$AF,$AE,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$9E,$C5,$24,$C6,$9F,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$99,$AC,$00,$AC,$9A,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
	.db $02,$99,$06,$06,$06,$A0,$9B,$CF,$9B,$9B,$CF,$9B,$9B,$CF,$9B,$9F
	.db $02,$99,$01,$01,$01,$01,$05,$02,$02,$02,$02,$02,$04,$01,$01,$9A
	.db $02,$9C,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$BD,$27,$BE,$9D
#endif
#if pageof($)=5
pete2_compressed_map:
dm3:
	.db $00,$90,$04,$05,$02,$40,$D0,$44,$15,$06,$41,$D0,$84,$25,$0A,$42
	.db $D0,$42,$78,$CF,$26,$CC,$F2,$7D,$09,$26,$40,$00,$1C,$00,$26,$91
	.db $30,$2C,$00,$02,$91,$84,$20,$9C,$2F,$42,$72,$F8,$9D,$42,$4D,$72
	.db $BC,$2F,$2B,$CA,$E4,$24,$9E,$31,$42,$43,$19,$12,$42,$09,$92,$B0
	.db $00,$2B,$11,$D4,$1C,$99,$01,$93,$82,$81,$10,$26,$D3,$B4,$ED,$12
	.db $26,$40,$15,$04,$01,$01,$50,$30,$11,$41,$46,$09,$C2,$61,$4A,$52
	.db $D4,$C4,$81,$22,$FF,$C0

#endif
#if pageof($)=3

pete2_defaults:
animate_section()
	upper_torch(32,96)
	upper_torch(64,96)
	upper_torch(208,208)
	upper_torch(160,208)
	upper_torch(112,208)
object_section()
	doortoph(44,176)
enemy_section()
	platform(176,224,d_left,0)
misc_section()
	hole_replace(112, 224, 80, 16, 200, 224)
	extend_enemy_screen(16, 96, 240, 160)
end_section()

#endif
#ifdef MAP_EDITOR_DATA
pete1_map:
	.db $92,$8F,$8F,$8F,$8F,$8F,$8F,$8F,$93,$9A,$DD,$9E,$C5,$AB,$C6,$9F
	.db $8D,$00,$00,$00,$00,$00,$00,$00,$8E,$9A,$AD,$99,$01,$03,$01,$9A
	.db $8D,$00,$02,$96,$8C,$8C,$B1,$5A,$B3,$9A,$AD,$99,$05,$00,$04,$9A
	.db $D1,$00,$00,$D3,$0A,$06,$06,$01,$AC,$C2,$AD,$C0,$05,$00,$04,$9A
	.db $90,$B1,$27,$B3,$04,$AC,$01,$01,$01,$26,$2D,$25,$01,$06,$01,$9A
	.db $AF,$AF,$2F,$AE,$09,$01,$AC,$01,$01,$C3,$CB,$9C,$98,$98,$98,$9D
	.db $92,$B9,$24,$BA,$93,$01,$03,$03,$03,$A0,$9B,$9B,$9B,$9B,$9B,$9B
	.db $8D,$00,$00,$AC,$8E,$01,$B8,$48,$BA,$8F,$8F,$8F,$8F,$8F,$8F,$93
	.db $D1,$00,$00,$AC,$D3,$4C,$D1,$00,$00,$00,$02,$00,$00,$00,$00,$D3
	.db $8D,$AC,$00,$00,$8E,$4C,$90,$97,$02,$00,$00,$00,$96,$8C,$8C,$91
	.db $8D,$00,$00,$00,$8E,$4C,$AC,$B0,$5A,$B2,$8C,$8C,$91,$0A,$06,$A2
	.db $8D,$00,$00,$00,$8E,$04,$AC,$58,$58,$58,$06,$06,$06,$01,$01,$9A
	.db $90,$B1,$5A,$B2,$91,$4C,$B8,$48,$BA,$93,$04,$01,$01,$01,$01,$9A
	.db $A3,$06,$06,$A2,$A3,$4C,$8D,$00,$96,$91,$04,$01,$01,$01,$01,$9A
	.db $99,$01,$01,$9A,$99,$4C,$8D,$00,$8E,$0A,$01,$01,$01,$01,$01,$9A
	.db $9C,$BD,$27,$BF,$99,$4C,$B0,$27,$B3,$04,$A2,$98,$98,$98,$98,$9D
#endif
#if pageof($)=5
pete1_compressed_map:
dm1:
	.db $24,$88,$F4,$05,$02,$23,$C9,$32,$68,$DD,$27,$8C,$52,$AC,$C6,$27
	.db $C8,$D0,$01,$0E,$43,$C0,$02,$38,$9A,$2B,$49,$90,$04,$03,$00,$49
	.db $A4,$34,$02,$25,$88,$C2,$30,$B1,$16,$8B,$34,$4C,$99,$01,$40,$00
	.db $10,$9A,$34,$50,$E3,$4C,$0A,$01,$80,$60,$04,$AC,$30,$8A,$D3,$01
	.db $24,$49,$89,$02,$C4,$27,$2C,$C0,$42,$B0,$01,$4E,$C2,$60,$B4,$25
	.db $00,$52,$D2,$68,$AF,$2B,$C2,$F2,$B8,$09,$4B,$93,$B3,$0C,$CB,$27
	.db $09,$85,$34,$9D,$24,$8B,$90,$90,$BA,$24,$D1,$60,$0C,$03,$28,$09
	.db $B5,$65,$5A,$43,$40,$02,$B0,$8E,$00,$4B,$81,$20,$BA,$40,$D0,$12
	.db $4D,$28,$57,$4D,$31,$31,$67,$00,$00,$24,$40,$D3,$23,$4A,$C4,$38
	.db $8E,$13,$09,$02,$5D,$6D,$43,$91,$C2,$30,$91,$57,$11,$11,$30,$AC
	.db $2C,$05,$A2,$C9,$1D,$24,$52,$B2,$89,$7C,$23,$93,$91,$61,$8A,$4B
	.db $12,$D4,$61,$35,$60,$49,$11,$31,$61,$54,$C0,$44,$ED,$3B,$26,$8A
	.db $34,$B0,$A2,$28,$C4,$C4,$34,$96,$24,$59,$56,$58,$9A,$45,$51,$82
	.db $65,$9D,$44,$40,$A6,$59,$97,$27,$0B,$D0,$9C,$BF,$69,$8B,$04,$DC
	.db $04,$28,$94,$D5,$3B,$FF

#endif
#if pageof($)=3

pete1_defaults:
animate_section()
	lower_left_torch(0,128)
	lower_right_torch(64,128)
	lower_left_torch(96,128)
	lower_right_torch(240,128)
	lower_left_torch(0,48)
	lower_right_torch(48,48)
object_section()
	button(231,132,kPeteButton3)
	button(231,68,kPeteButton4)
	generic_object(192,192,0,16,16,16,0,o_noaccess | o_touchable,0,chest_gfx)
	general_block(32,160)
	general_block(48,160)
	general_block(112,48)
	free_block(112,96)
	general_block(80,96)
	free_block(112,80)
	general_block(208,32)
	locked_door(112,240)
	doortopv(156,78)
	doortoph(28,96)
	generic_object(224,176,0,12,12,16,kGeneralBlock,1,0,pot_gfx)
	generic_object(224,208,0,12,12,16,kGeneralBlock,1,0,pot_gfx)
	generic_object(176,208,0,12,12,16,kGeneralBlock,1,0,pot_gfx)
	generic_object(226,65,0,12,12,12,kNo_action,o_noaccess | 1,0,pot_gfx)
enemy_section()
	sliding_trap(164,196,d_right)
	wall_trap(224,34,d_down)
misc_section()
	hole(160, 128, 16, 16)
	hole(128, 144, 16, 16)
	hole(32, 32, 16, 16)
	extend_enemy_screen(112, 160, 144, 96)
	generic_misc(160, 168, 80, 80, moderate_pete_pots)
end_section()

#endif
#ifdef MAP_EDITOR_DATA
pete0_map:
	.db $9E,$C5,$24,$C7,$99,$4C,$B8,$24,$BB,$09,$9A,$9E,$9B,$9B,$9B,$9F
	.db $99,$01,$01,$9A,$99,$4C,$B4,$00,$94,$93,$9A,$99,$01,$01,$01,$9A
	.db $A1,$03,$03,$A0,$A1,$4C,$4E,$00,$00,$8E,$9A,$99,$01,$01,$01,$9A
	.db $92,$B9,$48,$BA,$93,$07,$B5,$00,$AC,$8E,$A0,$A1,$03,$03,$03,$A0
	.db $8D,$00,$00,$00,$8E,$DD,$8D,$00,$00,$94,$93,$DD,$B8,$48,$BA,$93
	.db $90,$8C,$B1,$A8,$B3,$CB,$90,$97,$0A,$06,$8E,$AD,$8D,$00,$00,$8E
	.db $D7,$AF,$AF,$2F,$AF,$AF,$AE,$90,$8C,$8C,$91,$CB,$90,$97,$00,$8E
	.db $92,$8F,$B9,$AB,$BA,$93,$DD,$D7,$AF,$AF,$AF,$AE,$DD,$8D,$00,$8E
	.db $8D,$00,$00,$00,$00,$8E,$AD,$92,$8F,$8F,$8F,$93,$AD,$8D,$02,$8E
	.db $D1,$00,$0A,$0B,$00,$D3,$AD,$D1,$AC,$00,$00,$8E,$CB,$8D,$02,$D3
	.db $8D,$00,$04,$05,$00,$B6,$AD,$B4,$08,$00,$00,$94,$8F,$95,$00,$8E
	.db $8D,$00,$09,$08,$00,$AA,$2D,$A9,$00,$0A,$0B,$00,$00,$00,$00,$8E
	.db $8D,$00,$00,$00,$00,$B7,$AD,$B5,$00,$AC,$AC,$96,$8C,$97,$00,$8E
	.db $8D,$00,$96,$8C,$8C,$91,$CB,$90,$8C,$8C,$8C,$91,$DD,$8D,$00,$8E
	.db $8D,$00,$8E,$A2,$98,$98,$98,$98,$98,$98,$98,$A3,$AD,$8D,$00,$8E
	.db $90,$8C,$91,$99,$D7,$AF,$AF,$AF,$AF,$AF,$AE,$99,$AD,$B0,$A7,$B3
#endif
#if pageof($)=5
pete0_compressed_map:
dm0:
	.db $27,$8C,$50,$90,$C7,$26,$44,$C2,$E0,$24,$2E,$C0,$92,$68,$9E,$26
	.db $D0,$C2,$7C,$99,$00,$40,$12,$69,$04,$2D,$00,$02,$50,$93,$44,$91
	.db $04,$44,$A1,$00,$C0,$32,$80,$A1,$13,$04,$E0,$00,$00,$23,$91,$84
	.db $40,$9A,$24,$8B,$91,$20,$BA,$24,$C0,$72,$D4,$00,$2B,$08,$E4,$79
	.db $1C,$47,$48,$D4,$89,$23,$37,$53,$54,$54,$93,$37,$4B,$84,$A8,$93
	.db $24,$08,$C2,$C4,$A8,$2C,$CC,$B2,$40,$97,$02,$80,$62,$38,$AD,$4E
	.db $48,$E3,$5C,$AF,$2B,$C2,$F5,$3C,$AE,$50,$08,$C2,$45,$45,$25,$D2
	.db $32,$48,$8F,$2E,$4A,$B4,$AC,$DD,$53,$95,$24,$E1,$23,$4E,$52,$25
	.db $29,$5A,$23,$C8,$F2,$4D,$4B,$00,$88,$E3,$44,$00,$02,$80,$B0,$00
	.db $D3,$2B,$4D,$12,$B1,$65,$32,$C8,$D0,$08,$D3,$4D,$40,$40,$14,$00
	.db $2D,$8A,$D2,$D0,$08,$48,$89,$42,$3C,$95,$58,$C0,$00,$25,$83,$2A
	.db $82,$D2,$A5,$6F,$5C,$53,$62,$39,$64,$00,$0B,$72,$B5,$2E,$2B,$0A
	.db $C2,$58,$8C,$56,$19,$20,$01,$9A,$55,$54,$52,$31,$A2,$24,$56,$26
	.db $74,$8E,$28,$89,$86,$A5,$AA,$26,$0A,$35,$2D,$23,$50,$09,$12,$65
	.db $60,$53,$CA,$F2,$B8,$99,$2B,$4B,$02,$9C,$B3,$FF,$C0

#endif
#if pageof($)=3

pete0_defaults:
animate_section()
	lower_right_torch(80,144)
	lower_left_torch(0,144)
	lower_left_torch(112,144)
	lower_right_torch(240,144)
object_section()
	general_block(144,160)
	general_block(160,160)
	general_block(128,176)
	button(231,20,kPeteButton2)
	button(199,20,kPeteButton1)
	pot(66,160,12,12)
	sign(213,19,kSign2)
	doortoph(44,112)
	doortopv(92,190)
	general_block(112,32)
	heart_container(150,77)
enemy_section()
misc_section()
	generic_misc(120, 136, 80, 80, moderate_threeblock)
	hole(224, 128, 16, 32)
end_section()

#endif
