-- Scene file for loading
-- Make sure to create the "loadingBar" item

-- declare our scene table
loading = {
	SceneTransition = {
		To = nil,
	},

	WidgetCallbacks = {
	},

	OnCreate = function()
		LOG('Create scene "' .. Gui.name .. '"')
		Gui:setCallbackTable('WidgetCallbacks')
		
		Gui:addNewProgressBar('loadingBar')
		
		Gui:addNewLabel('progress')
		Gui:setWidgetPositionPercent('progress', '25%', '40%')
		Gui:setLabelWidgetText('progress', '')
	end,
	
	OnTick = function(deltaTime)
		local percent = round(Scene:getPercentLoaded(), 2)
		Gui:setLabelWidgetText('progress', 'Loaded ' .. tostring(percent) .. '%')
	end,
}
tick = loading.OnTick -- just in case tick() gets called, make sure we point it to something