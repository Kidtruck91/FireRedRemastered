OrangeIslands_Mikan_MapScripts::
	.byte 0

OrangeIslands_Mikan_EventScript_Gym::
	msgbox OrangeIslands_Mikan_Text_Gym, MSGBOX_SIGN
	end

OrangeIslands_Mikan_EventScript_Beach::
	msgbox OrangeIslands_Mikan_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Mikan_EventScript_Town::
	msgbox OrangeIslands_Mikan_Text_Town, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Mikan_EventScript_SailToRoute5::
	warp MAP_ORANGE_ISLANDS_ROUTE_5, 24, 122
	waitstate
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Mikan_EventScript_SailToRoute6::
	warp MAP_ORANGE_ISLANDS_ROUTE_6, 85, 33
	waitstate
	end
