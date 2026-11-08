.equ LOCALID_LUANA, 1

OrangeIslands_Kumquat_Gym_MapScripts::
	.byte 0

@ Every battle here is a double battle.
OrangeIslands_Kumquat_Gym_EventScript_Luana::
	goto_if_defeated TRAINER_LEADER_LUANA, OrangeIslands_Kumquat_Gym_EventScript_LuanaAfter
	trainerbattle_double TRAINER_LEADER_LUANA, OrangeIslands_Kumquat_Gym_Text_LuanaIntro, OrangeIslands_Kumquat_Gym_Text_LuanaDefeat, OrangeIslands_Kumquat_Gym_Text_LuanaNotEnough, OrangeIslands_Kumquat_Gym_EventScript_LuanaDefeated, NO_MUSIC
	end

OrangeIslands_Kumquat_Gym_EventScript_LuanaDefeated::
	setflag FLAG_BADGE_JADE_STAR
	message OrangeIslands_Kumquat_Gym_Text_ReceivedBadge
	waitmessage
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	msgbox OrangeIslands_Kumquat_Gym_Text_ExplainBadge
	goto OrangeIslands_Kumquat_Gym_EventScript_GiveTM33
	end

OrangeIslands_Kumquat_Gym_EventScript_LuanaAfter::
	lock
	faceplayer
	goto_if_unset FLAG_GOT_TM33_FROM_LUANA, OrangeIslands_Kumquat_Gym_EventScript_GiveTM33
	msgbox OrangeIslands_Kumquat_Gym_Text_LuanaPostBattle
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_GiveTM33::
	checkitemspace ITEM_TM33
	goto_if_eq VAR_RESULT, FALSE, OrangeIslands_Kumquat_Gym_EventScript_NoRoom
	giveitem_msg OrangeIslands_Kumquat_Gym_Text_ReceivedTM33, ITEM_TM33
	setflag FLAG_GOT_TM33_FROM_LUANA
	msgbox OrangeIslands_Kumquat_Gym_Text_ExplainTM33
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_NoRoom::
	msgbox OrangeIslands_Kumquat_Gym_Text_NoRoom
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_Ana::
	trainerbattle_double TRAINER_TWINS_KUMQUAT_ANA_BEL, OrangeIslands_Kumquat_Gym_Text_AnaIntro, OrangeIslands_Kumquat_Gym_Text_AnaDefeat, OrangeIslands_Kumquat_Gym_Text_TwinsNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_AnaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_Bel::
	trainerbattle_double TRAINER_TWINS_KUMQUAT_ANA_BEL, OrangeIslands_Kumquat_Gym_Text_BelIntro, OrangeIslands_Kumquat_Gym_Text_BelDefeat, OrangeIslands_Kumquat_Gym_Text_TwinsNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_BelPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_Lex::
	trainerbattle_double TRAINER_COOL_COUPLE_KUMQUAT_LEX_NYA, OrangeIslands_Kumquat_Gym_Text_LexIntro, OrangeIslands_Kumquat_Gym_Text_LexDefeat, OrangeIslands_Kumquat_Gym_Text_CoupleNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_LexPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_Nya::
	trainerbattle_double TRAINER_COOL_COUPLE_KUMQUAT_LEX_NYA, OrangeIslands_Kumquat_Gym_Text_NyaIntro, OrangeIslands_Kumquat_Gym_Text_NyaDefeat, OrangeIslands_Kumquat_Gym_Text_CoupleNotEnough
	msgbox OrangeIslands_Kumquat_Gym_Text_NyaPostBattle, MSGBOX_AUTOCLOSE
	end

OrangeIslands_Kumquat_Gym_EventScript_GymGuy::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_JADE_STAR, OrangeIslands_Kumquat_Gym_EventScript_GymGuyAfter
	msgbox OrangeIslands_Kumquat_Gym_Text_GymGuyAdvice
	release
	end

OrangeIslands_Kumquat_Gym_EventScript_GymGuyAfter::
	msgbox OrangeIslands_Kumquat_Gym_Text_GymGuyAfter
	release
	end

OrangeIslands_Kumquat_Gym_Text_LuanaIntro::
	.string "Welcome to KUMQUAT. I'm LUANA.\p"
	.string "Here, POKéMON battle two at a\n"
	.string "time. Partners have to work\l"
	.string "together, not just hit hard.\p"
	.string "Show me your best pair!$"

OrangeIslands_Kumquat_Gym_Text_LuanaDefeat::
	.string "Wonderful teamwork. You've\n"
	.string "earned the last badge.$"

OrangeIslands_Kumquat_Gym_Text_LuanaNotEnough::
	.string "Oh, you need at least two\n"
	.string "POKéMON for a battle here.$"

