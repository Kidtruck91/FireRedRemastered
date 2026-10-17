OrangeIslands_Route_17_MapScripts::
	.byte 0

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 10
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 33
	goto_if_lt OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Lo
	compare VAR_0x8005, 54
	goto_if_gt OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Hi
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Go:
	subvar VAR_0x8005, 32
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_10, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Lo:
	setvar VAR_0x8005, 33
	goto OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Go
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Hi:
	setvar VAR_0x8005, 54
	goto OrangeIslands_Route_17_EventScript_SeaEdgeToRoute10_Go
