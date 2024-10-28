Goldenrod_DeptStore_5F_MapScripts::
	.byte 0

GoldenrodCity_DepartmentStore_5F_EventScript_Clerk1_Items::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart GoldenrodCity_DepartmentStore_5F_Items1
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
GoldenrodCity_DepartmentStore_5F_Items1::
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POKE_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_PREMIER_BALL
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_MAX_POTION
	.2byte ITEM_REVIVE
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_ETHER
	.2byte ITEM_MAX_ETHER
	.2byte ITEM_ELIXIR
	.2byte ITEM_MAX_ELIXIR
	.2byte ITEM_NONE
	release
	end
	
GoldenrodCity_DepartmentStore_5F_EventScript_Clerk2_Items::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart GoldenrodCity_DepartmentStore_5F_Items2
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
GoldenrodCity_DepartmentStore_5F_Items2::
	.2byte ITEM_BRIGHT_POWDER
	.2byte ITEM_WHITE_HERB
	.2byte ITEM_MACHO_BRACE
	.2byte ITEM_QUICK_CLAW
	.2byte ITEM_SOOTHE_BELL
	.2byte ITEM_MENTAL_HERB
	.2byte ITEM_CHOICE_BAND
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_SILVER_POWDER
	.2byte ITEM_AMULET_COIN
	.2byte ITEM_CLEANSE_TAG
	.2byte ITEM_DEEP_SEA_TOOTH
	.2byte ITEM_DEEP_SEA_SCALE
	.2byte ITEM_SMOKE_BALL
	.2byte ITEM_FOCUS_BAND
	.2byte ITEM_LUCKY_EGG
	.2byte ITEM_SCOPE_LENS
	.2byte ITEM_LEFTOVERS
	.2byte ITEM_LIGHT_BALL
	.2byte ITEM_SOFT_SAND
	.2byte ITEM_HARD_STONE
	.2byte ITEM_MIRACLE_SEED
	.2byte ITEM_BLACK_GLASSES
	.2byte ITEM_BLACK_BELT
	.2byte ITEM_MAGNET
    .2byte ITEM_MYSTIC_WATER
	.2byte ITEM_SHARP_BEAK
	.2byte ITEM_POISON_BARB
	.2byte ITEM_NEVER_MELT_ICE
	.2byte ITEM_SPELL_TAG
	.2byte ITEM_TWISTED_SPOON
	.2byte ITEM_CHARCOAL
	.2byte ITEM_DRAGON_FANG
	.2byte ITEM_SILK_SCARF
	.2byte ITEM_SHELL_BELL
	.2byte ITEM_SEA_INCENSE 
	.2byte ITEM_LAX_INCENSE
	.2byte ITEM_LUCKY_PUNCH
	.2byte ITEM_METAL_POWDER
	.2byte ITEM_THICK_CLUB
	.2byte ITEM_STICK
	.2byte ITEM_NONE
	release
	end
