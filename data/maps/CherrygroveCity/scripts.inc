CherrygroveCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CherrygroveCity_OnTransition
	.byte 0

CherrygroveCity_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_CHERRYGROVE_CITY
	end
