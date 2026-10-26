.equ LOCALID_DANNY,           1
.equ LOCALID_KAI,             2
.equ LOCALID_SORA,            3
.equ LOCALID_YUKI,            4
.equ LOCALID_GUIDE,           5
.equ LOCALID_BOULDER_LEFT,    6
.equ LOCALID_BOULDER_CENTRE,  7
.equ LOCALID_BOULDER_RIGHT,   8

OrangeIslands_Navel_Gym_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, OrangeIslands_Navel_Gym_OnLoad
	.byte 0

@ The rockfall across the gate is drawn in, not stored in the map, so the open
@ state is what the layout holds and the three plate flags decide the rest.
OrangeIslands_Navel_Gym_OnLoad::
	goto_if_unset FLAG_NAVEL_GYM_PLATE_LEFT,   OrangeIslands_Navel_Gym_EventScript_DrawRockfall
	goto_if_unset FLAG_NAVEL_GYM_PLATE_CENTRE, OrangeIslands_Navel_Gym_EventScript_DrawRockfall
	goto_if_unset FLAG_NAVEL_GYM_PLATE_RIGHT,  OrangeIslands_Navel_Gym_EventScript_DrawRockfall
	end

OrangeIslands_Navel_Gym_EventScript_DrawRockfall::
	setmetatile  9, 7, METATILE_Cave_RockBarrier_Top, 1
	setmetatile 10, 7, METATILE_Cave_RockBarrier_Top, 1
	setmetatile  9, 8, METATILE_Cave_RockBarrier_Bottom, 1
	setmetatile 10, 8, METATILE_Cave_RockBarrier_Bottom, 1
	end

@ Each plate is a coord event on an MB_STRENGTH_BUTTON tile. Its var_value is 99,
@ a value the var never holds, so walking onto the plate does nothing -- the only
@ thing that fires it is HandleBoulderActivateVictoryRoadSwitch, which matches
@ coord events by position alone when a boulder comes to rest. Each press copies
@ its boulder's position into the save so it stays put.
OrangeIslands_Navel_Gym_EventScript_PlateLeft::
	lockall
	goto_if_set FLAG_NAVEL_GYM_PLATE_LEFT, OrangeIslands_Navel_Gym_EventScript_PlateIdle
	setflag FLAG_NAVEL_GYM_PLATE_LEFT
	copyobjectxytoperm LOCALID_BOULDER_LEFT
	goto OrangeIslands_Navel_Gym_EventScript_PlatePressed

OrangeIslands_Navel_Gym_EventScript_PlateCentre::
	lockall
	goto_if_set FLAG_NAVEL_GYM_PLATE_CENTRE, OrangeIslands_Navel_Gym_EventScript_PlateIdle
	setflag FLAG_NAVEL_GYM_PLATE_CENTRE
	copyobjectxytoperm LOCALID_BOULDER_CENTRE
	goto OrangeIslands_Navel_Gym_EventScript_PlatePressed

OrangeIslands_Navel_Gym_EventScript_PlateRight::
	lockall
	goto_if_set FLAG_NAVEL_GYM_PLATE_RIGHT, OrangeIslands_Navel_Gym_EventScript_PlateIdle
	setflag FLAG_NAVEL_GYM_PLATE_RIGHT
	copyobjectxytoperm LOCALID_BOULDER_RIGHT
	goto OrangeIslands_Navel_Gym_EventScript_PlatePressed

OrangeIslands_Navel_Gym_EventScript_PlatePressed::
	playse SE_SWITCH
	waitse
	goto_if_unset FLAG_NAVEL_GYM_PLATE_LEFT,   OrangeIslands_Navel_Gym_EventScript_PlateHolds
	goto_if_unset FLAG_NAVEL_GYM_PLATE_CENTRE, OrangeIslands_Navel_Gym_EventScript_PlateHolds
	goto_if_unset FLAG_NAVEL_GYM_PLATE_RIGHT,  OrangeIslands_Navel_Gym_EventScript_PlateHolds
	setmetatile  9, 7, METATILE_Cave_Floor, 0
	setmetatile 10, 7, METATILE_Cave_Floor, 0
	setmetatile  9, 8, METATILE_Cave_Floor, 0
	setmetatile 10, 8, METATILE_Cave_Floor, 0
	playse SE_ICE_BREAK
	special DrawWholeMapView
	waitse
	msgbox OrangeIslands_Navel_Gym_Text_GateOpens
	releaseall
	end

OrangeIslands_Navel_Gym_EventScript_PlateHolds::
	msgbox OrangeIslands_Navel_Gym_Text_PlateHolds
	releaseall
	end

OrangeIslands_Navel_Gym_EventScript_PlateIdle::
	releaseall
	end

