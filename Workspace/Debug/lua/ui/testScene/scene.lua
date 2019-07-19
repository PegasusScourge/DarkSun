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
			LOG('Spawning 10 entities')
			for i=0,9,1 do
				Scene:spawnEntity('test', 0, 0, i)
			end
		end,
		vsync_check = function(signal)
			if signal == 'Checked' then
				LOG('Vsync on')
				Settings.opengl_vsync = true
			else
				LOG('Vsync off')
				Settings.opengl_vsync = false
			end
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
		Gui:setButtonWidgetText('testButton', 'Spawn 10 entities')
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
		
		-- create lighting
		Scene:setLightPosition(1, 0.0, 5.0, 0.0)
		Scene:setLightColor(1, 0.0, 4.0, 0.0)
		Scene:setLightAttenuation(1, true)
		LOG('Lighting set')
	end,
	
	OnTick = function(deltaTime)
		local fps = round(1/deltaTime, 2)
		Gui:setLabelWidgetText('fpsLabel', 'FPS=' .. tostring(fps) .. ', dt:' .. tostring(round(deltaTime, 4)) .. 's')
		
		Gui:setLabelWidgetText('cameraInfo', 'Pos = (' .. tostring(round(Gui:cameraX())) .. ',' .. tostring(round(Gui:cameraZ())) .. ')')
	end,
}
tick = testScene.OnTick -- just in case tick() gets called, make sure we point it to something