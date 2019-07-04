#pragma once
/**

File: LuaEngine.h
Description:

Header file for LuaEngine.cpp, wrapping a Lua instace and state for running game logic

*/

#include <iostream>
#include <lua.hpp>
#include <LuaBridge.h>
#include <RefCountedPtr.h>
#include <LuaState.h>

// Classes etc to be bound
#include "LuaPlugin.h"

using namespace std;
using namespace luabridge;

class LuaEngine {

private:
	lua::State L;

	bool validEngine = false;

	/* Function mapping for the lua engine */
	void luabridge_bind();

public:
	LuaEngine();

};