OrangeIslands_Route_7_MapScripts::
	.byte 0

OrangeIslands_Route_7_EventScript_Moro::
	msgbox OrangeIslands_Route_7_Text_Moro, MSGBOX_SIGN
	end

OrangeIslands_Route_7_EventScript_South::
	msgbox OrangeIslands_Route_7_Text_South, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Sunburst
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 21
	goto_if_lt OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Lo
	compare VAR_0x8005, 82
	goto_if_gt OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Hi
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go:
	subvar VAR_0x8005, 20
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_SUNBURST, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Lo:
	setvar VAR_0x8005, 21
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Hi:
	setvar VAR_0x8005, 82
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Moro
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Lo
	compare VAR_0x8004, 59
	goto_if_gt OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Hi
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Go:
	subvar VAR_0x8004, 4
	setvar VAR_0x8005, 54
	warp MAP_ORANGE_ISLANDS_MORO, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Go
OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Hi:
	setvar VAR_0x8004, 59
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToMoro_Go

@ Sea corner: choose the crossed edge from the incoming direction
OrangeIslands_Route_7_EventScript_SeaCorner_71_0::
	specialvar VAR_RESULT, GetPlayerFacingDirection
	compare VAR_RESULT, DIR_EAST
	goto_if_eq OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst
	compare VAR_RESULT, DIR_NORTH
	goto_if_eq OrangeIslands_Route_7_EventScript_SeaEdgeToMoro
	end
