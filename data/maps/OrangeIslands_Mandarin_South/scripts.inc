OrangeIslands_Mandarin_South_MapScripts::
	.byte 0

OrangeIslands_Mandarin_South_EventScript_Beach::
	msgbox OrangeIslands_Mandarin_South_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Mandarin_South_EventScript_Town::
	msgbox OrangeIslands_Mandarin_South_Text_Town, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 6
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 68
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Lo
	compare VAR_0x8005, 75
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Go:
	subvar VAR_0x8005, 10
	setvar VAR_0x8004, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_6, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Lo:
	setvar VAR_0x8005, 68
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Hi:
	setvar VAR_0x8005, 75
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute6_Go

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Route 8
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 1
	goto_if_lt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Lo
	compare VAR_0x8004, 94
	goto_if_gt OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Hi
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Go:
	addvar VAR_0x8004, 36
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_ROUTE_8, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Lo:
	setvar VAR_0x8004, 1
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Go
OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Hi:
	setvar VAR_0x8004, 94
	goto OrangeIslands_Mandarin_South_EventScript_SeaEdgeToRoute8_Go
