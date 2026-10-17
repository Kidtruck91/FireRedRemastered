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

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 5
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Lo
	compare VAR_0x8005, 71
	goto_if_gt OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Hi
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Go:
	addvar VAR_0x8005, 33
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_5, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Go
OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Hi:
	setvar VAR_0x8005, 71
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToRoute5_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8005, 65
	goto_if_gt OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Go:
	addvar VAR_0x8005, 10
	setvar VAR_0x8004, 94
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8005, 65
	goto OrangeIslands_Route_6_EventScript_SeaEdgeToMandarinSouth_Go
