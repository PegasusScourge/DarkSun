#pragma once
/**

File: LuaEngine.h
Description:

Header file for LuaEngine.cpp, wrapping a Lua instace and state for running game logic

*/

#include <iostream>
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>

#include <lua.hpp>

#include <LuaBridge.h>
#include <RefCountedPtr.h>

#include <LuaState.h>
#include <LuaValue.h>

#include "Log.h"

using namespace luabridge;
using string = std::string;

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

		/* Recursive list files and return */
		void recursiveListFiles(std::string path, std::vector<std::string> *list);

		/* Import a file into the lua engine */
		void lua_import(string f);

	public:
		LuaEngine();

		/* Adds a file and executes to load its content */
		void addFile(string f);

		/* Adds files from the path recursively, looking for a specific extension */
		void addFilesRecursive(string p, string ext);

		/* Wraps a doString call on the state in a try-catch to prevent nastiness */
		void doString(string s);

		lua::State* getState();

		bool isValid() { return validEngine; }

	};

}
