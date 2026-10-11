@ A wild Pokemon wandering in the overworld (src/overworld_mons.c). Walking into it or
@ pressing A starts a wild battle against it. It counts as a normal wild battle (Nuzlocke too).
EventScript_OverworldMon::
	lock
	faceplayer
	callnative OverworldMon_PrepareBattle
	goto_if_eq VAR_RESULT, FALSE, EventScript_OverworldMonGone
	playmoncry VAR_0x8004, CRY_MODE_ENCOUNTER
	waitmoncry
	callnative OverworldMon_RemoveTalked
	dowildbattle
	release
	end

EventScript_OverworldMonGone::
	callnative OverworldMon_RemoveTalked
	release
	end
