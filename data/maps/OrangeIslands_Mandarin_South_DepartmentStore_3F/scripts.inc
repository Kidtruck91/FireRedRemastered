OrangeIslands_Mandarin_South_DepartmentStore_3F_MapScripts::
	.byte 0

OrangeIslands_Mandarin_South_DepartmentStore_3F_EventScript_LiftClerk::
	msgbox OrangeIslands_Mandarin_South_DepartmentStore_3F_Text_LiftClerk, MSGBOX_NPC
	end

OrangeIslands_Mandarin_South_DepartmentStore_3F_EventScript_Scientist3F::
	msgbox OrangeIslands_Mandarin_South_DepartmentStore_3F_Text_Scientist3F, MSGBOX_NPC
	end

OrangeIslands_Mandarin_South_DepartmentStore_3F_EventScript_Clerk3F::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart OrangeIslands_Mandarin_South_DepartmentStore_3F_ItemsClerk3F
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
OrangeIslands_Mandarin_South_DepartmentStore_3F_ItemsClerk3F::
	.2byte ITEM_TM13
	.2byte ITEM_TM24
	.2byte ITEM_TM45
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_REVIVE
	.2byte ITEM_PROTEIN
	.2byte ITEM_IRON
	.2byte ITEM_CARBOS
	.2byte ITEM_CALCIUM
	.2byte ITEM_HP_UP
	.2byte ITEM_NONE
	.align 2

