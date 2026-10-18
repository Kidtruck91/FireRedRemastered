Route34_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route34_EventScript_JohtoTodd::
	trainerbattle_single TRAINER_JOHTO_CAMPER_TODD, Route34_Text_JohtoToddIntro, Route34_Text_JohtoToddDefeat
	msgbox Route34_Text_JohtoToddPostBattle, MSGBOX_AUTOCLOSE
	end

Route34_EventScript_JohtoSamuel::
	trainerbattle_single TRAINER_JOHTO_YOUNGSTER_SAMUEL, Route34_Text_JohtoSamuelIntro, Route34_Text_JohtoSamuelDefeat
	msgbox Route34_Text_JohtoSamuelPostBattle, MSGBOX_AUTOCLOSE
	end

Route34_EventScript_JohtoIan::
	trainerbattle_single TRAINER_JOHTO_YOUNGSTER_IAN, Route34_Text_JohtoIanIntro, Route34_Text_JohtoIanDefeat
	msgbox Route34_Text_JohtoIanPostBattle, MSGBOX_AUTOCLOSE
	end

Route34_EventScript_JohtoGina::
	trainerbattle_single TRAINER_JOHTO_PICNICKER_GINA, Route34_Text_JohtoGinaIntro, Route34_Text_JohtoGinaDefeat
	msgbox Route34_Text_JohtoGinaPostBattle, MSGBOX_AUTOCLOSE
	end

Route34_EventScript_JohtoBrandon::
	trainerbattle_single TRAINER_JOHTO_POKEFAN_BRANDON, Route34_Text_JohtoBrandonIntro, Route34_Text_JohtoBrandonDefeat
	msgbox Route34_Text_JohtoBrandonPostBattle, MSGBOX_AUTOCLOSE
	end

Route34_EventScript_JohtoIrene::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_IRENE, Route34_Text_JohtoIreneIntro, Route34_Text_JohtoIreneDefeat
	msgbox Route34_Text_JohtoIrenePostBattle, MSGBOX_AUTOCLOSE
	end

Route34_EventScript_JohtoJenn::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_JENN, Route34_Text_JohtoJennIntro, Route34_Text_JohtoJennDefeat
	msgbox Route34_Text_JohtoJennPostBattle, MSGBOX_AUTOCLOSE
	end

Route34_EventScript_JohtoKate::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_KATE, Route34_Text_JohtoKateIntro, Route34_Text_JohtoKateDefeat
	msgbox Route34_Text_JohtoKatePostBattle, MSGBOX_AUTOCLOSE
	end

Route34_Text_JohtoToddIntro::
	.string "I'm confident in my ability to\n"
	.string "raise POKéMON.\p"
	.string "Want to see?$"

Route34_Text_JohtoToddDefeat::
	.string "Did I screw up my training?$"

Route34_Text_JohtoToddPostBattle::
	.string "Maybe I should take one to a\n"
	.string "DAY-\p"
	.string "CARE. Or maybe use some items…$"

Route34_Text_JohtoSamuelIntro::
	.string "This is where I do my\n"
	.string "training!$"

Route34_Text_JohtoSamuelDefeat::
	.string "Beaten by a passing stranger!$"

Route34_Text_JohtoSamuelPostBattle::
	.string "I'm going to train even\n"
	.string "harder.\p"
	.string "After all, I'm trying to\n"
	.string "become a GYM LEADER.$"

Route34_Text_JohtoIanIntro::
	.string "I'm the best in my class at\n"
	.string "POKéMON.$"

Route34_Text_JohtoIanDefeat::
	.string "No! There are bet- ter\n"
	.string "trainers…$"

Route34_Text_JohtoIanPostBattle::
	.string "I'm trying hard so I can be\n"
	.string "the star in my class.$"

Route34_Text_JohtoGinaIntro::
	.string "Are you a trainer?\p"
	.string "Let's have a practice battle.$"

Route34_Text_JohtoGinaDefeat::
	.string "Oh, no! I just can't win…$"

Route34_Text_JohtoGinaPostBattle::
	.string "You're too strong to be a\n"
	.string "practice partner.$"

Route34_Text_JohtoBrandonIntro::
	.string "I just got my POKéMON back\n"
	.string "from DAY-CARE.\p"
	.string "Let's see how much stronger it\n"
	.string "got!$"

Route34_Text_JohtoBrandonDefeat::
	.string "Why does it end this way?$"

Route34_Text_JohtoBrandonPostBattle::
	.string "My POKéMON knew moves I didn't\n"
	.string "know it had.\p"
	.string "That confounded me to no end!$"

Route34_Text_JohtoIreneIntro::
	.string "IRENE: Kyaaah! Someone found\n"
	.string "us!$"

Route34_Text_JohtoIreneDefeat::
	.string "IRENE: Ohhh! Too strong!$"

Route34_Text_JohtoIrenePostBattle::
	.string "IRENE: My sister KATE will get\n"
	.string "you for this!$"

Route34_Text_JohtoJennIntro::
	.string "JENN: You can't beat IRENE and\n"
	.string "go unpunished!$"

Route34_Text_JohtoJennDefeat::
	.string "JENN: So sorry, IRENE! Sis!$"

Route34_Text_JohtoJennPostBattle::
	.string "JENN: Don't get cocky! My\n"
	.string "sister KATE is tough!$"

Route34_Text_JohtoKateIntro::
	.string "KATE: You sure were mean to my\n"
	.string "little sisters!$"

Route34_Text_JohtoKateDefeat::
	.string "KATE: No! I can't believe I\n"
	.string "lost.$"

Route34_Text_JohtoKatePostBattle::
	.string "KATE: You're too strong. I\n"
	.string "didn't stand a chance.\p"
	.string "Here. You deserve this.$"

