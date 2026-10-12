MohoganyTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MohoganyTown_OnTransition
	.byte 0

MohoganyTown_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_MAHOGANY_TOWN
	end
