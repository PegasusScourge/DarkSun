# DarkSun Changelog
(2019-07-23 00:39 GMT)

### Version [ALPHA][0.2.0] (unreleased)
#### Profiler
 - Changed profiling to output at the end of each frame if applicable, instead of hogging memory in the background
 - Changed frequency from every 20th frame to 200th
#### Scenes
 - Added exposure of the following functions to lua scenes:
   - Scene:setCameraEnabled(enabled)	--> Sets the in-game camera to be enabled/disabled
   - Scene:setTacticalZoomSettings(minHeight, maxHeight, xDelta)	--> Sets the tactical zoom paramaters for the camera. xDelta is the distance the camera is at minHeight

### Version [ALPHA][0.1.0]

 - First public release