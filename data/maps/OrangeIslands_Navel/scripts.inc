OrangeIslands_Navel_MapScripts::
	.byte 0

OrangeIslands_Navel_EventScript_Gym::
	msgbox OrangeIslands_Navel_Text_Gym, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_Town::
	msgbox OrangeIslands_Navel_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_Beach::
	msgbox OrangeIslands_Navel_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_WestTrail::
	msgbox OrangeIslands_Navel_Text_WestTrail, MSGBOX_SIGN
	end

OrangeIslands_Navel_EventScript_EastTrail::
	msgbox OrangeIslands_Navel_Text_EastTrail, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 5
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 23
	goto_if_lt OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Lo
	compare VAR_0x8004, 66
	goto_if_gt OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Hi
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Go:
	subvar VAR_0x8004, 21
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_5, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Lo:
	setvar VAR_0x8004, 23
	goto OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Go
OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Hi:
	setvar VAR_0x8004, 66
	goto OrangeIslands_Navel_EventScript_SeaEdgeToRoute5_Go
