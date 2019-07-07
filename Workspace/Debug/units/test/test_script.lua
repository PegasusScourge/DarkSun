-- entity script file

testSpider = {
	OnCreate = function()
		-- Nothing
	end,
	
	OnTick = function()
		LOG('Entity ' .. myEntity.internalName .. ' tick!')
	end,
}