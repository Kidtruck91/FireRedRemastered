PalletTown_PlayersHouse_OptionsCloset_MapScripts::
	.byte 0

@ Each furnishing shows what its setting is on and offers to change it. The
@ option index goes in VAR_0x8004; BufferGameOption and CycleGameOption
@ (src/game_options.c) fill STR_VAR_1 with the setting's name and STR_VAR_2
@ with its value, so one pair of texts serves every option and adding another
@ setting needs no new text.
PalletTown_PlayersHouse_OptionsCloset_EventScript_Nuzlocke::
	setvar VAR_0x8004, 0
	goto PalletTown_PlayersHouse_OptionsCloset_EventScript_Option
	end

PalletTown_PlayersHouse_OptionsCloset_EventScript_MugshotPalette::
	setvar VAR_0x8004, 1
	goto PalletTown_PlayersHouse_OptionsCloset_EventScript_Option
	end

PalletTown_PlayersHouse_OptionsCloset_EventScript_LevelCap::
	setvar VAR_0x8004, 2
	goto PalletTown_PlayersHouse_OptionsCloset_EventScript_Option
	end

PalletTown_PlayersHouse_OptionsCloset_EventScript_PhysSpecSplit::
	setvar VAR_0x8004, 3
	goto PalletTown_PlayersHouse_OptionsCloset_EventScript_Option
	end

PalletTown_PlayersHouse_OptionsCloset_EventScript_ExpAll::
	setvar VAR_0x8004, 4
	goto PalletTown_PlayersHouse_OptionsCloset_EventScript_Option
	end

PalletTown_PlayersHouse_OptionsCloset_EventScript_Option::
	lockall
	callnative BufferGameOption
	msgbox PalletTown_PlayersHouse_OptionsCloset_Text_OptionIs, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, PalletTown_PlayersHouse_OptionsCloset_EventScript_OptionDone
	callnative CycleGameOption
	msgbox PalletTown_PlayersHouse_OptionsCloset_Text_OptionChanged
PalletTown_PlayersHouse_OptionsCloset_EventScript_OptionDone::
	releaseall
	end
