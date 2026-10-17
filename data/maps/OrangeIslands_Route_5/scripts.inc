OrangeIslands_Route_5_MapScripts::
	.byte 0

OrangeIslands_Route_5_EventScript_Currents::
	msgbox OrangeIslands_Route_5_Text_Currents, MSGBOX_SIGN
	end

OrangeIslands_Route_5_EventScript_North::
	msgbox OrangeIslands_Route_5_Text_North, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mikan
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Lo
	compare VAR_0x8004, 47
	goto_if_gt OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Hi
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Go:
	addvar VAR_0x8004, 8
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_MIKAN, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Go
OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Hi:
	setvar VAR_0x8004, 47
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToMikan_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Navel
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 0
	goto_if_lt OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Lo
	compare VAR_0x8004, 47
	goto_if_gt OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Hi
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Go:
	addvar VAR_0x8004, 21
	setvar VAR_0x8005, 63
	warp MAP_ORANGE_ISLANDS_NAVEL, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Lo:
	setvar VAR_0x8004, 0
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Go
OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Hi:
	setvar VAR_0x8004, 47
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToNavel_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 6
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 63
	goto_if_lt OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Lo
	compare VAR_0x8005, 70
	goto_if_gt OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Hi
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Go:
	subvar VAR_0x8005, 33
	setvar VAR_0x8004, 86
	warp MAP_ORANGE_ISLANDS_ROUTE_6, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Lo:
	setvar VAR_0x8005, 63
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Go
OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Hi:
	setvar VAR_0x8005, 70
	goto OrangeIslands_Route_5_EventScript_SeaEdgeToRoute6_Go
