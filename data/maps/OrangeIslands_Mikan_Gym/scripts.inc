.equ LOCALID_CISSY, 3

OrangeIslands_Mikan_Gym_MapScripts::
	.byte 0

OrangeIslands_Mikan_Gym_EventScript_Cissy::
	goto_if_defeated TRAINER_LEADER_CISSY, OrangeIslands_Mikan_Gym_EventScript_CissyAfter
	setweather WEATHER_RAIN
	doweather
	trainerbattle_single TRAINER_LEADER_CISSY, OrangeIslands_Mikan_Gym_Text_CissyIntro, OrangeIslands_Mikan_Gym_Text_CissyDefeat, OrangeIslands_Mikan_Gym_EventScript_CissyDefeated, NO_MUSIC
	end

OrangeIslands_Mikan_Gym_EventScript_CissyAfter::
	lock
	faceplayer
	goto_if_unset FLAG_GOT_TM18_FROM_CISSY, OrangeIslands_Mikan_Gym_EventScript_GiveTM18
	msgbox OrangeIslands_Mikan_Gym_Text_CissyPostBattle
	release
	end

OrangeIslands_Mikan_Gym_EventScript_CissyDefeated::
	resetweather
	doweather
	setflag FLAG_BADGE_CORAL_EYE
	message OrangeIslands_Mikan_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Mikan_Gym_Text_ExplainBadge
	goto OrangeIslands_Mikan_Gym_EventScript_GiveTM18
	end

OrangeIslands_Mikan_Gym_EventScript_GiveTM18::
	checkitemspace ITEM_TM18
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Mikan_Gym_EventScript_NoRoomForTM18
	giveitem_msg OrangeIslands_Mikan_Gym_Text_ReceivedTM18, ITEM_TM18
	setflag FLAG_GOT_TM18_FROM_CISSY
	msgbox OrangeIslands_Mikan_Gym_Text_ExplainTM18
	release
	end

OrangeIslands_Mikan_Gym_EventScript_NoRoomForTM18::
	msgbox OrangeIslands_Mikan_Gym_Text_NoRoom
	release
	end

OrangeIslands_Mikan_Gym_EventScript_Marco::
	trainerbattle_single TRAINER_SWIMMER_M_MARCO, OrangeIslands_Mikan_Gym_Text_MarcoIntro, OrangeIslands_Mikan_Gym_Text_MarcoDefeat
	msgbox OrangeIslands_Mikan_Gym_Text_MarcoPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Gym_EventScript_Coral::
	trainerbattle_single TRAINER_SWIMMER_F_CORAL, OrangeIslands_Mikan_Gym_Text_CoralIntro, OrangeIslands_Mikan_Gym_Text_CoralDefeat
	msgbox OrangeIslands_Mikan_Gym_Text_CoralPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_CORAL_EYE, OrangeIslands_Mikan_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Mikan_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Mikan_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Mikan_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Mikan_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE_CORAL_EYE, OrangeIslands_Mikan_Gym_EventScript_GymStatueAfter
	msgbox OrangeIslands_Mikan_Gym_Text_GymStatue
	releaseall
	end

OrangeIslands_Mikan_Gym_EventScript_GymStatueAfter::
	msgbox OrangeIslands_Mikan_Gym_Text_GymStatueAfter
	releaseall
	end

@ ---------------------------------------------------------------- text
OrangeIslands_Mikan_Gym_Text_CissyIntro::
	.string "So you're taking on the ORANGE\n"
	.string "LEAGUE? I'm CISSY, MIKAN's LEADER.\p"
	.string "Out here, we battle with the sea\n"
	.string "and the sky on our side.\p"
	.string "Let's make it rain!$"

OrangeIslands_Mikan_Gym_Text_CissyDefeat::
	.string "I couldn't land a hit on you\n"
	.string "when it counted!$"

OrangeIslands_Mikan_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the CORAL-EYE\n"
	.string "BADGE from CISSY!$"

OrangeIslands_Mikan_Gym_Text_ExplainBadge::
	.string "That's proof you beat me. The\n"
	.string "ORANGE CREW will raise your level\l"
	.string "cap now.\p"
	.string "Take this too. It's what makes\n"
	.string "my battles so slippery!$"

OrangeIslands_Mikan_Gym_Text_ReceivedTM18::
	.string "{PLAYER} received TM18\n"
	.string "from CISSY.$"

OrangeIslands_Mikan_Gym_Text_ExplainTM18::
	.string "TM18 is RAIN DANCE. It makes it\n"
	.string "rain for five turns.\p"
	.string "WATER moves hit harder, FIRE moves\n"
	.string "fizzle, and THUNDER never misses!$"

OrangeIslands_Mikan_Gym_Text_NoRoom::
	.string "Your BAG is full. Come back for\n"
	.string "this when you've made room.$"

OrangeIslands_Mikan_Gym_Text_CissyPostBattle::
	.string "The next gym is on NAVEL ISLAND,\n"
	.string "north of here. Good luck!$"

OrangeIslands_Mikan_Gym_Text_MarcoIntro::
	.string "Can you hit what you can't see?$"

OrangeIslands_Mikan_Gym_Text_MarcoDefeat::
	.string "Guess you can!$"

OrangeIslands_Mikan_Gym_Text_MarcoPostBattle::
	.string "CISSY's POKéMON are even harder\n"
	.string "to pin down than mine.$"

OrangeIslands_Mikan_Gym_Text_CoralIntro::
	.string "Rain or shine, I'm ready!$"

OrangeIslands_Mikan_Gym_Text_CoralDefeat::
	.string "I'm all washed up!$"

OrangeIslands_Mikan_Gym_Text_CoralPostBattle::
	.string "When CISSY makes it rain, her\n"
	.string "THUNDER never misses.$"

OrangeIslands_Mikan_Gym_Text_GymGuyAdvice::
	.string "Hey! Going for the CORAL-EYE\n"
	.string "BADGE?\p"
	.string "CISSY battles in the rain. Her\n"
	.string "WATER moves get stronger, and\l"
	.string "THUNDER can't miss.\p"
	.string "She also loves to cut your\n"
	.string "accuracy and dodge your attacks.\p"
	.string "GRASS types are a safe bet here!$"

OrangeIslands_Mikan_Gym_Text_GymGuyAfter::
	.string "You beat CISSY in her own rain!\n"
	.string "That was something!$"

OrangeIslands_Mikan_Gym_Text_GymStatue::
	.string "MIKAN ISLAND POKéMON GYM\n"
	.string "LEADER: CISSY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

OrangeIslands_Mikan_Gym_Text_GymStatueAfter::
	.string "MIKAN ISLAND POKéMON GYM\n"
	.string "LEADER: CISSY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"
