OrangeIslands_Route_14_MapScripts::
	.byte 0

OrangeIslands_Route_14_EventScript_Resort::
	msgbox OrangeIslands_Route_14_Text_Resort, MSGBOX_SIGN
	end

OrangeIslands_Route_14_EventScript_South::
	msgbox OrangeIslands_Route_14_Text_South, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Trovita
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 20
	goto_if_lt OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Lo
	compare VAR_0x8004, 67
	goto_if_gt OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Hi
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Go:
	subvar VAR_0x8004, 8
	setvar VAR_0x8005, 1
	warp MAP_ORANGE_ISLANDS_TROVITA, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Lo:
	setvar VAR_0x8004, 20
	goto OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Go
OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Hi:
	setvar VAR_0x8004, 67
	goto OrangeIslands_Route_14_EventScript_SeaEdgeToTrovita_Go
