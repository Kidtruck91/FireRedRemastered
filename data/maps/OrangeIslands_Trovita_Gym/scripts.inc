.equ LOCALID_RUDY, 1

OrangeIslands_Trovita_Gym_MapScripts::
	.byte 0

@ Type triad: the player battles with one FIRE, one WATER and one GRASS POKéMON.
OrangeIslands_Trovita_Gym_EventScript_Rudy::
	lock
	faceplayer
	goto_if_defeated TRAINER_LEADER_RUDY, OrangeIslands_Trovita_Gym_EventScript_RudyAfter
	msgbox OrangeIslands_Trovita_Gym_Text_RudyChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Trovita_Gym_EventScript_RudyDeclined
	msgbox OrangeIslands_Trovita_Gym_Text_ChooseThree
	closemessage
	setvar VAR_0x8005, TYPE_FIRE
	setvar VAR_0x8006, TYPE_WATER
	setvar VAR_0x8007, TYPE_GRASS
	callnative OrangeGym_ChooseTypeTriad
	waitstate
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Trovita_Gym_EventScript_RudyDeclined
	msgbox OrangeIslands_Trovita_Gym_Text_RudyIntro
	trainerbattle_no_intro TRAINER_LEADER_RUDY, OrangeIslands_Trovita_Gym_Text_RudyDefeat
	setflag FLAG_BADGE_SEA_RUBY
	message OrangeIslands_Trovita_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Trovita_Gym_Text_ExplainBadge
	goto OrangeIslands_Trovita_Gym_EventScript_GiveTM10
	end

OrangeIslands_Trovita_Gym_EventScript_RudyDeclined::
	msgbox OrangeIslands_Trovita_Gym_Text_RudyDeclined
	release
	end

OrangeIslands_Trovita_Gym_EventScript_RudyAfter::
	goto_if_unset FLAG_GOT_TM10_FROM_RUDY, OrangeIslands_Trovita_Gym_EventScript_GiveTM10
	msgbox OrangeIslands_Trovita_Gym_Text_RudyPostBattle
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GiveTM10::
	checkitemspace ITEM_TM10
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Trovita_Gym_EventScript_NoRoom
	giveitem_msg OrangeIslands_Trovita_Gym_Text_ReceivedTM10, ITEM_TM10
	setflag FLAG_GOT_TM10_FROM_RUDY
	msgbox OrangeIslands_Trovita_Gym_Text_ExplainTM10
	release
	end

OrangeIslands_Trovita_Gym_EventScript_NoRoom::
	msgbox OrangeIslands_Trovita_Gym_Text_NoRoom
	release
	end

OrangeIslands_Trovita_Gym_EventScript_Lime::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_LIME, OrangeIslands_Trovita_Gym_Text_LimeIntro, OrangeIslands_Trovita_Gym_Text_LimeDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_LimePostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Gym_EventScript_Rowan::
	trainerbattle_single TRAINER_COOLTRAINER_TROVITA_ROWAN, OrangeIslands_Trovita_Gym_Text_RowanIntro, OrangeIslands_Trovita_Gym_Text_RowanDefeat
	msgbox OrangeIslands_Trovita_Gym_Text_RowanPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Trovita_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_SEA_RUBY, OrangeIslands_Trovita_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Trovita_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Trovita_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Trovita_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE_SEA_RUBY, OrangeIslands_Trovita_Gym_EventScript_GymStatueAfter
	msgbox OrangeIslands_Trovita_Gym_Text_GymStatue
	releaseall
	end

OrangeIslands_Trovita_Gym_EventScript_GymStatueAfter::
	msgbox OrangeIslands_Trovita_Gym_Text_GymStatueAfter
	releaseall
	end

OrangeIslands_Trovita_Gym_Text_RudyChallenge::
	.string "Welcome to TROVITA. I'm RUDY.\p"
	.string "A good trainer balances FIRE,\n"
	.string "WATER and GRASS, each covering\l"
	.string "the others.\p"
	.string "So here, you battle me with one\n"
	.string "of each. Just three POKéMON.\p"
	.string "Shall we?$"

OrangeIslands_Trovita_Gym_Text_ChooseThree::
	.string "Pick one FIRE, one WATER and one\n"
	.string "GRASS POKéMON.$"

OrangeIslands_Trovita_Gym_Text_RudyDeclined::
	.string "Come back once you have a FIRE,\n"
	.string "a WATER and a GRASS POKéMON.$"

OrangeIslands_Trovita_Gym_Text_RudyIntro::
	.string "A fine triad. Let's see how\n"
	.string "well it holds together!$"

OrangeIslands_Trovita_Gym_Text_RudyDefeat::
	.string "Perfectly balanced. You've\n"
	.string "earned this.$"

OrangeIslands_Trovita_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "SEA RUBY BADGE from RUDY!$"

OrangeIslands_Trovita_Gym_Text_ExplainBadge::
	.string "The SEA RUBY BADGE. The ORANGE\n"
	.string "CREW will raise your level cap.\p"
	.string "Please, take this as well.$"

OrangeIslands_Trovita_Gym_Text_ReceivedTM10::
	.string "{PLAYER} received TM10\n"
	.string "from RUDY.$"

OrangeIslands_Trovita_Gym_Text_ExplainTM10::
	.string "TM10 is HIDDEN POWER. Its type\n"
	.string "is different for every POKéMON.\p"
	.string "A good way to cover a gap in\n"
	.string "your triad!$"

OrangeIslands_Trovita_Gym_Text_NoRoom::
	.string "Your BAG is full. Come back\n"
	.string "once you've made room.$"

OrangeIslands_Trovita_Gym_Text_RudyPostBattle::
	.string "The last gym is on KUMQUAT\n"
	.string "ISLAND. Good luck!$"

OrangeIslands_Trovita_Gym_Text_LimeIntro::
	.string "FIRE, WATER, GRASS! Can you\n"
	.string "handle all three?$"

OrangeIslands_Trovita_Gym_Text_LimeDefeat::
	.string "My triad fell apart!$"

OrangeIslands_Trovita_Gym_Text_LimePostBattle::
	.string "RUDY only lets you use three\n"
	.string "POKéMON. Pick them well!$"

OrangeIslands_Trovita_Gym_Text_RowanIntro::
	.string "Show me your balance!$"

OrangeIslands_Trovita_Gym_Text_RowanDefeat::
	.string "Out of balance...$"

OrangeIslands_Trovita_Gym_Text_RowanPostBattle::
	.string "RUDY's team doesn't follow the\n"
	.string "triad. Watch for ELECTRIC and\l"
	.string "PSYCHIC moves!$"

OrangeIslands_Trovita_Gym_Text_GymGuyAdvice::
	.string "Yo! RUDY's rule: you battle him\n"
	.string "with one FIRE, one WATER and one\l"
	.string "GRASS POKéMON, nothing else.\p"
	.string "His own team is a mix, so think\n"
	.string "about which of your three\l"
	.string "covers what!$"

OrangeIslands_Trovita_Gym_Text_GymGuyAfter::
	.string "Three POKéMON against RUDY, and\n"
	.string "you won! Nicely done!$"

OrangeIslands_Trovita_Gym_Text_GymStatue::
	.string "TROVITA ISLAND POKéMON GYM\n"
	.string "LEADER: RUDY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

OrangeIslands_Trovita_Gym_Text_GymStatueAfter::
	.string "TROVITA ISLAND POKéMON GYM\n"
	.string "LEADER: RUDY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"
