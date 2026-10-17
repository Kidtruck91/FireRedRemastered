OrangeIslands_Kumquat_MapScripts::
	.byte 0

OrangeIslands_Kumquat_EventScript_Beach::
	msgbox OrangeIslands_Kumquat_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Kumquat_EventScript_Town::
	msgbox OrangeIslands_Kumquat_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 13
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 12
	goto_if_lt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8004, 59
	goto_if_gt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go:
	addvar VAR_0x8004, 8
	setvar VAR_0x8005, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8004, 12
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8004, 59
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 14
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 6
	goto_if_lt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Lo
	compare VAR_0x8005, 49
	goto_if_gt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Hi
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go:
	addvar VAR_0x8005, 6
	setvar VAR_0x8004, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_14, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Lo:
	setvar VAR_0x8005, 6
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Hi:
	setvar VAR_0x8005, 49
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go
