Route35_Daycare_MapScripts::
	.byte 0

Script_RandomEgg::
	lock
	faceplayer
	msgbox Text_EggQuestion, MSGBOX_YESNO
	compare VAR_RESULT, 0
	goto_if_eq Script_PlayerAnsweredNo
	getpartysize
	compare VAR_RESULT, 6
	goto_if_eq Script_PlayerHasFullParty
	random 78
	switch VAR_RESULT
	case 0, Script_RandomOption1
	case 1, Script_RandomOption2
	case 2, Script_RandomOption3
	case 3, Script_RandomOption4
	case 4, Script_RandomOption5
	case 5, Script_RandomOption6
	case 6, Script_RandomOption7
	case 7, Script_RandomOption8
	case 8, Script_RandomOption9
	case 9, Script_RandomOption10
	case 10, Script_RandomOption11
	case 11, Script_RandomOption12
	case 12, Script_RandomOption13
	case 13, Script_RandomOption14
	case 14, Script_RandomOption15
	case 15, Script_RandomOption16
	case 16, Script_RandomOption17
	case 17, Script_RandomOption18
	case 18, Script_RandomOption19
	case 19, Script_RandomOption20
	case 20, Script_RandomOption21
	case 21, Script_RandomOption22
	case 22, Script_RandomOption23
	case 23, Script_RandomOption24
	case 24, Script_RandomOption25
	case 25, Script_RandomOption26
	case 26, Script_RandomOption27
	case 27, Script_RandomOption28
	case 28, Script_RandomOption29
	case 29, Script_RandomOption30
	case 30, Script_RandomOption31
	case 31, Script_RandomOption32
	case 32, Script_RandomOption33
	case 33, Script_RandomOption34
	case 34, Script_RandomOption35
	case 35, Script_RandomOption36
	case 36, Script_RandomOption37
	case 37, Script_RandomOption38
	case 38, Script_RandomOption39
	case 39, Script_RandomOption40
	case 40, Script_RandomOption41
	case 41, Script_RandomOption42
	case 42, Script_RandomOption43
	case 43, Script_RandomOption44
	case 44, Script_RandomOption45
	case 45, Script_RandomOption46
	case 46, Script_RandomOption47
	case 47, Script_RandomOption48
	case 48, Script_RandomOption49
	case 49, Script_RandomOption50
	case 50, Script_RandomOption51
	case 51, Script_RandomOption52
	case 52, Script_RandomOption53
	case 53, Script_RandomOption54
	case 54, Script_RandomOption55
	case 55, Script_RandomOption56
	case 56, Script_RandomOption57
	case 57, Script_RandomOption58
	case 58, Script_RandomOption59
	case 59, Script_RandomOption60
	case 60, Script_RandomOption61
	case 61, Script_RandomOption62
	case 62, Script_RandomOption63
	case 63, Script_RandomOption64
	case 64, Script_RandomOption65
	case 65, Script_RandomOption66
	case 66, Script_RandomOption67
	case 67, Script_RandomOption68
	case 68, Script_RandomOption69
	case 69, Script_RandomOption70
	case 70, Script_RandomOption71
	case 71, Script_RandomOption72
	case 72, Script_RandomOption73
	case 73, Script_RandomOption74
	case 74, Script_RandomOption75
	case 75, Script_RandomOption76
	case 76, Script_RandomOption77
	case 77, Script_RandomOption78
	msgbox Text_ReceivedEgg, MSGBOX_DEFAULT
	waitfanfare
	end

Script_RandomOption1::
	giveegg SPECIES_PICHU
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption2::
	giveegg SPECIES_IGGLYBUFF
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption3::
	giveegg SPECIES_MAGBY
	playfanfare MUS_OBTAIN_ITEM
	end
	
