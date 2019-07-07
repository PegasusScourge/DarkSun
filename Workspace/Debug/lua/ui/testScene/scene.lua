-- Scene file for testScene

-- declare our scene table
testScene = {
	OnCreate = function()
		LOG('Create scene "' .. myGui.name .. '"')
	end,
	
	OnTick = function(deltaTime)
		--LOG('Scene tick!')
	end,
}
tick = testScene.OnTick -- just in case tick() gets called, make sure we point it to something