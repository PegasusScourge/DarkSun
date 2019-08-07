# Lua Documentation

This documentation covers the available functions and variables that an instance may access.
The following are the available instances:
 - Entity blueprints
 - Entity scripts
 - Maps
 - Scene scripts
 
This documentation is presented as follows:
 - \<type\>: name[(arguments)]
 
Functions are called on tables using the : system, not the .

e.g. "Audio:playSound(xyz)"

## All instances (These are auto-imported)
 - function: LOG(msg)															--> Prints a message to the lua log
 - table: Audio																	--> Table that provides an interface to the audio system of the engine
   - function: playSound(soundPath, category, posX, posY, posZ, shouldLoop)		--> Plays a sound at path soundPath at the specified position with the presets of the sound category
   - function: newCategory(category)											--> Creates a new category with the name category
   - function: setCategoryVolume(category, newVolume)							--> Sets the volume of category to the volume specified (0 =< x =< 100)
   - function: setCategoryAttenuation(category, newAttentuationFactor)			--> Sets the attentuation of the category to the specified factor (x > 0)
 - function: recursiveList(object)												--> Prints to the lua log the type and content and value of the object, recursively if a table
 - function: round(num, numDecimalPlaces)										--> Takes a number and rounds it to the specified number of decimal places then returns it
 - function: tablelength(T) 													--> Returns the number of elements in the table T
 - function: import(path)														--> Loads the specified file

## Entity blueprints
 - table: blueprints															--> Table for blueprints to be entered into. Use "blueprints.\<bpName\>" to get the blueprint information

## Entity scripts
 - table: thisEntity															--> The entity this script is running on
   - string: internalName														--> The internal reference name of a blueprint used by the game engine (currently analagous to the blueprint name)
   - string: displayName														--> The name of the entity displayed on the UI
   - string: bpName																--> The name of the blueprint this entity was built from (currently analagous to the blueprint name)
   - number: id																	--> The id of this entity (unique)
   - function: moveTo(x, y, z)													--> Makes the entity pathfind to the specified position
   - function: rotateTo(x, y, z)												--> Points the entity at the position specified
   - function: isPathfinding()													--> Returns true/false if the entity is currently conducting pathfinding operations to a location
 - table: EntityOrders															--> Provides a table of "enums" that specifiy the order type for entity orders. Not in use, present for future use
   - number: ORDER_STOP															--> Represents the "stop" order
   - number: ORDER_ASSIST														--> Represents the "assist" order
   - number: ORDER_ATTACK														--> Represents the "attack" order
   - number: ORDER_MOVE															--> Represents the "move" order
   - number: ORDER_REPAIR														--> Represents the "repair" order

## Maps
 - N/A 
 
## Scene Scripts
 - table: EntityOrders															--> Provides a table of "enums" that specifiy the order type for entity orders. Not in use, present for future use
   - number: ORDER_STOP															--> Represents the "stop" order
   - number: ORDER_ASSIST														--> Represents the "assist" order
   - number: ORDER_ATTACK														--> Represents the "attack" order
   - number: ORDER_MOVE															--> Represents the "move" order
   - number: ORDER_REPAIR														--> Represents the "repair" order
 - table: Scene																	--> The scene this script is running on
   - function: spawnEntity(bpName, x, y, z)										--> Spawns an entity from the blueprint specified at the coordinates given
   - function: killEntity(id)													--> Kills the entity with the coresponding id, removing it from the game
   - function: getPercentLoaded()												--> Returns the loading percentage of the scene (when loading terrain etc). Only used in the Loading Scene
   - function: setLightPosition(lightNumber, x, y, z)							--> Places the specified light at the coordinates given
   - function: setLightColor(lightNumber, r, g, b)								--> Sets the color of the specified light
   - function: setLightAttenuation(lightNumber, attentuation)					--> Sets the attenuation factor of the light specified
   - function: getLightPosition(lightNumber)									--> Returns the light position as a vector
   - function: getLightColor(lightNumber)										--> Returns the color of the light as a vector
   - function: getLightAttenuation(lightNumber)									--> Returns the attentuation factor of the specified light
   - function: setCameraEnabled(enabled)										--> Enables/disables the camera functionality allowing for "static" scenes
   - function: setTacticalZoomSettings(min, max, xDelta)						--> Sets the minimum zoom height (y units) and maximum zoom height (y units) and the distance behind the focused point at minimum zoom (xDelta)
   - function: hasMap()															--> Returns if a map is loaded (true/false). Useful to determine if the Map table is present
 - table: Map																	--> Table representing the map attached to this scene. ONLY PRESENT IF A MAP IS LOADED WITH THIS SCENE
   - function: sizeX()															--> Returns the x dimensions of the map
   - function: sizeY()															--> Returns the y dimensions of the map