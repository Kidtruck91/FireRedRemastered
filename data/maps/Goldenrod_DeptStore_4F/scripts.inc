Goldenrod_DeptStore_4F_MapScripts::
	.byte 0

GoldenrodCity_DepartmentStore_4F_EventScript_Clerk_Items::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart GoldenrodCity_DepartmentStore_4F_Items
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
GoldenrodCity_DepartmentStore_4F_Items::
	.2byte ITEM_HP_UP
	.2byte ITEM_PROTEIN
	.2byte ITEM_IRON
	.2byte ITEM_CARBOS
	.2byte ITEM_CALCIUM
	.2byte ITEM_ZINC
	.2byte ITEM_PP_UP
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_NONE
	release
	end
