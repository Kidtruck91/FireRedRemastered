OrangeIslands_Route_10_MapScripts::
	.byte 0

OrangeIslands_Route_10_EventScript_West::
	msgbox OrangeIslands_Route_10_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_10_EventScript_Debris::
	msgbox OrangeIslands_Route_10_Text_Debris, MSGBOX_SIGN
	end

@ Sea warp: different tilesets, so no seamless edge
OrangeIslands_Route_10_EventScript_SailToNavelBeach::
	warp MAP_ORANGE_ISLANDS_NAVEL_BEACH, 86, 5
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 10
	goto_if_lt OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8005, 57
	goto_if_gt OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth_Go:
	addvar VAR_0x8005, 4
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8005, 10
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8005, 57
	goto OrangeIslands_Route_10_EventScript_SeaEdgeToMandarinSouth_Go
