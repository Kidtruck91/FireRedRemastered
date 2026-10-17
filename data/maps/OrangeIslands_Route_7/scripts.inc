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
	compare VAR_0x8005, 20
	goto_if_lt OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Lo
	compare VAR_0x8005, 83
	goto_if_gt OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Hi
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go:
	subvar VAR_0x8005, 20
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_SUNBURST, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Lo:
	setvar VAR_0x8005, 20
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go
OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Hi:
	setvar VAR_0x8005, 83
	goto OrangeIslands_Route_7_EventScript_SeaEdgeToSunburst_Go
