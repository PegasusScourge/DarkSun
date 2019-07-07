-- entity script file

testSpider = {
	OnCreate = function()
		-- Nothing
	end,
	
	OnTick = function()
		--LOG('Entity ' .. myEntity.internalName .. ' tick!')
	end,
}
tick = testSpider.OnTick -- just in case tick() gets called, make sure we point it to something