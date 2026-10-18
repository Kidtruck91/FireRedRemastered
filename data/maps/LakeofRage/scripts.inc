LakeofRage_MapScripts::
	.byte 0

@ ---- Johto route trainers (johto_route_trainers.py) ----
LakeofRage_EventScript_JohtoAndre::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_ANDRE, LakeofRage_Text_JohtoAndreIntro, LakeofRage_Text_JohtoAndreDefeat
	msgbox LakeofRage_Text_JohtoAndrePostBattle, MSGBOX_AUTOCLOSE
	end

LakeofRage_EventScript_JohtoRaymond::
	trainerbattle_single TRAINER_JOHTO_FISHERMAN_RAYMOND, LakeofRage_Text_JohtoRaymondIntro, LakeofRage_Text_JohtoRaymondDefeat
	msgbox LakeofRage_Text_JohtoRaymondPostBattle, MSGBOX_AUTOCLOSE
	end

LakeofRage_EventScript_JohtoAaron::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_AARON, LakeofRage_Text_JohtoAaronIntro, LakeofRage_Text_JohtoAaronDefeat
	msgbox LakeofRage_Text_JohtoAaronPostBattle, MSGBOX_AUTOCLOSE
	end

LakeofRage_EventScript_JohtoLois::
	trainerbattle_single TRAINER_JOHTO_COOLTRAINER_LOIS, LakeofRage_Text_JohtoLoisIntro, LakeofRage_Text_JohtoLoisDefeat
	msgbox LakeofRage_Text_JohtoLoisPostBattle, MSGBOX_AUTOCLOSE
	end

LakeofRage_Text_JohtoAndreIntro::
	.string "Let me battle with the POKéMON\n"
	.string "I just caught!$"

LakeofRage_Text_JohtoAndreDefeat::
	.string "I might be an ex- pert angler,\n"
	.string "but\p"
	.string "I stink as a POKé- MON\n"
	.string "trainer…$"

LakeofRage_Text_JohtoAndrePostBattle::
	.string "I won't lose as an angler! I\n"
	.string "catch POKéMON all day.$"

LakeofRage_Text_JohtoRaymondIntro::
	.string "No matter what I do, all I\n"
	.string "catch\p"
	.string "are the same POKé- MON…$"

LakeofRage_Text_JohtoRaymondDefeat::
	.string "My line's all tangled up…$"

LakeofRage_Text_JohtoRaymondPostBattle::
	.string "Why can't I catch any good\n"
	.string "POKéMON?$"

LakeofRage_Text_JohtoAaronIntro::
	.string "If a trainer spots another\n"
	.string "trainer,\p"
	.string "he has to make a challenge.\p"
	.string "That is our destiny.$"

LakeofRage_Text_JohtoAaronDefeat::
	.string "Whew… Good battle.$"

LakeofRage_Text_JohtoAaronPostBattle::
	.string "POKéMON and their trainer\n"
	.string "become\p"
	.string "powerful through constant\n"
	.string "battling.$"

LakeofRage_Text_JohtoLoisIntro::
	.string "What happened to the red\n"
	.string "GYARADOS?\p"
	.string "It's gone?\p"
	.string "Oh, darn. I came here for\n"
	.string "nothing?\p"
	.string "I know--let's battle!$"

LakeofRage_Text_JohtoLoisDefeat::
	.string "Good going!$"

LakeofRage_Text_JohtoLoisPostBattle::
	.string "Come to think of it, I've seen\n"
	.string "a pink BUTTERFREE.$"

