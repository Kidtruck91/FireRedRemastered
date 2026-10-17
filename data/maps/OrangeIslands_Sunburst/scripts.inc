OrangeIslands_Sunburst_MapScripts::
	.byte 0

OrangeIslands_Sunburst_EventScript_Beach::
	msgbox OrangeIslands_Sunburst_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Sunburst_EventScript_Town::
	msgbox OrangeIslands_Sunburst_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 7
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 2
	goto_if_lt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Lo
	compare VAR_0x8004, 60
	goto_if_gt OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Hi
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go:
	addvar VAR_0x8004, 6
	setvar VAR_0x8005, 86
	warp MAP_ORANGE_ISLANDS_ROUTE_7, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Lo:
	setvar VAR_0x8004, 2
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go
OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Hi:
	setvar VAR_0x8004, 60
	goto OrangeIslands_Sunburst_EventScript_SeaEdgeToRoute7_Go
