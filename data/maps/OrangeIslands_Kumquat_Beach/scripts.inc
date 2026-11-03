OrangeIslands_Kumquat_Beach_MapScripts::
	.byte 0

@ Sea edge: the west shore crosses to Kumquat. See the matching script on Kumquat for
@ why this is a trigger and not a map connection.
@ This edge is sand on rows 16-38 and rock or walled-off ocean everywhere else, and it
@ meets Kumquat's east sand on rows 23-44, so the crossing is seven rows south onto
@ column 64 -- the furthest in that still shows no border on a 72-wide map, and clear
@ land for every row this can arrive on. Row 38 is the one overhang: 38+7 is 45, which
@ is rock on Kumquat, so it is held at 44.
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat::
	getplayerxy VAR_0x8004, VAR_0x8005
	setvar VAR_0x8004, 64
	compare VAR_0x8005, 37
	goto_if_gt OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_Last
	addvar VAR_0x8005, 7
	goto OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat_Last:
	setvar VAR_0x8005, 44
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
