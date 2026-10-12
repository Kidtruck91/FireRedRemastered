AzaleaTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, AzaleaTown_OnTransition
	.byte 0

AzaleaTown_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_AZALEA_TOWN
	end
