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

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Kinnow
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 6
	goto_if_lt OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Lo
	compare VAR_0x8005, 39
	goto_if_gt OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Hi
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go:
	addvar VAR_0x8005, 10
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_KINNOW, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Lo:
	setvar VAR_0x8005, 6
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go
OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Hi:
	setvar VAR_0x8005, 39
	goto OrangeIslands_Route_8_EventScript_SeaEdgeToKinnow_Go
