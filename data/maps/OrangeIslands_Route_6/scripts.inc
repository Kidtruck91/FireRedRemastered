OrangeIslands_Route_6_MapScripts::
	.byte 0

OrangeIslands_Route_6_EventScript_South::
	msgbox OrangeIslands_Route_6_Text_South, MSGBOX_SIGN
	end

OrangeIslands_Route_6_EventScript_East::
	msgbox OrangeIslands_Route_6_Text_East, MSGBOX_SIGN
	end

OrangeIslands_Route_6_EventScript_Interior::
	msgbox OrangeIslands_Route_6_Text_Interior, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Route_6_EventScript_SailToMikan::
	warp MAP_ORANGE_ISLANDS_MIKAN, 1, 15
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Sunburst
OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst_Lo
	compare VAR_0x8004, 67
	goto_if_gt OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst_Hi
OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst_Go:
	subvar VAR_0x8004, 12
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_SUNBURST, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst_Go
OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst_Hi:
	setvar VAR_0x8004, 67
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToSunburst_Go
