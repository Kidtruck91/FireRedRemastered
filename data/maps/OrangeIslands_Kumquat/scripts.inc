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
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8004, 67
	goto_if_gt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go:
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8004, 67
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute13_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 14
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 4
	goto_if_lt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Lo
	compare VAR_0x8004, 51
	goto_if_gt OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Hi
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go:
	addvar VAR_0x8004, 16
	setvar VAR_0x8005, 70
	warp MAP_ORANGE_ISLANDS_ROUTE_14, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Lo:
	setvar VAR_0x8004, 4
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go
OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Hi:
	setvar VAR_0x8004, 51
	goto OrangeIslands_Kumquat_EventScript_SeaEdgeToRoute14_Go
