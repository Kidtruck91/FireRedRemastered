Goldenrod_DeptStore_2F_MapScripts::
	.byte 0


GoldenrodCity_DepartmentStore_2F_EventScript_ClerkTMs1::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart GoldenrodCity_DepartmentStore_2F_TMs1
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
GoldenrodCity_DepartmentStore_2F_TMs1::
	.2byte ITEM_TM01
	.2byte ITEM_TM02
	.2byte ITEM_TM03
	.2byte ITEM_TM04
	.2byte ITEM_TM05
	.2byte ITEM_TM06
	.2byte ITEM_TM07
	.2byte ITEM_TM08
    .2byte ITEM_TM09
	.2byte ITEM_TM10
	.2byte ITEM_TM11
	.2byte ITEM_TM12
	.2byte ITEM_TM13
	.2byte ITEM_TM14
	.2byte ITEM_TM15
	.2byte ITEM_TM16
	.2byte ITEM_TM17
	.2byte ITEM_TM18
	.2byte ITEM_TM19
	.2byte ITEM_TM20
	.2byte ITEM_TM21
	.2byte ITEM_TM22
	.2byte ITEM_TM23
	.2byte ITEM_TM24
	.2byte ITEM_TM25
	.2byte ITEM_NONE
	release
	end

GoldenrodCity_DepartmentStore_2F_EventScript_ClerkTMs2::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart GoldenrodCity_DepartmentStore_2F_TMs2
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
GoldenrodCity_DepartmentStore_2F_TMs2::
	.2byte ITEM_TM26
	.2byte ITEM_TM27
	.2byte ITEM_TM28
	.2byte ITEM_TM29
	.2byte ITEM_TM30
	.2byte ITEM_TM31
	.2byte ITEM_TM32
	.2byte ITEM_TM33
	.2byte ITEM_TM34
	.2byte ITEM_TM35
	.2byte ITEM_TM36
	.2byte ITEM_TM37
	.2byte ITEM_TM38
	.2byte ITEM_TM39
	.2byte ITEM_TM40
	.2byte ITEM_TM41
	.2byte ITEM_TM42
	.2byte ITEM_TM43
	.2byte ITEM_TM44
	.2byte ITEM_TM45
	.2byte ITEM_TM46
	.2byte ITEM_TM47
	.2byte ITEM_TM48
	.2byte ITEM_TM49
	.2byte ITEM_TM50
	.2byte ITEM_NONE
	release
	end
