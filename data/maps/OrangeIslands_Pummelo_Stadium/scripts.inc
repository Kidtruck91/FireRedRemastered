OrangeIslands_Pummelo_Stadium_MapScripts::
	.byte 0

OrangeIslands_Pummelo_Stadium_EventScript_Leave::
	warp MAP_ORANGE_ISLANDS_PUMMELO, 19, 18
	waitstate
	end

@ ---- Drake (orange_drake_battle.py) ----
@ No rules: a plain 6-on-6 single battle. Needs the four Orange badges.
@ After the Kanto Champion (FLAG_DEFEATED_CHAMP) he takes rematches any number of times.
OrangeIslands_Pummelo_Stadium_EventScript_Drake::
	lock
	faceplayer
	goto_if_defeated TRAINER_CHAMPION_DRAKE, OrangeIslands_Pummelo_Stadium_EventScript_DrakeAfter
	goto_if_unset FLAG_BADGE_CORAL_EYE, OrangeIslands_Pummelo_Stadium_EventScript_DrakeNoBadges
	goto_if_unset FLAG_BADGE_SPIKE_SHELL, OrangeIslands_Pummelo_Stadium_EventScript_DrakeNoBadges
	goto_if_unset FLAG_BADGE_SEA_RUBY, OrangeIslands_Pummelo_Stadium_EventScript_DrakeNoBadges
	goto_if_unset FLAG_BADGE_JADE_STAR, OrangeIslands_Pummelo_Stadium_EventScript_DrakeNoBadges
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeIntro
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeReady, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Pummelo_Stadium_EventScript_DrakeNotReady
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeGo
	trainerbattle_no_intro TRAINER_CHAMPION_DRAKE, OrangeIslands_Pummelo_Stadium_Text_DrakeDefeat
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeWon
	setflag FLAG_ORANGE_LEAGUE_CHAMPION
	message OrangeIslands_Pummelo_Stadium_Text_ReceivedTrophy
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeAfterTrophy
	goto OrangeIslands_Pummelo_Stadium_EventScript_DrakeGiveGSBall
	end

OrangeIslands_Pummelo_Stadium_EventScript_DrakeGiveGSBall::
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeGSBall
	giveitem ITEM_GS_BALL
	setflag FLAG_GOT_GS_BALL
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeGSBallFerry
	release
	end

OrangeIslands_Pummelo_Stadium_EventScript_DrakeNoBadges::
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeNoBadges
	release
	end

OrangeIslands_Pummelo_Stadium_EventScript_DrakeNotReady::
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeNotReady
	release
	end

OrangeIslands_Pummelo_Stadium_EventScript_DrakeAfter::
	setflag FLAG_ORANGE_LEAGUE_CHAMPION
	goto_if_unset FLAG_GOT_GS_BALL, OrangeIslands_Pummelo_Stadium_EventScript_DrakeGiveGSBall
	goto_if_unset FLAG_DEFEATED_CHAMP, OrangeIslands_Pummelo_Stadium_EventScript_DrakePostBattle
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeRematchAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, OrangeIslands_Pummelo_Stadium_EventScript_DrakeNoRematch
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeRematchGo
	cleartrainerflag TRAINER_CHAMPION_DRAKE_REMATCH
	trainerbattle_no_intro TRAINER_CHAMPION_DRAKE_REMATCH, OrangeIslands_Pummelo_Stadium_Text_DrakeRematchDefeat
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeRematchWon
	release
	end

OrangeIslands_Pummelo_Stadium_EventScript_DrakePostBattle::
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakePostBattle
	release
	end

OrangeIslands_Pummelo_Stadium_EventScript_DrakeNoRematch::
	msgbox OrangeIslands_Pummelo_Stadium_Text_DrakeNoRematch
	release
	end

OrangeIslands_Pummelo_Stadium_Text_DrakeNoBadges::
	.string "This is the ORANGE LEAGUE.\n"
	.string "Four badges gets you a battle.\p"
	.string "Come back when you've got\n"
	.string "all four.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeIntro::
	.string "So you're the one who beat\n"
	.string "the whole ORANGE CREW.\p"
	.string "I'm DRAKE. I run this league.\p"
	.string "CISSY had her rain. DANNY took\n"
	.string "your items. RUDY picked your\l"
	.string "team. LUANA made you pair up.\p"
	.string "Not here.\p"
	.string "No rules. No tricks.\n"
	.string "Six on six. Last one standing\l"
	.string "wins.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeReady::
	.string "Ready?$"

OrangeIslands_Pummelo_Stadium_Text_DrakeNotReady::
	.string "Then get ready.\n"
	.string "I'm not going anywhere.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeGo::
	.string "Good. Let's fight.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeDefeat::
	.string "...That's it.\n"
	.string "You beat me straight up.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeWon::
	.string "No excuses. That was the best\n"
	.string "fight I've had in years.\p"
	.string "You're the ORANGE LEAGUE\n"
	.string "CHAMPION. Take this.$"

OrangeIslands_Pummelo_Stadium_Text_ReceivedTrophy::
	.string "{PLAYER} received the\n"
	.string "WINNER'S TROPHY!$"

OrangeIslands_Pummelo_Stadium_Text_DrakeAfterTrophy::
	.string "Your name goes up on the wall\n"
	.string "with the others.\p"
	.string "The islands are yours now.\n"
	.string "No level limits, and you can\l"
	.string "bring any team you like.\p"
	.string "Any PC will let you switch\n"
	.string "between your KANTO and ORANGE\l"
	.string "boxes.$"

OrangeIslands_Pummelo_Stadium_Text_DrakePostBattle::
	.string "You won. That's all there is\n"
	.string "to it.\p"
	.string "Go win the INDIGO PLATEAU too.\n"
	.string "Then come back and we'll go\l"
	.string "again. No holding back.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeRematchAsk::
	.string "The INDIGO CHAMPION, huh?\n"
	.string "Then I'm done holding back.\p"
	.string "Another round?$"

OrangeIslands_Pummelo_Stadium_Text_DrakeNoRematch::
	.string "Suit yourself.\n"
	.string "I'll be here.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeRematchGo::
	.string "Full strength this time.\n"
	.string "Let's fight.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeRematchDefeat::
	.string "Ha! Still the champ.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeRematchWon::
	.string "Good fight. Come back any time.\n"
	.string "I'll be stronger.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeGSBall::
	.string "One more thing.\p"
	.string "This BALL washed up on PUMMELO a\n"
	.string "few years back. Nobody here can\l"
	.string "open it. Not even me.\p"
	.string "You take it.$"

OrangeIslands_Pummelo_Stadium_Text_DrakeGSBallFerry::
	.string "PROF. OAK might know what it is.\n"
	.string "Show it to him.\p"
	.string "The SEAGALLOP captain was waiting\n"
	.string "for it. His ship is back at\l"
	.string "VALENCIA, ready to take you home.$"
