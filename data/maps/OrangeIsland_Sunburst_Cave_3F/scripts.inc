OrangeIsland_Sunburst_Cave_3F_MapScripts::
	.byte 0

OrangeIsland_Sunburst_Cave_3F_EventScript_ItemStarPiece::
	finditem ITEM_STAR_PIECE
	end

@ --- built by docs/orange-islands/build_sunburst_cave.py ---
OrangeIsland_Sunburst_Cave_3F_EventScript_Grant::
	trainerbattle_single TRAINER_ORANGE_HIKER_GRANT, OrangeIsland_Sunburst_Cave_3F_Text_GrantIntro, OrangeIsland_Sunburst_Cave_3F_Text_GrantDefeat
	msgbox OrangeIsland_Sunburst_Cave_3F_Text_GrantPost, MSGBOX_AUTOCLOSE
	end

OrangeIsland_Sunburst_Cave_3F_EventScript_Odar::
	trainerbattle_single TRAINER_ORANGE_BLACK_BELT_ODAR, OrangeIsland_Sunburst_Cave_3F_Text_OdarIntro, OrangeIsland_Sunburst_Cave_3F_Text_OdarDefeat
	msgbox OrangeIsland_Sunburst_Cave_3F_Text_OdarPost, MSGBOX_AUTOCLOSE
	end

OrangeIsland_Sunburst_Cave_3F_EventScript_ItemRevive::
	finditem ITEM_REVIVE
	end

@ The STEELIX in the bottom room. setwildbattle builds a plain wild
@ mon; the callnative gives it SHEER FORCE -- its hidden ability --
@ and the moveset that goes with it. See src/orange_static_mons.c.
OrangeIsland_Sunburst_Cave_3F_EventScript_Steelix::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_STEELIX, CRY_MODE_ENCOUNTER
	message OrangeIsland_Sunburst_Cave_3F_Text_Steelix
	waitmessage
	waitmoncry
	delay 20
	playbgm MUS_ENCOUNTER_GYM_LEADER, 0
	waitbuttonpress
	setwildbattle SPECIES_STEELIX, 25
	callnative OrangeStaticMon_SunburstSteelix
	setflag FLAG_SYS_SPECIAL_WILD_BATTLE
	special StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_SPECIAL_WILD_BATTLE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, OrangeIsland_Sunburst_Cave_3F_EventScript_SteelixGone
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, OrangeIsland_Sunburst_Cave_3F_EventScript_SteelixGone
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, OrangeIsland_Sunburst_Cave_3F_EventScript_SteelixGone
	release
	end

@ Caught, beaten or fled from, it does not stay in the room. The
@ local id is its place in the map object list, worked out when this
@ was written rather than guessed.
OrangeIsland_Sunburst_Cave_3F_EventScript_SteelixGone::
	setflag FLAG_ORANGE_SUNBURST_STEELIX
	removeobject 5
	release
	end
@ --- end build_sunburst_cave.py ---