OrangeIslands_Kumquat_Gym_Text_ReceivedBadge::
	.string "{PLAYER} received the\n"
	.string "JADE STAR BADGE from LUANA!$"

OrangeIslands_Kumquat_Gym_Text_ExplainBadge::
	.string "With four badges, you can take a\n"
	.string "ferry to PUMMELO ISLAND and face\l"
	.string "the head of the ORANGE CREW.\p"
	.string "Here, this will help your pairs.$"

OrangeIslands_Kumquat_Gym_Text_ReceivedTM33::
	.string "{PLAYER} received TM33\n"
	.string "from LUANA.$"

OrangeIslands_Kumquat_Gym_Text_ExplainTM33::
	.string "TM33 is REFLECT. It halves\n"
	.string "physical damage to your side\l"
	.string "for five turns.$"

OrangeIslands_Kumquat_Gym_Text_NoRoom::
	.string "Your BAG is full. Come back\n"
	.string "once you've made room.$"

OrangeIslands_Kumquat_Gym_Text_LuanaPostBattle::
	.string "DRAKE is waiting on PUMMELO\n"
	.string "ISLAND. Give him my best!$"

OrangeIslands_Kumquat_Gym_Text_AnaIntro::
	.string "ANA: Our DUGTRIO doesn't aim.\n"
	.string "It just shakes the whole floor!$"

OrangeIslands_Kumquat_Gym_Text_BelIntro::
	.string "BEL: And my DODRIO is up in the\n"
	.string "air where it can't be shaken!$"

OrangeIslands_Kumquat_Gym_Text_AnaDefeat::
	.string "ANA: We lost together!$"

OrangeIslands_Kumquat_Gym_Text_BelDefeat::
	.string "BEL: Together, we lost!$"

OrangeIslands_Kumquat_Gym_Text_TwinsNotEnough::
	.string "We battle two at a time! Come\n"
	.string "back with more POKéMON!$"

OrangeIslands_Kumquat_Gym_Text_AnaPostBattle::
	.string "ANA: EARTHQUAKE hits everything\n"
	.string "standing on the ground. Ours\l"
	.string "wasn't standing on it.$"

OrangeIslands_Kumquat_Gym_Text_BelPostBattle::
	.string "BEL: ARENA TRAP meant you\n"
	.string "couldn't swap out of it either.\p"
	.string "Find a partner that doesn't\n"
	.string "care, and you can swing wide\l"
	.string "every turn!$"

OrangeIslands_Kumquat_Gym_Text_LexIntro::
	.string "LEX: RAPIDASH, bring the sun out!$"

OrangeIslands_Kumquat_Gym_Text_NyaIntro::
	.string "NYA: Once it's shining, my\n"
	.string "VILEPLUME doesn't wait around.$"

OrangeIslands_Kumquat_Gym_Text_LexDefeat::
	.string "LEX: Out of sync...$"

OrangeIslands_Kumquat_Gym_Text_NyaDefeat::
	.string "NYA: We'll practice more.$"

OrangeIslands_Kumquat_Gym_Text_CoupleNotEnough::
	.string "A double battle needs two\n"
	.string "POKéMON on your side!$"

OrangeIslands_Kumquat_Gym_Text_LexPostBattle::
	.string "LEX: One SUNNY DAY did three\n"
	.string "jobs. Bigger FIRE BLAST, no\l"
	.string "charge turn on SOLARBEAM...$"

OrangeIslands_Kumquat_Gym_Text_NyaPostBattle::
	.string "NYA: ...and CHLOROPHYLL doubled\n"
	.string "VILEPLUME's speed.\p"
	.string "LUANA never uses EARTHQUAKE.\n"
	.string "She won't hit her own side.$"

OrangeIslands_Kumquat_Gym_Text_GymGuyAdvice::
	.string "Yo! Here's how LUANA rolls.\p"
	.string "Every battle in her gym is a\n"
	.string "double battle, and she picks\l"
	.string "pairs that work together.\p"
	.string "Not two strong POKéMON. Two that\n"
	.string "make each other better. That's\l"
	.string "the whole lesson.\p"
	.string "Out here your two can get in each\n"
	.string "other's way instead.\p"
	.string "EARTHQUAKE and SURF don't aim.\n"
	.string "One hits your partner too. The\l"
	.string "other only hits theirs. Know\l"
	.string "which is which!\p"
	.string "Every pair in here is built\n"
	.string "around one trick. Ask them\l"
	.string "about it after you win.\p"
	.string "LUANA plays it clean. She never\n"
	.string "brings a move that could hit her\l"
	.string "own side, because her two are\l"
	.string "chosen so she never has to.$"

OrangeIslands_Kumquat_Gym_Text_GymGuyAfter::
	.string "Four badges! You're ready for\n"
	.string "PUMMELO ISLAND!$"
