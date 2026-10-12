EcruteakCity_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, EcruteakCity_OnTransition
	.byte 0

EcruteakCity_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_ECRUTEAK_CITY
	end

