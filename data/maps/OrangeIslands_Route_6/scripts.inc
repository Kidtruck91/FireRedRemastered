OrangeIslands_Route_6_MapScripts::
	.byte 0

OrangeIslands_Route_6_EventScript_South::
	msgbox OrangeIslands_Route_6_Text_South, MSGBOX_SIGN
	end

OrangeIslands_Route_6_EventScript_East::
	msgbox OrangeIslands_Route_6_Text_East, MSGBOX_SIGN
	end

OrangeIslands_Route_6_EventScript_Interior::
	msgbox OrangeIslands_Route_6_Text_Interior, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Route_6_EventScript_SailToMikan::
	warp MAP_ORANGE_ISLANDS_MIKAN, 1, 15
	waitstate
	end
