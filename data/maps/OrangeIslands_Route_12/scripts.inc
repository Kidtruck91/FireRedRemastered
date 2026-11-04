OrangeIslands_Route_12_MapScripts::
	.byte 0

OrangeIslands_Route_12_EventScript_West::
	msgbox OrangeIslands_Route_12_Text_West, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth::
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8005, 4
	goto_if_lt OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Lo
	compare VAR_0x8005, 39
	goto_if_gt OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Hi
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Go:
	addvar VAR_0x8005, 16
	setvar VAR_0x8004, 94
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Lo:
	setvar VAR_0x8005, 4
	goto OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Go
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Hi:
	setvar VAR_0x8005, 39
	goto OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth_Go

@ Sea edge: any surfable cell on this edge crosses to Trovita.
@ Trovita's west coast is open on rows 41-54 only -- the rest is cliff -- while this
@ edge is open on rows 4-31. Carrying the row across put 25 of the 28 landings inside
@ that cliff, so the crossing lands at one spot on the open stretch instead: column 7,
@ the first that shows no border on a 72-wide map, and row 47, the middle of the coast.
@ Both are open water for the whole stretch.
OrangeIslands_Route_12_EventScript_SeaEdgeToTrovita::
	warp MAP_ORANGE_ISLANDS_TROVITA, 7, 47
	waitstate
	end
