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
	compare VAR_0x8005, 16
	goto_if_lt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Lo
	compare VAR_0x8005, 49
	goto_if_gt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Hi
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go:
	subvar VAR_0x8005, 10
	setvar VAR_0x8004, 129
	warp MAP_ORANGE_ISLANDS_ROUTE_8, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Lo:
	setvar VAR_0x8005, 16
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Hi:
	setvar VAR_0x8005, 49
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute8_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 11
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 16
	goto_if_lt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11_Lo
	compare VAR_0x8004, 55
	goto_if_gt OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11_Hi
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11_Go:
	addvar VAR_0x8004, 4
	setvar VAR_0x8005, 77
	warp MAP_ORANGE_ISLANDS_ROUTE_11, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11_Lo:
	setvar VAR_0x8004, 16
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11_Go
OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11_Hi:
	setvar VAR_0x8004, 55
	goto OrangeIslands_Kinnow_EventScript_SeaEdgeToRoute11_Go