Script_RandomOption4::
	giveegg SPECIES_ELEKID
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption5::
	giveegg SPECIES_SMOOCHUM
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption6::
	giveegg SPECIES_EEVEE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption7::
	giveegg SPECIES_TOGEPI
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption8::
	giveegg SPECIES_SUNKERN
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption9::
	giveegg SPECIES_AIPOM
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption10::
	giveegg SPECIES_MISDREAVUS
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption11::
	giveegg SPECIES_SNUBBULL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption12::
	giveegg SPECIES_SHUCKLE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption13::
	giveegg SPECIES_PHANPY
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption14::
	giveegg SPECIES_TYROGUE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption15::
	giveegg SPECIES_LARVITAR
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption16::
	giveegg SPECIES_HOUNDOUR
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption17::
	giveegg SPECIES_TREECKO
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption18::
	giveegg SPECIES_TORCHIC
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption19::
	giveegg SPECIES_MUDKIP
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption20::
	giveegg SPECIES_RALTS
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption21::
	giveegg SPECIES_WHISMUR
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption22::
	giveegg SPECIES_NINCADA
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption23::
	giveegg SPECIES_NOSEPASS
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption24::
	giveegg SPECIES_ARON
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption25::
	giveegg SPECIES_BAGON
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption26::
	giveegg SPECIES_SABLEYE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption27::
	giveegg SPECIES_POOCHYENA
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption28::
	giveegg SPECIES_ZIGZAGOON
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption29::
	giveegg SPECIES_WURMPLE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption30::
	giveegg SPECIES_LOTAD
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption31::
	giveegg SPECIES_SEEDOT
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption32::
	giveegg SPECIES_TAILLOW
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption33::
	giveegg SPECIES_WINGULL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption34::
	giveegg SPECIES_SURSKIT
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption35::
	giveegg SPECIES_SHROOMISH
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption36::
	giveegg SPECIES_SLAKOTH
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption37::
	giveegg SPECIES_MAKUHITA
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption38::
	giveegg SPECIES_SKITTY
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption39::
	giveegg SPECIES_MAWILE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption40::
	giveegg SPECIES_MEDITITE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption41::
	giveegg SPECIES_ELECTRIKE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption42::
	giveegg SPECIES_PLUSLE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption43::
	giveegg SPECIES_MINUN
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption44::
	giveegg SPECIES_VOLBEAT
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption45::
	giveegg SPECIES_ILLUMISE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption46::
	giveegg SPECIES_ROSELIA
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption47::
	giveegg SPECIES_GULPIN
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption48::
	giveegg SPECIES_CARVANHA
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption49::
	giveegg SPECIES_WAILMER
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption50::
	giveegg SPECIES_NUMEL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption51::
	giveegg SPECIES_TORKOAL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption52::
	giveegg SPECIES_SPOINK
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption53::
	giveegg SPECIES_SPINDA
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption54::
	giveegg SPECIES_TRAPINCH
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption55::
	giveegg SPECIES_CACNEA
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption56::
	giveegg SPECIES_SWABLU
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption57::
	giveegg SPECIES_ZANGOOSE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption58::
	giveegg SPECIES_SEVIPER
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption59::
	giveegg SPECIES_LUNATONE
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption60::
	giveegg SPECIES_SOLROCK
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption61::
	giveegg SPECIES_BARBOACH
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption62::
	giveegg SPECIES_CORPHISH
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption63::
	giveegg SPECIES_BALTOY
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption64::
	giveegg SPECIES_FEEBAS
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption65::
	giveegg SPECIES_CASTFORM
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption66::
	giveegg SPECIES_KECLEON
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption67::
	giveegg SPECIES_SHUPPET
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption68::
	giveegg SPECIES_DUSKULL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption69::
	giveegg SPECIES_TROPIUS
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption70::
	giveegg SPECIES_CHIMECHO
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption71::
	giveegg SPECIES_ABSOL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption72::
	giveegg SPECIES_WYNAUT
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption73::
	giveegg SPECIES_SNORUNT
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption74::
	giveegg SPECIES_SPHEAL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption75::
	giveegg SPECIES_CLAMPERL
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption76::
	giveegg SPECIES_RELICANTH
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption77::
	giveegg SPECIES_LUVDISC
	playfanfare MUS_OBTAIN_ITEM
	end

Script_RandomOption78::
	giveegg SPECIES_BELDUM
	playfanfare MUS_OBTAIN_ITEM
	end

Script_PlayerHasFullParty::
	msgbox Text_PlayerHasFullParty, MSGBOX_DEFAULT
	release
	end

Script_PlayerAnsweredNo::
	msgbox Text_PlayerAnsweredNo, MSGBOX_DEFAULT
	release
	end

Text_EggQuestion:
	.string "Will you take this Egg?$"

Text_PlayerAnsweredNo:
	.string "Oh, that's too bad.$"

Text_ReceivedEgg:
	.string "{PLAYER} received the Egg!$"

Text_PlayerHasFullParty:
	.string "Your party is full.\n"
	.string "There's no room for this Egg.$"
