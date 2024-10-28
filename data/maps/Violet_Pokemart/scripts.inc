Violet_Pokemart_MapScripts::
	.byte 0

Violet_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart Violet_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
Violet_Mart_Items::
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_NONE
	release
	end
