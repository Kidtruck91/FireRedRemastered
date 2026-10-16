OrangeIslands_Valencia_MapScripts::
	.byte 0

OrangeIslands_Valencia_EventScript_Beach::
	msgbox OrangeIslands_Valencia_Text_Beach, MSGBOX_SIGN
	end

OrangeIslands_Valencia_EventScript_Town::
	msgbox OrangeIslands_Valencia_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_49::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 33
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_50::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 34
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_51::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 35
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_52::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 36
	waitstate
	end

OrangeIslands_Valencia_EventScript_EdgeToValenciaBeach_53::
	warp MAP_ORANGE_ISLANDS_VALENCIA_BEACH, 8, 37
	waitstate
	end

@ Edge warp: different tilesets, so no seamless edge
OrangeIslands_Valencia_EventScript_WalkToRoute2_27_44::
	warp MAP_ORANGE_ISLANDS_ROUTE_2, 27, 44
	waitstate
	end
