OrangeIslands_Valencia_Mart_MapScripts::
	.byte 0

OrangeIslands_Valencia_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart OrangeIslands_Valencia_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

OrangeIslands_Valencia_Mart_EventScript_Sailor::
	msgbox OrangeIslands_Valencia_Mart_Text_Sailor, MSGBOX_NPC
	end

OrangeIslands_Valencia_Mart_EventScript_Lass::
	msgbox OrangeIslands_Valencia_Mart_Text_Lass, MSGBOX_NPC
	end

	.align 2
OrangeIslands_Valencia_Mart_Items::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_NONE

