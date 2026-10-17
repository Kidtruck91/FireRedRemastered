OrangeIslands_Pinkan_MapScripts::
	.byte 0

OrangeIslands_Pinkan_EventScript_Beach::
	msgbox OrangeIslands_Pinkan_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Pinkan_EventScript_Town::
	msgbox OrangeIslands_Pinkan_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 13
OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 24
	goto_if_lt OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8005, 43
	goto_if_gt OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13_Go:
	addvar VAR_0x8005, 6
	setvar VAR_0x8004, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8005, 24
	goto OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8005, 43
	goto OrangeIslands_Pinkan_EventScript_SeaEdgeToRoute13_Go
