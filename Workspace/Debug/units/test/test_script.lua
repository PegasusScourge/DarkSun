-- entity script file

-- import('lua/ui/testScene/scene.lua')

testSpider = {
	OnCreate = function()
		-- Nothing
		LOG('I am called ' .. thisEntity.internalName)
	end,
	
	OnTick = function()
		if not thisEntity:isPathfinding() then
			local xNew = math.random(10, 100)
			local zNew = math.random(10, 100)
			thisEntity:moveTo(xNew, 6.0, zNew)
			LOG('Navigating to X=' .. tostring(xNew) .. ', Z=' .. tostring(zNew))
		end
	end,
}
tick = testSpider.OnTick -- just in case tick() gets called, make sure we point it to something