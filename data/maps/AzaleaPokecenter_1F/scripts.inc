AzaleaPokecenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, AzaleaPokecenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

AzaleaPokecenter_1F_OnTransition::
	setrespawn SPAWN_AZALEA_TOWN
	end

AzaleaPokecenter_1F_EventScript_Nurse::
	lock
	faceplayer
	call EventScript_PkmnCenterNurse
	release
	end
