Cherrygrove_Pokecenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Cherrygrove_Pokecenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

Cherrygrove_Pokecenter_1F_OnTransition::
	setrespawn SPAWN_CHERRYGROVE_CITY
	end

Cherrygrove_Pokecenter_1F_EventScript_Nurse::
	lock
	faceplayer
	call EventScript_PkmnCenterNurse
	release
	end
