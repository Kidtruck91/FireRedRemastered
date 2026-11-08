OrangeIslands_Sunburst_Glassworks_MapScripts::
	.byte 0

OrangeIslands_Sunburst_Glassworks_EventScript_Daughter::
	msgbox OrangeIslands_Sunburst_Glassworks_Text_Daughter, MSGBOX_NPC
	end

OrangeIslands_Sunburst_Glassworks_EventScript_Apprentice::
	msgbox OrangeIslands_Sunburst_Glassworks_Text_Apprentice, MSGBOX_NPC
	end

OrangeIslands_Sunburst_Glassworks_EventScript_Glassblower::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	msgbox OrangeIslands_Sunburst_Glassworks_Text_Glassblower, MSGBOX_DEFAULT
	message Text_MayIHelpYou
	waitmessage
	pokemart OrangeIslands_Sunburst_Glassworks_ItemsGlassblower
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
OrangeIslands_Sunburst_Glassworks_ItemsGlassblower::
	.2byte ITEM_BLUE_FLUTE
	.2byte ITEM_YELLOW_FLUTE
	.2byte ITEM_RED_FLUTE
	.2byte ITEM_BLACK_FLUTE
	.2byte ITEM_WHITE_FLUTE
	.2byte ITEM_NONE
	.align 2

