-- Scene file for testScene

-- declare our scene table
testScene = {
	SceneTransition = {
		To = nil,
	},

	WidgetCallbacks = {
		exitButton = function(signal)
			testScene.SceneTransition.To = 'exit'
			Gui:transitionScene()
		end,
		testButton = function(signal)
			testScene.SceneTransition.To = 'testScene'
			Gui:transitionScene()
		end,
		vsync_check = function(signal)
			LOG('Detected ' .. signal .. ' on checkbox')
		end,
	},

	OnCreate = function()
		LOG('Create scene "' .. Gui.name .. '"')
		Gui:setCallbackTable('WidgetCallbacks')
		
		Gui:addNewButton('exitButton')
		Gui:setWidgetPositionPercent('exitButton', '0%', '14%')
		Gui:setButtonWidgetText('exitButton', 'Close')
		Gui:registerWidgetCallback('exitButton', 'pressed')
		
		Gui:addNewButton('testButton')
		Gui:setWidgetPositionPercent('testButton', '10%', '14%')
		Gui:setButtonWidgetText('testButton', 'Switch Scene')
		Gui:registerWidgetCallback('testButton', 'pressed')
		
		Gui:addNewLabel('fpsLabel')
		Gui:setWidgetPositionPercent('fpsLabel', '0%', '10%')
		Gui:setLabelWidgetText('fpsLabel', 'Hello World!')
		
		Gui:addNewLabel('cameraInfo')
		Gui:setWidgetPositionPercent('cameraInfo', '0%', '5%')
		Gui:setLabelWidgetText('cameraInfo', 'Hello World!')
		
		Gui:addNewCheckBox('vsync_check')
		Gui:setWidgetPositionPercent('vsync_check', '0%', '17%')
		Gui:setCheckBoxWidgetText('vsync_check', 'Use Vsync')
		Gui:registerWidgetCallback('vsync_check', 'checked')
		Gui:registerWidgetCallback('vsync_check', 'unchecked')
	end,
	
	OnTick = function(deltaTime)
		local fps = round(1/deltaTime, 2)
		Gui:setLabelWidgetText('fpsLabel', 'FPS=' .. tostring(fps) .. ', dt:' .. tostring(round(deltaTime, 4)) .. 's')
		
		Gui:setLabelWidgetText('cameraInfo', 'Pos = (' .. tostring(round(Gui:cameraX())) .. ',' .. tostring(round(Gui:cameraZ())) .. ')')
	end,
}
tick = testScene.OnTick -- just in case tick() gets called, make sure we point it to something