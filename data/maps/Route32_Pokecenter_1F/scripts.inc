Route32_Pokecenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Route32_Pokecenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

Route32_Pokecenter_1F_OnTransition::
	setrespawn SPAWN_ROUTE_32
	end

Route32_Pokecenter_1F_EventScript_Nurse::
	lock
	faceplayer
	call EventScript_PkmnCenterNurse
	release
	end
