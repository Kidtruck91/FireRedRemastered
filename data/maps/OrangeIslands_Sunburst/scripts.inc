OrangeIslands_Sunburst_MapScripts::
	.byte 0

OrangeIslands_Sunburst_EventScript_Beach::
	msgbox OrangeIslands_Sunburst_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Sunburst_EventScript_Town::
	msgbox OrangeIslands_Sunburst_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 6
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 8
	goto_if_lt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6_Lo
	compare VAR_0x8004, 55
	goto_if_gt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6_Hi
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6_Go:
	addvar VAR_0x8004, 12
	setvar VAR_0x8005, 69
	warp MAP_ORANGE_ISLANDS_ROUTE_6, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6_Lo:
	setvar VAR_0x8004, 8
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6_Go
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6_Hi:
	setvar VAR_0x8004, 55
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 7
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 0
	goto_if_lt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Lo
	compare VAR_0x8005, 63
	goto_if_gt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Hi
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go:
	addvar VAR_0x8005, 20
	setvar VAR_0x8004, 63
	warp MAP_ORANGE_ISLANDS_ROUTE_7, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Lo:
	setvar VAR_0x8005, 0
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Hi:
	setvar VAR_0x8005, 63
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go

@ Sea corner: choose the crossed edge from the incoming direction
OrangeIslands_Sunburst_EventScript_SeaCorner_0_0::
	specialvar VAR_RESULT, GetPlayerFacingDirection
	compare VAR_RESULT, DIR_NORTH
	goto_if_eq OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute6
	compare VAR_RESULT, DIR_WEST
	goto_if_eq OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7
	end
