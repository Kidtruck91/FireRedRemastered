@ VOLTORB FLIP. Written rather than ported -- it is a DS game and there is no
@ GBA source for it anywhere -- so see src/voltorb_flip.c for the rules and
@ the board tables. The level is kept in VAR_ORANGE_VOLTORB_LEVEL.

VoltorbFlip_EventScript_Play::
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, VoltorbFlip_EventScript_NoCoinCase
	special PlayVoltorbFlip
	waitstate
	release
	end

VoltorbFlip_EventScript_NoCoinCase::
	msgbox VoltorbFlip_Text_NoCoinCase, MSGBOX_DEFAULT
	release
	end

VoltorbFlip_Text_NoCoinCase::
	.string "You don't have a COIN CASE.$"
