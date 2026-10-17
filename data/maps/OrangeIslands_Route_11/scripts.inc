OrangeIslands_Route_11_MapScripts::
	.byte 0

OrangeIslands_Route_11_EventScript_Reef::
	msgbox OrangeIslands_Route_11_Text_Reef, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Trovita Beach
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Lo
	compare VAR_0x8004, 64
	goto_if_gt OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Hi
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go:
	addvar VAR_0x8004, 7
	setvar VAR_0x8005, 34
	warp MAP_ORANGE_ISLANDS_TROVITA_BEACH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go
OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Hi:
	setvar VAR_0x8004, 64
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToTrovitaBeach_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Navel Beach
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 44
	goto_if_lt OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Lo
	compare VAR_0x8005, 51
	goto_if_gt OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Hi
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Go:
	subvar VAR_0x8005, 42
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_NAVEL_BEACH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Lo:
	setvar VAR_0x8005, 44
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Go
OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Hi:
	setvar VAR_0x8005, 51
	goto OrangeIslands_Route_11_EventScript_SeaEdgeToNavelBeach_Go
