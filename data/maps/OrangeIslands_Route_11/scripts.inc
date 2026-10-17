OrangeIslands_Route_11_MapScripts::
	.byte 0

OrangeIslands_Route_11_EventScript_Reef::
	msgbox OrangeIslands_Route_11_Text_Reef, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow
OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow_Lo
	compare VAR_0x8004, 59
	goto_if_gt OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow_Hi
OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow_Go:
	subvar VAR_0x8004, 4
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_KINNOW, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow_Go
OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow_Hi:
	setvar VAR_0x8004, 59
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToKinnow_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Trovita Beach
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Lo
	compare VAR_0x8004, 59
	goto_if_gt OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Hi
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go:
	subvar VAR_0x8004, 4
	setvar VAR_0x8005, 34
	warp MAP_ORANGE_ISLANDS_TROVITA_BEACH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Hi:
	setvar VAR_0x8004, 59
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go
