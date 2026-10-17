OrangeIslands_Route_16_MapScripts::
	.byte 0

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 13
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 42
	goto_if_lt OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8005, 61
	goto_if_gt OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Go:
	subvar VAR_0x8005, 12
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8005, 42
	goto OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8005, 61
	goto OrangeIslands_Route_16_EventScript_SeaEdgeToRoute13_Go
