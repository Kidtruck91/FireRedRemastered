OrangeIslands_Route_10_MapScripts::
	.byte 0

OrangeIslands_Route_10_EventScript_West::
	msgbox OrangeIslands_Route_10_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_10_EventScript_Debris::
	msgbox OrangeIslands_Route_10_Text_Debris, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 50
	goto_if_lt OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Lo
	compare VAR_0x8005, 67
	goto_if_gt OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Hi
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Go:
	subvar VAR_0x8005, 50
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_KINNOW, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Lo:
	setvar VAR_0x8005, 50
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Go
OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Hi:
	setvar VAR_0x8005, 67
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToKinnow_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 17
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Lo
	compare VAR_0x8005, 23
	goto_if_gt OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Hi
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Go:
	addvar VAR_0x8005, 32
	setvar VAR_0x8004, 38
	warp MAP_ORANGE_ISLANDS_ROUTE_17, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Go
OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Hi:
	setvar VAR_0x8005, 23
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToRoute17_Go
