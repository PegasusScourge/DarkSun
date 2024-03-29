# DarkSun Changelog
(2019-07-23 00:39 GMT)

### Version [ALPHA][0.2.0] (unreleased)
##### Documentation
 - Added documentation for C++ code
 - Added documentation for lua scripting
##### Settings
 - Added external settings file, 'settings.lua'
 - Added 'antialiasing_level' as test value
##### OpenGL
 - Added theoretical implementation to change vertex buffer content to enable mesh deformation (map building, unit destruction etc)
##### Sounds
 - Added initial sound engine and test sound
 - Only mono sounds will be spatially rendered by SFML, moved to mono test sound to reflect this and test this
 - Sound requests now allow positional playing and respect category settings
##### General lighting
 - Fixed directional lighting behaviour being a bit odd
##### Maps
 - Tweaked the output range of maps from the heightmap data to get a more consistent output
 - Added map smoothing to produce a better output
 - Fixed 90 degree rotation of heightmaps
 - Begun exposing information to the LuaScene
 - Added sun position and color specificatin in map.lua
##### Profiler
 - Changed profiling to output at the end of each frame if applicable, instead of hogging memory in the background
 - Changed frequency from every 20th frame to 200th
##### Scenes
 - Added exposure of the following functions to lua scenes:
   - Scene:setCameraEnabled(enabled)	--> Sets the in-game camera to be enabled/disabled
   - Scene:setTacticalZoomSettings(minHeight, maxHeight, xDelta)	--> Sets the tactical zoom paramaters for the camera. xDelta is the distance the camera is at minHeight
   - Scene:getMapSizeX() --> Returns the width of the map currently loaded, or -1 if no map is loaded
   - Scene:getMapSizeY() --> Returns the height of the map currently loaded, or -1 if no map is loaded
   - EntityOrders changed from function value return to static properties 
#### Entities
 - Lua script reference for host entity of script changed to 'thisEntity' from 'myEntity' to clarify the entity being discussed 
 - Entity pathfinding fixed, still bugged by entity actually moves now
#### Lua scripts
 - Renamed base LuaEngine from 'myEngine' to 'LuaEngine'
 - Added 'Audio' table which provides a good access to the audio functions in init.lua
 - Currently unable to stop sounds currently playing

### Version [ALPHA][0.1.0]

 - First public release