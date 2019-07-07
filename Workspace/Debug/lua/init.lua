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

blueprints = {}

