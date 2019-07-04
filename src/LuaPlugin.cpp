/**

File: LuaPlugin.cpp
Description:

Default C++/Lua plugin functions

*/
#include "LuaPlugin.h"

int LuaPlugin::lua_testFunction() {
	cout << "Hello world!" << endl;
	return 0;
}