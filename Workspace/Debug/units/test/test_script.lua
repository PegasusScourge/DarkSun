-- entity script file

import('lua/ui/testScene/scene.lua')

testSpider = {
	OnCreate = function()
		-- Nothing
		LOG('I am called ' .. myEntity.internalName)
	end,
	
	OnTick = function()
		if not myEntity:isPathfinding() then
			local xNew = math.random(-10, 10)
			local zNew = math.random(-10, 10)
			myEntity:moveTo(xNew, 6.0, zNew)
			LOG('Navigating to X=' .. tostring(xNew) .. ', Z=' .. tostring(zNew))
		end
	end,
}
tick = testSpider.OnTick -- just in case tick() gets called, make sure we point it to something