OrangeIslands_Kumquat_Beach_MapScripts::
	.byte 0

@ Sea edge: surfing off the west side crosses to Kumquat's east coast.
@ A map connection cannot be used here -- Kumquat is built on gTileset_HoennGeneral
@ and this map on gTileset_OrangeGeneral, and a connected map's edge strip is drawn
@ with the tilesets of the map the player is standing on, so the seam would come out
@ with the wrong graphics and palettes. Every other Orange sea link is a trigger for
@ the same reason.
@ The landing spot is fixed rather than matched to the row the player left on: the
@ east side of Kumquat is open water only in rows 0-23 and 41-59, so there is no
@ column that lines up with all 40 rows of this map's west edge.
OrangeIslands_Kumquat_Beach_EventScript_SeaEdgeToKumquat::
	warp MAP_ORANGE_ISLANDS_KUMQUAT, 63, 18
	waitstate
	end

OrangeIslands_Kumquat_Beach_EventScript_Town::
	msgbox OrangeIslands_Kumquat_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Kumquat_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Kumquat_Beach_Text_Harbor, MSGBOX_SIGN
	end
