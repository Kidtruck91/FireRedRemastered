OrangeIslands_Mandarin_South_DepartmentStore_2F_MapScripts::
	.byte 0

OrangeIslands_Mandarin_South_DepartmentStore_2F_EventScript_LiftClerk::
	msgbox OrangeIslands_Mandarin_South_DepartmentStore_2F_Text_LiftClerk, MSGBOX_NPC
	end

OrangeIslands_Mandarin_South_DepartmentStore_2F_EventScript_Girl2F::
	msgbox OrangeIslands_Mandarin_South_DepartmentStore_2F_Text_Girl2F, MSGBOX_NPC
	end

OrangeIslands_Mandarin_South_DepartmentStore_2F_EventScript_Clerk2F::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart OrangeIslands_Mandarin_South_DepartmentStore_2F_ItemsClerk2F
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
OrangeIslands_Mandarin_South_DepartmentStore_2F_ItemsClerk2F::
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_NONE
	.align 2

