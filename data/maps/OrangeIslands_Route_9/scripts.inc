OrangeIslands_Route_9_MapScripts::
	.byte 0

OrangeIslands_Route_9_EventScript_South::
	msgbox OrangeIslands_Route_9_Text_South, MSGBOX_SIGN
	end

OrangeIslands_Route_9_EventScript_Platform::
	msgbox OrangeIslands_Route_9_Text_Platform, MSGBOX_SIGN
	end

@ Sea warp: different tilesets, so no seamless edge

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 17
OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 34
	goto_if_lt OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Lo
	compare VAR_0x8005, 72
	goto_if_gt OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Hi
OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Go:
	subvar VAR_0x8005, 30
	setvar VAR_0x8004, 38
	warp MAP_ORANGE_ISLANDS_ROUTE_17, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Lo:
	setvar VAR_0x8005, 34
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Go
OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Hi:
	setvar VAR_0x8005, 72
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToRoute17_Go

@ Kinnow's whole south coast was unused; this is the sea below it.
OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 8
	goto_if_lt OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow_Lo
	compare VAR_0x8004, 43
	goto_if_gt OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow_Hi
OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow_Go:
	addvar VAR_0x8004, 5
	setvar VAR_0x8005, 64
	warp MAP_ORANGE_ISLANDS_KINNOW, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow_Lo:
	setvar VAR_0x8004, 8
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow_Go
OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow_Hi:
	setvar VAR_0x8004, 43
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToKinnow_Go

@ Navel's south-west corner; Route 5 has x 23-66 of that coast.
OrangeIslands_Route_9_EventScript_SeaEdgeToNavel::
	getplayerxy VAR_0x8005, VAR_0x8004
	compare VAR_0x8004, 106
	goto_if_lt OrangeIslands_Route_9_EventScript_SeaEdgeToNavel_Lo
	compare VAR_0x8004, 113
	goto_if_gt OrangeIslands_Route_9_EventScript_SeaEdgeToNavel_Hi
OrangeIslands_Route_9_EventScript_SeaEdgeToNavel_Go:
	subvar VAR_0x8004, 104
	setvar VAR_0x8005, 63
	warp MAP_ORANGE_ISLANDS_NAVEL, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_9_EventScript_SeaEdgeToNavel_Lo:
	setvar VAR_0x8004, 106
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToNavel_Go
OrangeIslands_Route_9_EventScript_SeaEdgeToNavel_Hi:
	setvar VAR_0x8004, 113
	goto OrangeIslands_Route_9_EventScript_SeaEdgeToNavel_Go
