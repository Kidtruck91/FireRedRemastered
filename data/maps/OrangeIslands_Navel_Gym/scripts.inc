.equ LOCALID_DANNY, 1
.equ LOCALID_KAI,   2
.equ LOCALID_SORA,  3
.equ LOCALID_YUKI,  4

OrangeIslands_Navel_Gym_MapScripts::
	.byte 0

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
	.string "Yo! DANNY's gym is a marathon!\p"
	.string "You fight his crew and then him,\n"
	.string "back to back, with no items.\p"
	.string "Bring six POKéMON at full health,\n"
	.string "and watch out for ROCK, FIGHTING\l"
	.string "and ICE types!$"

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
