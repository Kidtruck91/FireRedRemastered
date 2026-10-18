Route39_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
Route39_EventScript_JohtoEugene::
	trainerbattle_single TRAINER_JOHTO_SAILOR_EUGENE, Route39_Text_JohtoEugeneIntro, Route39_Text_JohtoEugeneDefeat
	msgbox Route39_Text_JohtoEugenePostBattle, MSGBOX_AUTOCLOSE
	end

Route39_EventScript_JohtoDerek::
	trainerbattle_single TRAINER_JOHTO_POKEFAN_DEREK, Route39_Text_JohtoDerekIntro, Route39_Text_JohtoDerekDefeat
	msgbox Route39_Text_JohtoDerekPostBattle, MSGBOX_AUTOCLOSE
	end

Route39_EventScript_JohtoRuth::
	trainerbattle_single TRAINER_JOHTO_POKEFAN_RUTH, Route39_Text_JohtoRuthIntro, Route39_Text_JohtoRuthDefeat
	msgbox Route39_Text_JohtoRuthPostBattle, MSGBOX_AUTOCLOSE
	end

Route39_EventScript_JohtoNorman::
	trainerbattle_single TRAINER_JOHTO_PSYCHIC_NORMAN, Route39_Text_JohtoNormanIntro, Route39_Text_JohtoNormanDefeat
	msgbox Route39_Text_JohtoNormanPostBattle, MSGBOX_AUTOCLOSE
	end

Route39_Text_JohtoEugeneIntro::
	.string "I just got back to OLIVINE.\p"
	.string "So how about a POKéMON battle?$"

Route39_Text_JohtoEugeneDefeat::
	.string "Awaaargh!$"

Route39_Text_JohtoEugenePostBattle::
	.string "My POKéMON were caught and\n"
	.string "raised overseas.\p"
	.string "They're my compan- ions on\n"
	.string "those long voyages.$"

Route39_Text_JohtoDerekIntro::
	.string "This is a good time to brag\n"
	.string "about my PIKACHU!$"

Route39_Text_JohtoDerekDefeat::
	.string "I had no time to show off\n"
	.string "PIKACHU…$"

Route39_Text_JohtoDerekPostBattle::
	.string "I'm not listening to your\n"
	.string "bragging!\p"
	.string "We POKé FANS have a policy of\n"
	.string "not\p"
	.string "listening to other people\n"
	.string "brag!$"

Route39_Text_JohtoRuthIntro::
	.string "Such darling POKéMON.\p"
	.string "Let's show our POKéMON\n"
	.string "together at the same time.$"

Route39_Text_JohtoRuthDefeat::
	.string "I don't mind losing.$"

Route39_Text_JohtoRuthPostBattle::
	.string "Do you know about baby\n"
	.string "POKéMON?\p"
	.string "I bet they're just adorable!$"

Route39_Text_JohtoNormanIntro::
	.string "Let me see what your POKéMON\n"
	.string "are capable of.$"

Route39_Text_JohtoNormanDefeat::
	.string "Ooh, your POKéMON have\n"
	.string "potential.$"

Route39_Text_JohtoNormanPostBattle::
	.string "You know how POKé- MON have\n"
	.string "different abilities?\p"
	.string "People are like that too.\n"
	.string "Every- one has different\l"
	.string "potential.$"

