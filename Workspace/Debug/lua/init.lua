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

function round(num, numDecimalPlaces)
  local mult = 10^(numDecimalPlaces or 0)
  return math.floor(num * mult + 0.5) / mult
end

blueprints = {}

-- Setup the access to importing
function import(file)
	myEngine:import(file)
end
