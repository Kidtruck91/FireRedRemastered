Route35_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route35_EventScript_JohtoIvan::
	trainerbattle_single TRAINER_JOHTO_CAMPER_IVAN, Route35_Text_JohtoIvanIntro, Route35_Text_JohtoIvanDefeat
	msgbox Route35_Text_JohtoIvanPostBattle, MSGBOX_AUTOCLOSE
	end

Route35_EventScript_JohtoElliot::
	trainerbattle_single TRAINER_JOHTO_CAMPER_ELLIOT, Route35_Text_JohtoElliotIntro, Route35_Text_JohtoElliotDefeat
	msgbox Route35_Text_JohtoElliotPostBattle, MSGBOX_AUTOCLOSE
	end

Route35_EventScript_JohtoBrooke::
	trainerbattle_single TRAINER_JOHTO_PICNICKER_BROOKE, Route35_Text_JohtoBrookeIntro, Route35_Text_JohtoBrookeDefeat
	msgbox Route35_Text_JohtoBrookePostBattle, MSGBOX_AUTOCLOSE
	end

Route35_EventScript_JohtoKim::
	trainerbattle_single TRAINER_JOHTO_PICNICKER_KIM, Route35_Text_JohtoKimIntro, Route35_Text_JohtoKimDefeat
	msgbox Route35_Text_JohtoKimPostBattle, MSGBOX_AUTOCLOSE
	end

Route35_EventScript_JohtoBryan::
	trainerbattle_single TRAINER_JOHTO_BIRD_KEEPER_BRYAN, Route35_Text_JohtoBryanIntro, Route35_Text_JohtoBryanDefeat
	msgbox Route35_Text_JohtoBryanPostBattle, MSGBOX_AUTOCLOSE
	end

Route35_EventScript_JohtoWalt::
	trainerbattle_single TRAINER_JOHTO_KINDLER_WALT, Route35_Text_JohtoWaltIntro, Route35_Text_JohtoWaltDefeat
	msgbox Route35_Text_JohtoWaltPostBattle, MSGBOX_AUTOCLOSE
	end

Route35_EventScript_JohtoArnie::
	trainerbattle_single TRAINER_JOHTO_BUG_CATCHER_ARNIE, Route35_Text_JohtoArnieIntro, Route35_Text_JohtoArnieDefeat
	msgbox Route35_Text_JohtoArniePostBattle, MSGBOX_AUTOCLOSE
	end

Route35_EventScript_JohtoIrwin::
	trainerbattle_single TRAINER_JOHTO_JUGGLER_IRWIN, Route35_Text_JohtoIrwinIntro, Route35_Text_JohtoIrwinDefeat
	msgbox Route35_Text_JohtoIrwinPostBattle, MSGBOX_AUTOCLOSE
	end

Route35_Text_JohtoIvanIntro::
	.string "I've been getting POKéMON data\n"
	.string "off\p"
	.string "my radio. I think I'm good.$"

Route35_Text_JohtoIvanDefeat::
	.string "I give!$"

Route35_Text_JohtoIvanPostBattle::
	.string "Music on the radio changes the\n"
	.string "moods of wild POKéMON.$"

Route35_Text_JohtoElliotIntro::
	.string "I'm gonna show my girlfriend\n"
	.string "I'm hot stuff!$"

Route35_Text_JohtoElliotDefeat::
	.string "I wish you would have lost for\n"
	.string "me…$"

Route35_Text_JohtoElliotPostBattle::
	.string "I was humiliated in front of\n"
	.string "my girlfriend…$"

Route35_Text_JohtoBrookeIntro::
	.string "My boyfriend's weak, so I\n"
	.string "can't rely on him.$"

Route35_Text_JohtoBrookeDefeat::
	.string "Oh, my! You're so strong!$"

Route35_Text_JohtoBrookePostBattle::
	.string "I can count on my POKéMON more\n"
	.string "than my boyfriend.$"

Route35_Text_JohtoKimIntro::
	.string "Are you going to the GYM? Me\n"
	.string "too!$"

Route35_Text_JohtoKimDefeat::
	.string "Oh. I couldn't win…$"

Route35_Text_JohtoKimPostBattle::
	.string "The GYM BADGES are pretty. I\n"
	.string "collect them.$"

Route35_Text_JohtoBryanIntro::
	.string "What kinds of BALLS do you\n"
	.string "use?$"

Route35_Text_JohtoBryanDefeat::
	.string "Yikes! Not fast enough!$"

Route35_Text_JohtoBryanPostBattle::
	.string "Some POKéMON flee right away.\p"
	.string "Try catching them with KURT's\n"
	.string "FAST BALL.\p"
	.string "Whenever I find a WHT\n"
	.string "APRICORN, I take it to KURT.\p"
	.string "He turns it into a custom\n"
	.string "BALL.$"

Route35_Text_JohtoWaltIntro::
	.string "I'm practicing my fire\n"
	.string "breathing.$"

Route35_Text_JohtoWaltDefeat::
	.string "Ow! I scorched the tip of my\n"
	.string "nose!$"

Route35_Text_JohtoWaltPostBattle::
	.string "The POKéMON March on the radio\n"
	.string "lures wild POKéMON.$"

Route35_Text_JohtoArnieIntro::
	.string "I'll go anywhere if bug\n"
	.string "POKéMON appear there.$"

Route35_Text_JohtoArnieDefeat::
	.string "Huh? I shouldn't have lost\n"
	.string "that…$"

Route35_Text_JohtoArniePostBattle::
	.string "My VENONAT won me the Bug-\n"
	.string "Catching\p"
	.string "Contest at the NATIONAL PARK.$"

Route35_Text_JohtoIrwinIntro::
	.string "Behold my graceful BALL\n"
	.string "dexterity!$"

Route35_Text_JohtoIrwinDefeat::
	.string "Whew! That was a jolt!$"

Route35_Text_JohtoIrwinPostBattle::
	.string "I was going to dazzle you with\n"
	.string "my prize POKéMON.\p"
	.string "But your prowess electrified\n"
	.string "me!$"

