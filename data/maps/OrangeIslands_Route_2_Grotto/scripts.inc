OrangeIslands_Route_2_Grotto_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, OrangeIslands_Route_2_Grotto_OnResume
	.byte 0

OrangeIslands_Route_2_Grotto_OnResume:
	@ Surfaces on the northern pond's dive tile. Route 2 now has two ponds and
	@ both have dive tiles, but setdivewarp takes one destination per map, so
	@ diving in the southern pond also surfaces here.
	setdivewarp MAP_ORANGE_ISLANDS_ROUTE_2, 26, 26
	end

OrangeIslands_Route_2_Grotto_EventScript_ItemSunStone::
	finditem ITEM_SUN_STONE
	end

OrangeIslands_Route_2_Grotto_EventScript_ItemBigPearl::
	finditem ITEM_BIG_PEARL
	end

