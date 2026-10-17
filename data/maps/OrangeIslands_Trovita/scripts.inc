OrangeIslands_Trovita_MapScripts::
	.byte 0

OrangeIslands_Trovita_EventScript_Beach::
	msgbox OrangeIslands_Trovita_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Trovita_EventScript_Town::
	msgbox OrangeIslands_Trovita_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Trovita_EventScript_WalkToTrovitaFalls::
	warp MAP_ORANGE_ISLANDS_TROVITA_FALLS, 27, 45
	waitstate
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 12
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 16
	goto_if_lt OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12_Lo
	compare VAR_0x8005, 43
	goto_if_gt OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12_Hi
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12_Go:
	subvar VAR_0x8005, 12
	setvar VAR_0x8004, 137
	warp MAP_ORANGE_ISLANDS_ROUTE_12, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12_Lo:
	setvar VAR_0x8005, 16
	goto OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12_Go
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12_Hi:
	setvar VAR_0x8005, 43
	goto OrangeIslands_Trovita_EventScript_SeaEdgeToRoute12_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 14
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 12
	goto_if_lt OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Lo
	compare VAR_0x8004, 59
	goto_if_gt OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Hi
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Go:
	addvar VAR_0x8004, 8
	setvar VAR_0x8005, 69
	warp MAP_ORANGE_ISLANDS_ROUTE_14, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Lo:
	setvar VAR_0x8004, 12
	goto OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Go
OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Hi:
	setvar VAR_0x8004, 59
	goto OrangeIslands_Trovita_EventScript_SeaEdgeToRoute14_Go
