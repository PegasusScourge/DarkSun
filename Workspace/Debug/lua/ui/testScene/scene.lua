-- Scene file for testScene

-- declare our scene table
testScene = {
	SceneTransition = {
		To = nil,
	},

	FunctionCallbacks = {
		exitButton = function(what)
			testScene.SceneTransition.To = 'exit'
			myGui:transitionScene()
		end,
		testButton = function(what)
			testScene.SceneTransition.To = 'testScene'
			myGui:transitionScene()
		end,
	},

	OnCreate = function()
		LOG('Create scene "' .. myGui.name .. '"')
		myGui:setCallbackTable('FunctionCallbacks')
		
		myGui:addNewButton('exitButton')
		myGui:setWidgetPositionPercent('exitButton', '0%', '14%')
		myGui:setButtonWidgetText('exitButton', 'Close')
		myGui:registerWidgetCallback('exitButton', 'pressed')
		
		myGui:addNewButton('testButton')
		myGui:setWidgetPositionPercent('testButton', '10%', '14%')
		myGui:setButtonWidgetText('testButton', 'Switch Scene')
		myGui:registerWidgetCallback('testButton', 'pressed')
		
		myGui:addNewLabel('fpsLabel')
		myGui:setWidgetPositionPercent('fpsLabel', '0%', '10%')
		myGui:setLabelWidgetText('fpsLabel', 'Hello World!')
		
		myGui:addNewLabel('cameraInfo')
		myGui:setWidgetPositionPercent('cameraInfo', '0%', '5%')
		myGui:setLabelWidgetText('cameraInfo', 'Hello World!')
	end,
	
	OnTick = function(deltaTime)
		local fps = round(1/deltaTime, 2)
		myGui:setLabelWidgetText('fpsLabel', 'FPS=' .. tostring(fps) .. ', dt:' .. tostring(round(deltaTime, 4)) .. 's')
		
		myGui:setLabelWidgetText('cameraInfo', 'Pos = (' .. tostring(round(myGui:cameraX())) .. ',' .. tostring(round(myGui:cameraZ())) .. ')')
	end,
}
tick = testScene.OnTick -- just in case tick() gets called, make sure we point it to something