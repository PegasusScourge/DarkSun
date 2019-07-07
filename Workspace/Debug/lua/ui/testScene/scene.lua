-- Scene file for testScene

-- declare our scene table
testScene = {
	SceneTransition = {
		To = nil,
	},

	FunctionCallbacks = {
		clickableButton = function(what)
			--LOG('Button ' .. what .. ' was clicked!')
			--local vis = not myGui:isWidgetVisible('fpsLabel')
			--if vis then
			--	myGui:showWithEffect('fpsLabel', 'slideFromBottom', 500)
			--else
			--	myGui:hideWithEffect('fpsLabel', 'slideToTop', 250)
			--end
			testScene.SceneTransition.To = 'exit'
			myGui:transitionScene()
		end,
	},

	OnCreate = function()
		LOG('Create scene "' .. myGui.name .. '"')
		myGui:setCallbackTable('FunctionCallbacks')
		
		myGui:addNewButton('clickableButton')
		myGui:setWidgetPositionPercent('clickableButton', '0%', '14%')
		myGui:setButtonWidgetText('clickableButton', 'Close')
		myGui:registerWidgetCallback('clickableButton', 'pressed')
		
		for i=0,100 do
			local name = 'clickableButton_' .. tostring(i)
			myGui:addNewButton(name)
			myGui:setWidgetPosition(name, (i % 10) * 60, 100 + (math.floor(i / 10) * 20))
			myGui:setButtonWidgetText(name, 'Crash Me!')
			
			testScene.FunctionCallbacks[name] = function(what)
				-- Do nothing
			end
			
			myGui:registerWidgetCallback(name, 'pressed')
		end
		
		myGui:addNewLabel('fpsLabel')
		myGui:setWidgetPositionPercent('fpsLabel', '0%', '10%')
		myGui:setLabelWidgetText('fpsLabel', 'Hello World!')
	end,
	
	OnTick = function(deltaTime)
		local fps = math.ceil(1/deltaTime)
		myGui:setLabelWidgetText('fpsLabel', 'FPS=' .. tostring(fps) .. ', dt:' .. tostring(deltaTime) .. 's')
	end,
}
tick = testScene.OnTick -- just in case tick() gets called, make sure we point it to something