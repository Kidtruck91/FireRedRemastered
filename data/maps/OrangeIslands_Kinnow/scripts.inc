OrangeIslands_Kinnow_MapScripts::
	.byte 0

OrangeIslands_Kinnow_EventScript_Beach::
	msgbox OrangeIslands_Kinnow_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Kinnow_EventScript_Town::
	msgbox OrangeIslands_Kinnow_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 8
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 47
	goto_if_lt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Lo
	compare VAR_0x8005, 64
	goto_if_gt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Hi
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go:
	subvar VAR_0x8005, 46
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_8, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Lo:
	setvar VAR_0x8005, 47
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Hi:
	setvar VAR_0x8005, 64
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 10
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Lo
	compare VAR_0x8005, 7
	goto_if_gt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Hi
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Go:
	addvar VAR_0x8005, 50
	setvar VAR_0x8004, 102
	warp MAP_ORANGE_ISLANDS_ROUTE_10, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Go
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Hi:
	setvar VAR_0x8005, 7
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute10_Go