@ Danny's gauntlet: his crew, then Danny, back to back. No healing, no items.
OrangeIslands_Navel_Gym_EventScript_Danny::
	lock
	faceplayer
	goto_if_defeated TRAINER_LEADER_DANNY, OrangeIslands_Navel_Gym_EventScript_DannyAfter
	msgbox OrangeIslands_Navel_Gym_Text_DannyChallenge, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Navel_Gym_EventScript_DannyDeclined
	cleartrainerflag TRAINER_HIKER_NAVEL_KAI
	cleartrainerflag TRAINER_BLACK_BELT_NAVEL_SORA
	cleartrainerflag TRAINER_COOLTRAINER_NAVEL_YUKI
	setflag FLAG_NO_BATTLE_ITEMS
	msgbox OrangeIslands_Navel_Gym_Text_DannyGo
	closemessage
	applymovement LOCALID_KAI, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_KaiIntro
	trainerbattle_no_intro TRAINER_HIKER_NAVEL_KAI, OrangeIslands_Navel_Gym_Text_KaiDefeat
	applymovement LOCALID_SORA, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_SoraIntro
	trainerbattle_no_intro TRAINER_BLACK_BELT_NAVEL_SORA, OrangeIslands_Navel_Gym_Text_SoraDefeat
	applymovement LOCALID_YUKI, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_YukiIntro
	trainerbattle_no_intro TRAINER_COOLTRAINER_NAVEL_YUKI, OrangeIslands_Navel_Gym_Text_YukiDefeat
	applymovement LOCALID_DANNY, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OrangeIslands_Navel_Gym_Text_DannyIntro
	trainerbattle_no_intro TRAINER_LEADER_DANNY, OrangeIslands_Navel_Gym_Text_DannyDefeat
	clearflag FLAG_NO_BATTLE_ITEMS
	setflag FLAG_BADGE_SPIKE_SHELL
	message OrangeIslands_Navel_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Navel_Gym_Text_ExplainBadge
	goto OrangeIslands_Navel_Gym_EventScript_GiveTM07
	end

OrangeIslands_Navel_Gym_EventScript_DannyDeclined::
	msgbox OrangeIslands_Navel_Gym_Text_DannyDeclined
	release
	end

OrangeIslands_Navel_Gym_EventScript_DannyAfter::
	goto_if_unset FLAG_GOT_TM07_FROM_DANNY, OrangeIslands_Navel_Gym_EventScript_GiveTM07
	msgbox OrangeIslands_Navel_Gym_Text_DannyPostBattle
	release
	end

OrangeIslands_Navel_Gym_EventScript_GiveTM07::
	checkitemspace ITEM_TM07
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Navel_Gym_EventScript_NoRoom
	giveitem_msg OrangeIslands_Navel_Gym_Text_ReceivedTM07, ITEM_TM07
	setflag FLAG_GOT_TM07_FROM_DANNY
	msgbox OrangeIslands_Navel_Gym_Text_ExplainTM07
	release
	end

OrangeIslands_Navel_Gym_EventScript_NoRoom::
	msgbox OrangeIslands_Navel_Gym_Text_NoRoom
	release
	end

OrangeIslands_Navel_Gym_EventScript_Kai::
	msgbox OrangeIslands_Navel_Gym_Text_KaiTalk, MSGBOX_NPC
	end

OrangeIslands_Navel_Gym_EventScript_Sora::
	msgbox OrangeIslands_Navel_Gym_Text_SoraTalk, MSGBOX_NPC
	end

OrangeIslands_Navel_Gym_EventScript_Yuki::
	msgbox OrangeIslands_Navel_Gym_Text_YukiTalk, MSGBOX_NPC
	end

OrangeIslands_Navel_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_SPIKE_SHELL, OrangeIslands_Navel_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Navel_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Navel_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Navel_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Navel_Gym_EventScript_GymStatue::
	lockall
	goto_if_set FLAG_BADGE_SPIKE_SHELL, OrangeIslands_Navel_Gym_EventScript_GymStatueAfter
	msgbox OrangeIslands_Navel_Gym_Text_GymStatue
	releaseall
	end

OrangeIslands_Navel_Gym_EventScript_GymStatueAfter::
	msgbox OrangeIslands_Navel_Gym_Text_GymStatueAfter
	releaseall
	end

OrangeIslands_Navel_Gym_Text_DannyChallenge::
	.string "I'm DANNY, NAVEL's LEADER.\p"
	.string "My challenge is simple. Beat my\n"
	.string "crew, all three, then me. One\l"
	.string "after another, no breaks.\p"
	.string "No healing in between, and no\n"
	.string "items in battle. Lose once and\l"
	.string "you start over.\p"
	.string "Ready to climb?$"

OrangeIslands_Navel_Gym_Text_DannyDeclined::
	.string "Heal up and come back when you're\n"
	.string "ready. The summit isn't going\l"
	.string "anywhere.$"

