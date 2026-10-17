OrangeIslands_Route_13_MapScripts::
	.byte 0

OrangeIslands_Route_13_EventScript_Reserve::
	msgbox OrangeIslands_Route_13_Text_Reserve, MSGBOX_SIGN
	end

OrangeIslands_Route_13_EventScript_South::
	msgbox OrangeIslands_Route_13_Text_South, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 18
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Lo
	compare VAR_0x8004, 87
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Go:
	addvar VAR_0x8004, 4
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_18, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Hi:
	setvar VAR_0x8004, 87
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute18_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kumquat
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Lo
	compare VAR_0x8004, 71
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go:
	setvar VAR_0x8005, 58
	warp MAP_ORANGE_ISLANDS_KUMQUAT, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Hi:
	setvar VAR_0x8004, 71
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToKumquat_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 16
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Lo
	compare VAR_0x8005, 79
	goto_if_gt OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Hi
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Go:
	addvar VAR_0x8005, 12
	setvar VAR_0x8004, 34
	warp MAP_ORANGE_ISLANDS_ROUTE_16, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Go
OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Hi:
	setvar VAR_0x8005, 79
	goto OrangeIslands_Route_13_EventScript_SeaEdgeToRoute16_Go
