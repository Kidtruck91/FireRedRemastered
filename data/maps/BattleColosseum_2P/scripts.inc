BattleColosseum_2P_MapScripts::
    map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, BattleColosseum_2P_OnWarp
    call BattleColosseum_2P_EventScript_NurseJoyBattle
    .byte 0

BattleColosseum_2P_OnWarp::
    end

BattleColosseum_2P_EventScript_NurseJoyBattle::
    lockall
    special HealPlayerParty
    special SavePlayerParty
    trainerbattle_single TRAINER_NURSE_JOY, gText_NurseJoy_Text_BattleIntro, gText_NurseJoy_Text_BattleDefeat
    releaseall
    end
