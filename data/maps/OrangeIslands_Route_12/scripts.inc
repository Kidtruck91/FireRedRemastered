OrangeIslands_Route_12_MapScripts::
	.byte 0

OrangeIslands_Route_12_EventScript_West::
	msgbox OrangeIslands_Route_12_Text_West, MSGBOX_SIGN
	end

@ Sea edge: any surfable cell on this edge, landing at the matching spot on Mandarin South
@ Mandarin South's east coast is now cliff from row 0 to 67; only rows
@ 68-73 are open water, and this edge (rows 4-31) cannot reach them by
@ carrying the row across, so the crossing lands mid-channel instead.
@ Mandarin South's east coast is now cliff from row 0 to 67; only rows
@ 68-73 are open water, and this edge (rows 4-31) cannot reach them by
@ carrying the row across, so the crossing lands mid-channel instead.
OrangeIslands_Route_12_EventScript_SeaEdgeToMandarinSouth::
	setvar VAR_0x8004, 94
	setvar VAR_0x8005, 71
	warp MAP_ORANGE_ISLANDS_MANDARIN_SOUTH, VAR_0x8004, VAR_0x8005
	waitstate
	end

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
