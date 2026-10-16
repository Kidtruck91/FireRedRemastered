OrangeIslands_Route_9_MapScripts::
	.byte 0

OrangeIslands_Route_9_EventScript_South::
	msgbox OrangeIslands_Route_9_Text_South, MSGBOX_SIGN
	end

OrangeIslands_Route_9_EventScript_Platform::
	msgbox OrangeIslands_Route_9_Text_Platform, MSGBOX_SIGN
	end

@ Sea warp: different tilesets, so no seamless edge
OrangeIslands_Route_9_EventScript_SailToNavelBeach::
	warp MAP_ORANGE_ISLANDS_NAVEL_BEACH, 1, 5
	waitstate
	end
