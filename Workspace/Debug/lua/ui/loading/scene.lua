-- Scene file for loading
-- The "loadingBar" progress bar item is auto-created for this scene type

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
	end,
	
	OnTick = function(deltaTime)
	end,
}
tick = loading.OnTick -- just in case tick() gets called, make sure we point it to something