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
#include <LuaValue.h>

#include "Log.h"

// Classes etc to be bound
#include "LuaPlugin.h"

using namespace luabridge;

/* Log a value from lua to the dlua stream */
void lua_log(const char * s);

/* Give the lua directory to lua */
string lua_getLuaDir();
static string luaDir = "lua/";

namespace darksun {

	class LuaEngine {

	private:
		const string initFile = "init.lua";

		lua::State L;

		bool validEngine = false;

		/* Function mapping for the lua engine */
		void luabridge_bind();

		/* Self test functions */
		bool initConnection();

	public:
		LuaEngine();

		/* Adds a file and executes to load its content */
		void addFile(string f);

		/* Executes the tick() function of the engine (if present) for a single tick of the engine */
		void tick();

	};

}
