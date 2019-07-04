/**

File: LuaEngine.cpp
Description:

Wraps a Lua instace and executes code for game logic

*/
#include "LuaEngine.h"

LuaEngine::LuaEngine() {
	// Init the bind
	luabridge_bind();
}

void LuaEngine::luabridge_bind() {
	/* 
	Bind LuaPlugin class and functions
	*/
	luabridge::getGlobalNamespace(L.getState())
		.beginClass<LuaPlugin>("LuaPlugin")
		.addConstructor<void(*)(), RefCountedPtr<LuaPlugin> /* creation policy */ >()
		.addFunction("lua_testFunction", &LuaPlugin::lua_testFunction)
		.endClass();
}

