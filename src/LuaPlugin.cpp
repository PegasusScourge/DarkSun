/**

File: LuaPlugin.cpp
Description:

Default C++/Lua plugin functions

*/
#include "LuaPlugin.h"

using namespace darksun;

void LuaPlugin::helloworld() {
	dlua.log("Hello world!");
}