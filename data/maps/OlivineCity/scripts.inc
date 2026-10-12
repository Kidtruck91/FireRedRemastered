OlivineCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OlivineCity_OnTransition
	.byte 0

OlivineCity_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_OLIVINE_CITY
	end
