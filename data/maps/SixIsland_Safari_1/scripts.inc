SixIsland_Safari_1_MapScripts::
	.byte 0

SixIsland_Safari_1_EventScript_HabitatSign::
	callnative LuxurySafari_BufferHabitatName
	msgbox SixIsland_Safari_1_Text_HabitatSign, MSGBOX_SIGN
	end
