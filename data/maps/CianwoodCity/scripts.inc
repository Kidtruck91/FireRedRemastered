CianwoodCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CianwoodCity_OnTransition
	.byte 0

CianwoodCity_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_CIANWOOD_CITY
	end
