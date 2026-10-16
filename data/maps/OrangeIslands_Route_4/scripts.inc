OrangeIslands_Route_4_MapScripts::
	.byte 0

OrangeIslands_Route_4_EventScript_West::
	msgbox OrangeIslands_Route_4_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_4_EventScript_East::
	msgbox OrangeIslands_Route_4_Text_East, MSGBOX_SIGN
	end

OrangeIslands_Route_4_EventScript_Sandbar::
	msgbox OrangeIslands_Route_4_Text_Sandbar, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Route_4_EventScript_SailToRoute1::
	warp MAP_ORANGE_ISLANDS_ROUTE_1, 1, 9
	waitstate
	end
