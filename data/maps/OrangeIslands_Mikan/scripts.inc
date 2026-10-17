OrangeIslands_Mikan_MapScripts::
	.byte 0

OrangeIslands_Mikan_EventScript_Gym::
	msgbox OrangeIslands_Mikan_Text_Gym, MSGBOX_SIGN
	end

OrangeIslands_Mikan_EventScript_Beach::
	msgbox OrangeIslands_Mikan_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Mikan_EventScript_Town::
	msgbox OrangeIslands_Mikan_Text_Town, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Mikan_EventScript_SailToRoute6::
	warp MAP_ORANGE_ISLANDS_ROUTE_6, 85, 33
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 5
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 8
	goto_if_lt OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Lo
	compare VAR_0x8004, 55
	goto_if_gt OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Hi
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Go:
	subvar VAR_0x8004, 8
	setvar VAR_0x8005, 122
	warp MAP_ORANGE_ISLANDS_ROUTE_5, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Lo:
	setvar VAR_0x8004, 8
	goto OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Go
OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Hi:
	setvar VAR_0x8004, 55
	goto OrangeIslands_Mikan_EventScript_SeaEdgeToRoute5_Go
