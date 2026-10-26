.equ LOCALID_CISSY, 1
.equ LOCALID_MARCO, 2
.equ LOCALID_CORAL, 3

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
	msgbox OrangeIslands_Mikan_Gym_Text_MarcoTalk, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Mikan_Gym_EventScript_Coral::
	trainerbattle_single TRAINER_SWIMMER_F_CORAL, OrangeIslands_Mikan_Gym_Text_CoralIntro, OrangeIslands_Mikan_Gym_Text_CoralDefeat
	msgbox OrangeIslands_Mikan_Gym_Text_CoralTalk, MSGBOX_AUTOCLOSE
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
	.string "So you found the way up. Not many\n"
	.string "do it on the first try.\p"
	.string "I'm CISSY, MIKAN's LEADER. Out\n"
	.string "here we battle with the sea and\l"
	.string "the sky on our side.\p"
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
	.string "When you want out, swim to the far\n"
	.string "end of the pool. The outflow runs\l"
	.string "back down to the dock.\p"
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
	.string "Heading out? Swim to the far end\n"
	.string "of my pool. The outflow will take\l"
	.string "you back down.\p"
	.string "The next gym is on NAVEL ISLAND,\n"
	.string "north of here. Good luck!$"

OrangeIslands_Mikan_Gym_Text_MarcoIntro::
	.string "MARCO: Third rock! Nobody reaches\n"
	.string "my rock without a battle!$"

OrangeIslands_Mikan_Gym_Text_MarcoDefeat::
	.string "Gah! Washed out!$"

OrangeIslands_Mikan_Gym_Text_MarcoTalk::
	.string "One climb left. Pick your lane at\n"
	.string "the top and count to the middle.\p"
	.string "Only one of them goes up to\n"
	.string "CISSY.$"

OrangeIslands_Mikan_Gym_Text_CoralIntro::
	.string "CORAL: So the second channel let\n"
	.string "you through? Rain or shine, you\l"
	.string "still have to get past me!$"

OrangeIslands_Mikan_Gym_Text_CoralDefeat::
	.string "I'm all washed up!$"

OrangeIslands_Mikan_Gym_Text_CoralTalk::
	.string "The next channel runs west. Count\n"
	.string "in from that wall, not this one.\p"
	.string "And when CISSY makes it rain, her\n"
	.string "THUNDER never misses.$"

OrangeIslands_Mikan_Gym_Text_GymGuyAdvice::
	.string "Hey! Going for the CORAL-EYE\n"
	.string "BADGE?\p"
	.string "You can't walk to CISSY. You SURF,\n"
	.string "and the pool does the steering.\p"
	.string "Three rocks stand between you and\n"
	.string "her, and each one is behind its\l"
	.string "own channel.\p"
	.string "The bottom is a maze. Ride it\n"
	.string "round and it sets you on the\l"
	.string "first rock.\p"
	.string "After that it's the same trick\n"
	.string "three times. Swim out along the\l"
	.string "open water, pick where you cross,\l"
	.string "and only one column carries you\l"
	.string "up to the next rock.\p"
	.string "Pick wrong and the outflow takes\n"
	.string "you all the way back down here!\p"
	.string "Oh, and CISSY battles in the rain.\n"
	.string "GRASS types are a safe bet!$"

OrangeIslands_Mikan_Gym_Text_GymGuyAfter::
	.string "You read the current and beat\n"
	.string "CISSY in her own rain!$"

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
