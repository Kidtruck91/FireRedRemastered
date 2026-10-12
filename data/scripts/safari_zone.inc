@ VAR_SAFARI_RETURN_POINT: 0 = Fuchsia SAFARI ZONE, 1 = Six Island LUXURY SAFARI
SafariZone_EventScript_OutOfBallsMidBattle::
	special ExitSafariMode
	goto_if_eq VAR_SAFARI_RETURN_POINT, 1, SafariZone_EventScript_OutOfBallsMidBattleSixIsland
	setvar VAR_MAP_SCENE_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE, 3
	setwarp MAP_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE, 4, 1
	end

SafariZone_EventScript_OutOfBallsMidBattleSixIsland::
	setvar VAR_MAP_SCENE_SIX_ISLAND_SAFARI_ENTRANCE, 3
	setwarp MAP_SIX_ISLAND_SAFARI_ENTRANCE, 4, 1
	end

SafariZone_EventScript_Exit::
	special ExitSafariMode
	goto_if_eq VAR_SAFARI_RETURN_POINT, 1, SafariZone_EventScript_ExitSixIsland
	setvar VAR_MAP_SCENE_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE, 1
	warp MAP_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE, 4, 1
	waitstate
	end

SafariZone_EventScript_ExitSixIsland::
	setvar VAR_MAP_SCENE_SIX_ISLAND_SAFARI_ENTRANCE, 1
	warp MAP_SIX_ISLAND_SAFARI_ENTRANCE, 4, 1
	waitstate
	end

SafariZone_EventScript_RetirePrompt::
	lockall
	msgbox SafariZone_Text_WouldYouLikeToExit, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, SafariZone_EventScript_Retire
	releaseall
	end

SafariZone_EventScript_Retire::
	goto SafariZone_EventScript_Exit

SafariZone_EventScript_TimesUp::
	lockall
	playse SE_DING_DONG
	msgbox SafariZone_Text_TimesUp
	goto SafariZone_EventScript_Exit
	end

SafariZone_EventScript_OutOfBalls::
	lockall
	playse SE_DING_DONG
	msgbox SafariZone_Text_OutOfBalls
	goto SafariZone_EventScript_Exit
	end
