Blackthorn_Pokecenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Blackthorn_Pokecenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

Blackthorn_Pokecenter_1F_OnTransition::
	setrespawn SPAWN_BLACKTHORN_CITY
	end

Blackthorn_Pokecenter_1F_EventScript_Nurse::
	lock
	faceplayer
	call EventScript_PkmnCenterNurse
	release
	end
