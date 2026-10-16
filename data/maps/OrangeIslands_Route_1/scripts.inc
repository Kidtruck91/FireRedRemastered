OrangeIslands_Route_1_MapScripts::
	.byte 0

OrangeIslands_Route_1_EventScript_Ferry::
	msgbox OrangeIslands_Route_1_Text_Ferry, MSGBOX_SIGN
	end

OrangeIslands_Route_1_EventScript_Beach::
	msgbox OrangeIslands_Route_1_Text_Beach, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Route_1_EventScript_SailToRoute4::
	warp MAP_ORANGE_ISLANDS_ROUTE_4, 118, 23
	waitstate
	end
