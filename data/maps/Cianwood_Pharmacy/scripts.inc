Cianwood_Pharmacy_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Cianwood_Pharmacy_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

Cianwood_Pharmacy_OnTransition::
	setrespawn SPAWN_CIANWOOD_CITY
	end

Cianwood_Pharmacy_EventScript_Nurse::
	lock
	faceplayer
	call EventScript_PkmnCenterNurse
	release
	end

Cianwood_Pharmacy_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart Cianwood_Pharmacy_Items
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
Cianwood_Pharmacy_Items::
	.2byte ITEM_WHITE_HERB
	.2byte ITEM_MENTAL_HERB
	.2byte ITEM_ENERGY_POWDER
	.2byte ITEM_ENERGY_ROOT
	.2byte ITEM_HEAL_POWDER
	.2byte ITEM_REVIVAL_HERB
	.2byte ITEM_BERRY_JUICE
	.2byte ITEM_CHERI_BERRY
	.2byte ITEM_CHESTO_BERRY
	.2byte ITEM_PECHA_BERRY
	.2byte ITEM_RAWST_BERRY
	.2byte ITEM_ASPEAR_BERRY
	.2byte ITEM_LEPPA_BERRY
	.2byte ITEM_ORAN_BERRY
	.2byte ITEM_PERSIM_BERRY
	.2byte ITEM_LUM_BERRY
	.2byte ITEM_SITRUS_BERRY
	.2byte ITEM_FIGY_BERRY
	.2byte ITEM_WIKI_BERRY
	.2byte ITEM_MAGO_BERRY
	.2byte ITEM_AGUAV_BERRY
	.2byte ITEM_IAPAPA_BERRY
	.2byte ITEM_RAZZ_BERRY
	.2byte ITEM_BLUK_BERRY 
    .2byte ITEM_NANAB_BERRY 
	.2byte ITEM_WEPEAR_BERRY 
	.2byte ITEM_PINAP_BERRY
	.2byte ITEM_POMEG_BERRY 
	.2byte ITEM_KELPSY_BERRY 
	.2byte ITEM_QUALOT_BERRY
	.2byte ITEM_HONDEW_BERRY 
	.2byte ITEM_GREPA_BERRY 
	.2byte ITEM_TAMATO_BERRY
	.2byte ITEM_CORNN_BERRY 
	.2byte ITEM_MAGOST_BERRY 
	.2byte ITEM_RABUTA_BERRY 
	.2byte ITEM_NOMEL_BERRY	
	.2byte ITEM_SPELON_BERRY 
	.2byte ITEM_PAMTRE_BERRY 
	.2byte ITEM_WATMEL_BERRY 
	.2byte ITEM_DURIN_BERRY 
	.2byte ITEM_BELUE_BERRY 
	.2byte ITEM_LIECHI_BERRY 
	.2byte ITEM_GANLON_BERRY
	.2byte ITEM_SALAC_BERRY 
	.2byte ITEM_PETAYA_BERRY 
	.2byte ITEM_APICOT_BERRY 
	.2byte ITEM_LANSAT_BERRY 
	.2byte ITEM_STARF_BERRY 
	.2byte ITEM_ENIGMA_BERRY 
	.2byte ITEM_NONE
	release
	end
