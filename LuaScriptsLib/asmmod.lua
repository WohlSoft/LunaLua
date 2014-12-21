asmmod = {}

function asmmod.onInitAPI()

end

--Change the npcToCoins Animation:
function asmmod.setEndingAnimation(effect)
	mem(0x00A3C86E, FIELD_BYTE, effect)
end

--Change the npcToCoins Sound:
function asmmod.setEndingCoinSound(soundid)
	mem(0x00A3C87F, FIELD_BYTE, soundid)
end

function asmmod.setEndingCoinValue(coinvalue)
	mem(0x00A3C891, FIELD_BYTE, coinvalue)
end

return asmmod