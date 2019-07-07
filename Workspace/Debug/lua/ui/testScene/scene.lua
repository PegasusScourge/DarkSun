-- Scene file for testScene

-- declare our scene table
testScene = {
	FunctionCallbacks = {
		clickableButton = function(what)
			LOG('Button ' .. what .. ' was clicked!')
		end,
	},

	OnCreate = function()
		LOG('Create scene "' .. myGui.name .. '"')
		myGui:setCallbackTable('FunctionCallbacks')
		
		myGui:addNewLabel('fpsLabel')
		myGui:setWidgetPositionPercent('fpsLabel', '0%', '10%')
		myGui:setLabelWidgetText('fpsLabel', 'Hello World!')
		myGui:addNewButton('clickableButton')
		myGui:setWidgetPositionPercent('clickableButton', '0%', '14%')
		myGui:setButtonWidgetText('clickableButton', 'Crash Me!')
		myGui:registerWidgetCallback('clickableButton', 'pressed')
		end,
	
	OnTick = function(deltaTime)
		local fps = math.ceil(1/deltaTime)
		myGui:setLabelWidgetText('fpsLabel', 'FPS=' .. tostring(fps) .. ', dt:' .. tostring(deltaTime) .. 's')
	end,
}
tick = testScene.OnTick -- just in case tick() gets called, make sure we point it to something