OrangeIslands_Route_10_MapScripts::
	.byte 0

OrangeIslands_Route_10_EventScript_West::
	msgbox OrangeIslands_Route_10_Text_West, MSGBOX_SIGN
	end

OrangeIslands_Route_10_EventScript_Debris::
	msgbox OrangeIslands_Route_10_Text_Debris, MSGBOX_SIGN
	end

@ Sea warp: different tilesets, so no seamless edge
OrangeIslands_Route_10_EventScript_SailToNavelBeach::
	warp MAP_ORANGE_ISLANDS_NAVEL_BEACH, 86, 5
	waitstate
	end
