OrangeIslands_Route_2_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, OrangeIslands_Route_2_OnResume
	.byte 0

OrangeIslands_Route_2_OnResume:
	setdivewarp MAP_ORANGE_ISLANDS_ROUTE_2_GROTTO, 8, 7
	end

OrangeIslands_Route_2_EventScript_Jungle::
	msgbox OrangeIslands_Route_2_Text_Jungle, MSGBOX_SIGN
	end

OrangeIslands_Route_2_EventScript_Pond::
	msgbox OrangeIslands_Route_2_Text_Pond, MSGBOX_SIGN
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Route_2_EventScript_WalkToValencia_24_2::
	warp MAP_ORANGE_ISLANDS_VALENCIA, 39, 2
	waitstate
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Route_2_EventScript_WalkToValencia_25_2::
	warp MAP_ORANGE_ISLANDS_VALENCIA, 40, 2
	waitstate
	end
