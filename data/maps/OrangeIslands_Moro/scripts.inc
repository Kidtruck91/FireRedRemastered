OrangeIslands_Moro_MapScripts::
	.byte 0

OrangeIslands_Moro_EventScript_Beach::
	msgbox OrangeIslands_Moro_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Moro_EventScript_Town::
	msgbox OrangeIslands_Moro_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Moro_EventScript_BoardGhostShip::
	warp MAP_ORANGE_ISLANDS_MORO_GHOST_SHIP, 8, 10
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 7
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 16
	goto_if_lt OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Lo
	compare VAR_0x8004, 55
	goto_if_gt OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Hi
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Go:
	addvar VAR_0x8004, 4
	setvar VAR_0x8005, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_7, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Lo:
	setvar VAR_0x8004, 16
	goto OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Go
OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Hi:
	setvar VAR_0x8004, 55
	goto OrangeIslands_Moro_EventScript_SeaEdgeToRoute7_Go
