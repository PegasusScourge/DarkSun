/**

File: LuaEngine.cpp
Description:

Wraps a Lua instace and executes code for game logic

*/
#include "LuaEngine.h"

using namespace darksun;

LuaEngine::LuaEngine() {
	lua_log("LuaEngine init");

	// Init the bind
	luabridge_bind();

	// Load the init code
	validEngine = initConnection();

	dlua.log("Engine value = " + BoolToString(validEngine));
}

void LuaEngine::tick() {
	if (!validEngine) {// Only continue if the engine is valid
		dlua.error("Invalid engine, destroy!");
		return;
	}

	try {
		LuaRef t = getGlobal(L.getState(), "tick");
		t();
	}
	catch (exception& e) {
		string what = e.what();
		dlua.error("tick() error: " + what);
		dlua.log("Did you declare tick()?");
		validEngine = false; // Fail the engine here so execution stops
	}
}

void LuaEngine::addFile(string f) {
	if (!validEngine) // Only continue if the engine is valid
		return;

	try {
		L.doFile(lua_getLuaDir() + f);
	}
	catch (exception& e) {
		string what = e.what();
		dlua.error("Attempted to add file: " + what);
		validEngine = false; // Fail the engine here so execution stops
	}
}

void LuaEngine::luabridge_bind() {
	/* 
	Bind all the classes and functions here
	*/
	try {
		luabridge::getGlobalNamespace(L.getState())
			.beginNamespace("darksun")
				.beginClass<LuaPlugin>("LuaPlugin")
					.addConstructor<void(*)(), RefCountedPtr<LuaPlugin> /* creation policy */ >()
					.addFunction("helloworld", &LuaPlugin::helloworld)
				.endClass()
				.addProperty("luaDir", lua_getLuaDir)
			.endNamespace()
			.addFunction("LOG", lua_log);
	}
	catch (exception& e) {
		string what = e.what();
		dlua.error("Failed binding proccess: " + what);
		return;
	}
	
	//dlua.log("Survived the binding process");
}

bool LuaEngine::initConnection() {
	try {
		L.doFile(lua_getLuaDir() + initFile);
	}
	catch (exception& e) {
		string what = e.what();
		dlua.error("Self test file load error: " + what);
		return false;
	}
	return true;
}

void lua_log(const char * s) {
	dlua.log(s);
}

string lua_getLuaDir() {
	return luaDir;
}