OrangeIslands_Navel_Gym_Text_DannyGo::
	.string "Crew! We've got a challenger!$"

OrangeIslands_Navel_Gym_Text_KaiIntro::
	.string "KAI: I'm first! Let's see if you\n"
	.string "can make it past the foothills!$"

OrangeIslands_Navel_Gym_Text_KaiDefeat::
	.string "You're tougher than this\n"
	.string "mountain!$"

OrangeIslands_Navel_Gym_Text_SoraIntro::
	.string "SORA: Don't catch your breath!\n"
	.string "I'm next!$"

OrangeIslands_Navel_Gym_Text_SoraDefeat::
	.string "Hyah! You win!$"

OrangeIslands_Navel_Gym_Text_YukiIntro::
	.string "YUKI: The summit is cold.\n"
	.string "Can your team take the chill?$"

OrangeIslands_Navel_Gym_Text_YukiDefeat::
	.string "Brr... You melted right\n"
	.string "through us.$"

OrangeIslands_Navel_Gym_Text_DannyIntro::
	.string "You beat my whole crew without\n"
	.string "a rest. Now it's my turn!$"

OrangeIslands_Navel_Gym_Text_DannyDefeat::
	.string "You made it to the top!$"

OrangeIslands_Navel_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "SPIKE SHELL BADGE from DANNY!$"

OrangeIslands_Navel_Gym_Text_ExplainBadge::
	.string "That's the SPIKE SHELL BADGE.\n"
	.string "The ORANGE CREW will raise your\l"
	.string "level cap now.\p"
	.string "And take this. It's a souvenir\n"
	.string "from the top of NAVEL.$"

OrangeIslands_Navel_Gym_Text_ReceivedTM07::
	.string "{PLAYER} received TM07\n"
	.string "from DANNY.$"

OrangeIslands_Navel_Gym_Text_ExplainTM07::
	.string "TM07 is HAIL. It hurts every\n"
	.string "POKéMON that isn't ICE type\l"
	.string "for five turns.$"

OrangeIslands_Navel_Gym_Text_NoRoom::
	.string "Your BAG is full. Make room and\n"
	.string "come back for this.$"

OrangeIslands_Navel_Gym_Text_DannyPostBattle::
	.string "TROVITA ISLAND is next, along\n"
	.string "ROUTE 11 from NAVEL BEACH.$"

OrangeIslands_Navel_Gym_Text_KaiTalk::
	.string "Talk to DANNY when you're ready.\n"
	.string "Then we come at you one by one!$"

OrangeIslands_Navel_Gym_Text_SoraTalk::
	.string "Bring a full team. You won't get\n"
	.string "to heal between us.$"

OrangeIslands_Navel_Gym_Text_YukiTalk::
	.string "I'm last before DANNY. My team\n"
	.string "loves the cold.$"

OrangeIslands_Navel_Gym_Text_GymGuyAdvice::
	.string "Yo! DANNY doesn't take challengers\n"
	.string "who can't shift rock.\p"
	.string "Three plates in the floor hold the\n"
	.string "slide shut, and a POKéMON isn't\l"
	.string "heavy enough to work one. You need\l"
	.string "a boulder sitting on each.\p"
	.string "You'll want STRENGTH to push them\n"
	.string "and ROCK SMASH for what's in the\l"
	.string "way.\p"
	.string "Shove a boulder somewhere useless\n"
	.string "and don't panic. Step outside and\l"
	.string "the rocks settle back where they\l"
	.string "started.\p"
	.string "Past the gate it's a marathon. His\n"
	.string "crew and then him, back to back,\l"
	.string "with no items. Watch out for ROCK,\l"
	.string "FIGHTING and ICE types!$"

OrangeIslands_Navel_Gym_Text_PlateHolds::
	.string "The plate sinks under the boulder,\n"
	.string "and something heavy shifts deep in\l"
	.string "the rock.\p"
	.string "The slide still holds.$"

OrangeIslands_Navel_Gym_Text_GateOpens::
	.string "The last plate goes down.\p"
	.string "The slide across the gate gives\n"
	.string "way and slumps aside. The way up\l"
	.string "to DANNY is open!$"

OrangeIslands_Navel_Gym_Text_GymGuyAfter::
	.string "You made it through the whole\n"
	.string "gauntlet! Incredible!$"

OrangeIslands_Navel_Gym_Text_GymStatue::
	.string "NAVEL ISLAND POKéMON GYM\n"
	.string "LEADER: DANNY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}$"

OrangeIslands_Navel_Gym_Text_GymStatueAfter::
	.string "NAVEL ISLAND POKéMON GYM\n"
	.string "LEADER: DANNY\p"
	.string "WINNING TRAINERS:\n"
	.string "{RIVAL}, {PLAYER}$"
