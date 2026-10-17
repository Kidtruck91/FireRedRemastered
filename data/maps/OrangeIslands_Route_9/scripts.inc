OrangeIslands_Route_9_MapScripts::
	.byte 0

OrangeIslands_Route_9_EventScript_South::
	msgbox OrangeIslands_Route_9_Text_South, MSGBOX_SIGN
	end

OrangeIslands_Route_9_EventScript_Platform::
	msgbox OrangeIslands_Route_9_Text_Platform, MSGBOX_SIGN
	end

@ Sea warp: different tilesets, so no seamless edge
OrangeIslands_Route_9_EventScript_SailToNavelBeach::
	warp MAP_ORANGE_ISLANDS_NAVEL_BEACH, 1, 5
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 8
	goto_if_lt OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8004, 43
	goto_if_gt OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth_Go:
	addvar VAR_0x8004, 22
	setvar VAR_0x8005, 74
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8004, 8
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8004, 43
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToMandarinSouth_Go
