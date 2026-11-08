OrangeIslands_Kinnow_Massage_MapScripts::
	.byte 0

OrangeIslands_Kinnow_Massage_EventScript_Customer::
	msgbox OrangeIslands_Kinnow_Massage_Text_Customer, MSGBOX_NPC
	end

OrangeIslands_Kinnow_Massage_EventScript_Girl::
	msgbox OrangeIslands_Kinnow_Massage_Text_Girl, MSGBOX_NPC
	end

@ The parlour. The price buys repetitions of the game's own massage,
@ which is worth a flat three friendship however high the mon already is.
OrangeIslands_Kinnow_Massage_EventScript_Masseuse::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	special QuestLog_CutRecording
	showmoneybox 0, 0
	msgbox OrangeIslands_Kinnow_Massage_Text_Menu, MSGBOX_DEFAULT
	multichoice 0, 0, MULTICHOICE_KINNOW_MASSAGE, FALSE
	goto_if_eq VAR_RESULT, 0, OrangeIslands_Kinnow_Massage_EventScript_Short
	goto_if_eq VAR_RESULT, 1, OrangeIslands_Kinnow_Massage_EventScript_Long
	goto OrangeIslands_Kinnow_Massage_EventScript_Declined

OrangeIslands_Kinnow_Massage_EventScript_Short::
	checkmoney 500
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Kinnow_Massage_EventScript_NoMoney
	msgbox OrangeIslands_Kinnow_Massage_Text_WhichMon
	special ChoosePartyMon
	waitstate
	lock
	faceplayer
	goto_if_ge VAR_0x8004, PARTY_SIZE, OrangeIslands_Kinnow_Massage_EventScript_Declined
	specialvar VAR_RESULT, GetPartyMonSpecies
	goto_if_eq VAR_RESULT, SPECIES_EGG, OrangeIslands_Kinnow_Massage_EventScript_NotAnEgg
	removemoney 500
	updatemoneybox
	msgbox OrangeIslands_Kinnow_Massage_Text_LieBack
	closemessage
	fadescreen FADE_TO_BLACK
	playfanfare MUS_HEAL
	waitfanfare
	special DaisyMassageServices
	special DaisyMassageServices
	fadescreen FADE_FROM_BLACK
	hidemoneybox
	special BufferMonNickname
	msgbox OrangeIslands_Kinnow_Massage_Text_AllDone
	release
	end

OrangeIslands_Kinnow_Massage_EventScript_Long::
	checkmoney 2000
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Kinnow_Massage_EventScript_NoMoney
	msgbox OrangeIslands_Kinnow_Massage_Text_WhichMon
	special ChoosePartyMon
	waitstate
	lock
	faceplayer
	goto_if_ge VAR_0x8004, PARTY_SIZE, OrangeIslands_Kinnow_Massage_EventScript_Declined
	specialvar VAR_RESULT, GetPartyMonSpecies
	goto_if_eq VAR_RESULT, SPECIES_EGG, OrangeIslands_Kinnow_Massage_EventScript_NotAnEgg
	removemoney 2000
	updatemoneybox
	msgbox OrangeIslands_Kinnow_Massage_Text_LieBack
	closemessage
	fadescreen FADE_TO_BLACK
	playfanfare MUS_HEAL
	waitfanfare
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	special DaisyMassageServices
	fadescreen FADE_FROM_BLACK
	hidemoneybox
	special BufferMonNickname
	msgbox OrangeIslands_Kinnow_Massage_Text_AllDone
	release
	end

OrangeIslands_Kinnow_Massage_EventScript_NoMoney::
	hidemoneybox
	msgbox OrangeIslands_Kinnow_Massage_Text_NoMoney
	release
	end

OrangeIslands_Kinnow_Massage_EventScript_Declined::
	hidemoneybox
	msgbox OrangeIslands_Kinnow_Massage_Text_Declined
	release
	end

OrangeIslands_Kinnow_Massage_EventScript_NotAnEgg::
	hidemoneybox
	msgbox OrangeIslands_Kinnow_Massage_Text_NotAnEgg
	release
	end

