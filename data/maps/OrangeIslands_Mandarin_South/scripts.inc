OrangeIslands_Mandarin_South_MapScripts::
	.byte 0

OrangeIslands_Mandarin_South_EventScript_Beach::
	msgbox OrangeIslands_Mandarin_South_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Mandarin_South_EventScript_Town::
	msgbox OrangeIslands_Mandarin_South_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 13
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 24
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13_Lo
	compare VAR_0x8004, 71
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13_Go:
	subvar VAR_0x8004, 4
	setvar VAR_0x8005, 77
	warp MAP_ORANGE_ISLANDS_ROUTE_13, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13_Lo:
	setvar VAR_0x8004, 24
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13_Hi:
	setvar VAR_0x8004, 71
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute13_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 9
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 30
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9_Lo
	compare VAR_0x8004, 65
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9_Go:
	subvar VAR_0x8004, 22
	setvar VAR_0x8005, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_9, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9_Lo:
	setvar VAR_0x8004, 30
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9_Hi:
	setvar VAR_0x8004, 65
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute9_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 10
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 14
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10_Lo
	compare VAR_0x8005, 61
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10_Go:
	subvar VAR_0x8005, 4
	setvar VAR_0x8004, 101
	warp MAP_ORANGE_ISLANDS_ROUTE_10, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10_Lo:
	setvar VAR_0x8005, 14
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10_Hi:
	setvar VAR_0x8005, 61
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute10_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 12
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 20
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12_Lo
	compare VAR_0x8005, 55
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12_Go:
	subvar VAR_0x8005, 16
	setvar VAR_0x8004, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_12, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12_Lo:
	setvar VAR_0x8005, 20
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12_Hi:
	setvar VAR_0x8005, 55
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute12_Go
