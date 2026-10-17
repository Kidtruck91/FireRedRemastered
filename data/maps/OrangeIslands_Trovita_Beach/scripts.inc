OrangeIslands_Trovita_Beach_MapScripts::
	.byte 0

OrangeIslands_Trovita_Beach_EventScript_Town::
	msgbox OrangeIslands_Trovita_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Trovita_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Trovita_Beach_Text_Harbor, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 11
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 16
	goto_if_lt OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Lo
	compare VAR_0x8004, 55
	goto_if_gt OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Hi
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Go:
	addvar VAR_0x8004, 4
	setvar VAR_0x8005, 2
	warp MAP_ORANGE_ISLANDS_ROUTE_11, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Lo:
	setvar VAR_0x8004, 16
	goto OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Go
OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Hi:
	setvar VAR_0x8004, 55
	goto OrangeIslands_Trovita_Beach_EventScript_SeaEdgeToRoute11_Go
