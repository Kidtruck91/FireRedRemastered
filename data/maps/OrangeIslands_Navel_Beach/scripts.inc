OrangeIslands_Navel_Beach_MapScripts::
	.byte 0

OrangeIslands_Navel_Beach_EventScript_Town::
	msgbox OrangeIslands_Navel_Beach_Text_Town, MSGBOX_SIGN
	end

OrangeIslands_Navel_Beach_EventScript_Harbor::
	msgbox OrangeIslands_Navel_Beach_Text_Harbor, MSGBOX_SIGN
	end

@ Sea warp: different tilesets, so no seamless edge
OrangeIslands_Navel_Beach_EventScript_SailToRoute9::
	warp MAP_ORANGE_ISLANDS_ROUTE_9, 50, 109
	waitstate
	end

@ Sea warp: different tilesets, so no seamless edge
OrangeIslands_Navel_Beach_EventScript_SailToRoute10::
	warp MAP_ORANGE_ISLANDS_ROUTE_10, 1, 59
	waitstate
	end
