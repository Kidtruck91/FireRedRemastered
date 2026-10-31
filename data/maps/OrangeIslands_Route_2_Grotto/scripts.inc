OrangeIslands_Route_2_Grotto_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, OrangeIslands_Route_2_Grotto_OnResume
	.byte 0

OrangeIslands_Route_2_Grotto_OnResume:
	setdivewarp MAP_ORANGE_ISLANDS_ROUTE_2, 26, 23
	end

OrangeIslands_Route_2_Grotto_EventScript_ItemSunStone::
	finditem ITEM_SUN_STONE
	end

OrangeIslands_Route_2_Grotto_EventScript_ItemBigPearl::
	finditem ITEM_BIG_PEARL
	end

