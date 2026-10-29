OrangeIslands_Sunburst_Cave_MapScripts::
	.byte 0

@ ---- Orange Islands trainers (orange_route_trainers.py) ----
OrangeIslands_Sunburst_Cave_EventScript_OrangeMark::
	trainerbattle_single TRAINER_ORANGE_HIKER_MARK, OrangeIslands_Sunburst_Cave_Text_OrangeMarkIntro, OrangeIslands_Sunburst_Cave_Text_OrangeMarkDefeat
	msgbox OrangeIslands_Sunburst_Cave_Text_OrangeMarkPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_Cave_EventScript_OrangeBrady::
	trainerbattle_single TRAINER_ORANGE_HIKER_BRADY, OrangeIslands_Sunburst_Cave_Text_OrangeBradyIntro, OrangeIslands_Sunburst_Cave_Text_OrangeBradyDefeat
	msgbox OrangeIslands_Sunburst_Cave_Text_OrangeBradyPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Sunburst_Cave_Text_OrangeMarkIntro::
	.string "Hey, kid! Test your team\n"
	.string "against my rocks!$"

OrangeIslands_Sunburst_Cave_Text_OrangeMarkDefeat::
	.string "Crumbled!$"

OrangeIslands_Sunburst_Cave_Text_OrangeMarkPostBattle::
	.string "Good boots are a hiker's\n"
	.string "best friend.$"

OrangeIslands_Sunburst_Cave_Text_OrangeBradyIntro::
	.string "Hey, kid! Test your team\n"
	.string "against my rocks!$"

OrangeIslands_Sunburst_Cave_Text_OrangeBradyDefeat::
	.string "Crumbled!$"

OrangeIslands_Sunburst_Cave_Text_OrangeBradyPostBattle::
	.string "Good boots are a hiker's\n"
	.string "best friend.$"

@ ---- HM06 backstop (rocksmash_backstop.py) ----
@ The only other ROCK SMASH in the game is an optional NPC in EMBER SPA that
@ nothing sends you to. This one is deliberately on the way to DANNY's gym.
OrangeIslands_Sunburst_Cave_EventScript_RockSmashHiker::
	lock
	faceplayer
	goto_if_set FLAG_GOT_HM06, OrangeIslands_Sunburst_Cave_EventScript_RockSmashHikerAfter
	msgbox OrangeIslands_Sunburst_Cave_Text_RockSmashOffer
	giveitem ITEM_HM06
	setflag FLAG_GOT_HM06
	msgbox OrangeIslands_Sunburst_Cave_Text_RockSmashExplain
	release
	end

OrangeIslands_Sunburst_Cave_EventScript_RockSmashHikerAfter::
	msgbox OrangeIslands_Sunburst_Cave_Text_RockSmashAlready
	release
	end

OrangeIslands_Sunburst_Cave_Text_RockSmashOffer::
	.string "Oh, you came all the way up\n"
	.string "here? Then you've earned this.\p"
	.string "I cut this whole cave with it.\n"
	.string "Keep it. I know it by heart.$"

OrangeIslands_Sunburst_Cave_Text_RockSmashExplain::
	.string "HM06 is ROCK SMASH. Teach it to\n"
	.string "anything with arms.\p"
	.string "Cracks any rock small enough to\n"
	.string "pick up. The big ones still need\l"
	.string "STRENGTH.$"

OrangeIslands_Sunburst_Cave_Text_RockSmashAlready::
	.string "You've already got ROCK SMASH?\n"
	.string "Good. You'll want it on NAVEL.$"
