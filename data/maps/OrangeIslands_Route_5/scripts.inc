OrangeIslands_Route_5_MapScripts::
	.byte 0

OrangeIslands_Route_5_EventScript_Currents::
	msgbox OrangeIslands_Route_5_Text_Currents, MSGBOX_SIGN
	end

OrangeIslands_Route_5_EventScript_North::
	msgbox OrangeIslands_Route_5_Text_North, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Route_5_EventScript_SailToMikan::
	warp MAP_ORANGE_ISLANDS_MIKAN, 31, 1
	waitstate
	end
