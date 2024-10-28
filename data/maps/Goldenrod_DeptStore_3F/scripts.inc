Goldenrod_DeptStore_3F_MapScripts::
	.byte 0

GoldenrodCity_DepartmentStore_3F_EventScript_Clerk_Items::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart GoldenrodCity_DepartmentStore_3F_Items
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
GoldenrodCity_DepartmentStore_3F_Items::
	.2byte ITEM_SUN_STONE
	.2byte ITEM_MOON_STONE
	.2byte ITEM_FIRE_STONE
	.2byte ITEM_WATER_STONE
	.2byte ITEM_THUNDER_STONE
	.2byte ITEM_LEAF_STONE
	.2byte ITEM_METAL_COAT
	.2byte ITEM_DRAGON_SCALE
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_UP_GRADE
	.2byte ITEM_NONE
	release
	end
