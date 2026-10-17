OrangeIslands_Route_12_MapScripts::
	.byte 0

OrangeIslands_Route_12_EventScript_West::
	msgbox OrangeIslands_Route_12_Text_West, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 4
	goto_if_lt OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8005, 39
	goto_if_gt OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Go:
	addvar VAR_0x8005, 16
	setvar VAR_0x8004, 94
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8005, 4
	goto OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8005, 39
	goto OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Go
