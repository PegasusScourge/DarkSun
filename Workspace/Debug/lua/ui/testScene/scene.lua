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
			LOG('Spawning entities')
			for i=0,2,1 do
				Scene:spawnEntity('test', 30, 20, 30)
			end
		end,
		vsync_check = function(signal)
			if signal == 'Checked' then
				LOG('Vsync on')
				Settings.set_opengl_vsync(true)
			else
				LOG('Vsync off')
				Settings.set_opengl_vsync(false)
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
		
		Gui:addNewLabel('lightInfo')
		Gui:setWidgetPositionPercent('lightInfo', '0%', '25%')
		Gui:setLabelWidgetText('lightInfo', 'Hello World!')
		
		Gui:addNewCheckBox('vsync_check')
		Gui:setWidgetPositionPercent('vsync_check', '0%', '17%')
		Gui:setCheckBoxWidgetText('vsync_check', 'Use Vsync')
		Gui:registerWidgetCallback('vsync_check', 'checked')
		Gui:registerWidgetCallback('vsync_check', 'unchecked')
		
		-- create lighting
		Scene:setLightPosition(1, 12.0, 20.0, 4.0)
		Scene:setLightColor(1, 1.0, 1.0, 1.0)
		Scene:setLightAttenuation(1, true)
		LOG('Lighting set')
		
		-- create the camera settings
		Scene:setCameraEnabled(true)
	end,
	
	sinArg = 0,
	
	OnTick = function(deltaTime)
		local fps = round(1/deltaTime, 2)
		Gui:setLabelWidgetText('fpsLabel', 'FPS=' .. tostring(fps) .. ', dt:' .. tostring(round(deltaTime, 4)) .. 's')
		
		Gui:setLabelWidgetText('cameraInfo', 'Pos = (' .. tostring(round(Gui:cameraX())) .. ',' .. tostring(round(Gui:cameraZ())) .. ')')
		
		-- Scene:setLightPosition(1, 40.0 + (math.sin(testScene.sinArg) * 20.0), 20.0, 40.0 + (math.cos(testScene.sinArg) * 20.0))
		Scene:setLightPosition(1, 40, 20.0 + (math.cos(testScene.sinArg) * 20.0), 40.0)
		testScene.sinArg = testScene.sinArg + (1 * deltaTime)
		
		Gui:setLabelWidgetText('lightInfo', 'Light YPos = ' .. tostring(Scene:getLightPosition(1).y))
	end,
}
tick = testScene.OnTick -- just in case tick() gets called, make sure we point it to something