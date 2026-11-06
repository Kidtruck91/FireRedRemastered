OrangeIslands_Mandarin_South_Mart_MapScripts::
	.byte 0

OrangeIslands_Mandarin_South_Mart_EventScript_Sailor::
	msgbox OrangeIslands_Mandarin_South_Mart_Text_Sailor, MSGBOX_NPC
	end

OrangeIslands_Mandarin_South_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart OrangeIslands_Mandarin_South_Mart_ItemsClerk
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
OrangeIslands_Mandarin_South_Mart_ItemsClerk::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_NONE
	.align 2

