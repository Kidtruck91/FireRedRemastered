OrangeIslands_Route_17_MapScripts::
	.byte 0

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 9
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 4
	goto_if_lt OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9_Lo
	compare VAR_0x8005, 49
	goto_if_gt OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9_Hi
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9_Go:
	addvar VAR_0x8005, 30
	setvar VAR_0x8004, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_9, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9_Lo:
	setvar VAR_0x8005, 4
	goto OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9_Go
OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9_Hi:
	setvar VAR_0x8005, 49
	goto OrangeIslands_Route_17_EventScript_SeaEdgeToRoute9_Go
