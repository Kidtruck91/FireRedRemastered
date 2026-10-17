OrangeIslands_Route_8_MapScripts::
	.byte 0

OrangeIslands_Route_8_EventScript_West::
	msgbox OrangeIslands_Route_8_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_8_EventScript_East::
	msgbox OrangeIslands_Route_8_Text_East, MSGBOX_SIGN
	end

OrangeIslands_Route_8_EventScript_Rest::
	msgbox OrangeIslands_Route_8_Text_Rest, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 36
	goto_if_lt OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8004, 131
	goto_if_gt OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Go:
	subvar VAR_0x8004, 36
	setvar VAR_0x8005, 74
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8004, 36
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8004, 131
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToMandarinSouth_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Lo
	compare VAR_0x8005, 19
	goto_if_gt OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Hi
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go:
	addvar VAR_0x8005, 46
	setvar VAR_0x8004, 70
	warp MAP_ORANGE_ISLANDS_KINNOW, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Hi:
	setvar VAR_0x8005, 19
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go
