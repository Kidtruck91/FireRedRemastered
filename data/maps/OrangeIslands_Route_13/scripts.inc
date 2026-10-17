OrangeIslands_Route_13_MapScripts::
	.byte 0

OrangeIslands_Route_13_EventScript_Reserve::
	msgbox OrangeIslands_Route_13_Text_Reserve, MSGBOX_SIGN
	end

OrangeIslands_Route_13_EventScript_South::
	msgbox OrangeIslands_Route_13_Text_South, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8004, 67
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth_Go:
	addvar VAR_0x8004, 4
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8004, 67
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToMandarinSouth_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kumquat
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Lo
	compare VAR_0x8004, 67
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go:
	subvar VAR_0x8004, 8
	setvar VAR_0x8005, 58
	warp MAP_ORANGE_ISLANDS_KUMQUAT, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Hi:
	setvar VAR_0x8004, 67
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go
