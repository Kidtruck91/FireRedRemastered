OrangeIslands_Kumquat_Beach_MapScripts::
	.byte 0

@ Sea edge: the west edge crosses to Kumquat's east coast. See the matching script on
@ Kumquat for why this is a trigger and not a map connection.
@ This edge is ocean on rows 0-15, land on 16-37 and ocean on 38-52; Kumquat's east edge
@ is ocean 0-23, sand 24-44, ocean 45-59. Land rows carry across row for row, eight
@ south. Rows 17-19 are the rocks and cannot be stood on, so they never fire. Row 37
@ would land on row 45, which is already water on Kumquat, so it is held at 44.
@ The landing column is 63: the rocks on Kumquat's shore run diagonally through x=60-64,
@ and 63 is the only one of those clear of every row this can arrive on.
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 16
	goto_if_lt OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_North
	compare VAR_0x8005, 37
	goto_if_gt OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_South
	compare VAR_0x8005, 37
	goto_if_eq OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_LandEnd
@ sand: rows 16-36 -> Kumquat's east shore, rows 24-44
	addvar VAR_0x8005, 8
	setvar VAR_0x8004, 63
	goto OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_LandEnd:
	setvar VAR_0x8004, 63
	setvar VAR_0x8005, 44
	goto OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_North:
	setvar VAR_0x8004, 63
	setvar VAR_0x8005, 12
	goto OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_South:
	setvar VAR_0x8004, 63
	setvar VAR_0x8005, 52
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_Go:
	warp MAP_ORANGE_ISLANDS_KUMQUAT, VAR_0x8004, VAR_0x8005
	waitstate
	end

OrangeIslands_Kumquat_Beach_EventScript_Town::
	msgbox OrangeIslands_Kumquat_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Kumquat_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Kumquat_Beach_Text_Harbor, MSGBOX_SIGN
	end
