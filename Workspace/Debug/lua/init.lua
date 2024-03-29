-- *****************************************************
-- init.lua
-- *****************************************************
-- Initialisation file for in-lua functions etc. that are loaded for all engines
--
--
--

-- add the recursive function for listing
function recursiveList(object)
	for key, value in pairs(object) do
		LOG("Found '" .. tostring(key) .. "' with value '" .. tostring(value) .. "'")
		if type(value) == "table" then
			recursiveList(value)
		end
	end
end

-- rounds a number to the specified number of decimal places
function round(num, numDecimalPlaces)
	local mult = 10^(numDecimalPlaces or 0)
	return math.floor(num * mult + 0.5) / mult
end

-- Counts the number of elements in a table
function tablelength(T)
	local count = 0
	for _ in pairs(T) do count = count + 1 end
	return count
end

-- Setup the blueprints table
blueprints = {}

-- Setup the access to importing
function import(file)
	LuaEngine:import(file)
end

-- Setup the audio access
Audio = {
	playSound = function(self, soundPath, category, posX, posY, posZ, shouldLoop)
		LuaEngine:playSound(soundPath, category, posX, posY, posZ, shouldLoop)
	end,
	
	newCategory = function(self, category)
		LuaEngine:newSoundCategory(category)
	end,
	
	setCategoryVolume = function(self, category, newVolume)
		LuaEngine:setSoundCategoryVolume(category, newVolume)
	end,
	
	setCategoryAttenuation = function(self, category, newAttentuationFactor)
		LuaEngine:setSoundCategoryAttenuation(category, newAttentuationFactor)
	end,
}
