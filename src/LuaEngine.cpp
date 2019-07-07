/**

File: LuaEngine.cpp
Description:

Wraps a Lua instace and executes code for game logic

*/
#include "LuaEngine.h"

using namespace darksun;

LuaEngine::LuaEngine() {
	//lua_log("LuaEngine init");

	// Init the bind
	luabridge_bind();

	// Load the init code
	validEngine = initConnection();

	//dlua.log("Engine value = " + BoolToString(validEngine));
}

void LuaEngine::tick(float deltaTime) {
	if (!validEngine) {// Only continue if the engine is valid
		dlua.error("Invalid engine, destroy!");
		return;
	}

	try {
		LuaRef t = getGlobal(L.getState(), "tick");
		t(deltaTime);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("tick() error: " + what);
		dlua.log("Did you declare tick()?");
		//validEngine = false; // Fail the engine here so execution stops
	}
}

void LuaEngine::addFile(string f) {
	if (!validEngine) // Only continue if the engine is valid
		return;

	try {
		L.doFile(f);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Attempted to add file: " + what);
		validEngine = false; // Fail the engine here so execution stops
	}
}

void LuaEngine::addFilesRecursive(string p, string ext) {
	dlua.log("Recursive add files:");

	if (!std::filesystem::exists(p)) {
		dlua.error(" - Attempted to find files in path '" + p + "', but the path does not exist");
		return;
	}

	std::vector<std::string> allFiles(0);
	recursiveListFiles(p, &allFiles);

	if (allFiles.size() == 0) {
		// No files detected
		dlua.log(" - No files found on path '" + p + "'!");
		return;
	}

	for (std::string &fpath : allFiles) {
		std::filesystem::path file(fpath);
		std::string extension = file.extension().generic_string();

		if (extension.compare(ext) == 0) {
			// We have a valid file, register
			dlua.log(" - Registering file '" + fpath + "'");
			addFile(fpath);
		}
	}

	// Done
}

void LuaEngine::recursiveListFiles(std::string path, std::vector<std::string> *list) {
	for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(path)) {
		list->push_back(entry.path().generic_string());
		if (entry.is_directory()) {
			recursiveListFiles(entry.path().generic_string(), list);
		}
	}
}

void LuaEngine::doString(string s) {
	try {
		L.doString(s);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("LuaEngine.doString() failure: " + what);
	}
}

void LuaEngine::luabridge_bind() {
	/* 
	Bind all the classes and functions here
	*/
	try {
		luabridge::getGlobalNamespace(L.getState())
			.beginNamespace("darksun")
				.addProperty("luaDir", lua_getLuaDir)
			.endNamespace()
			.addFunction("LOG", lua_log);
	}
	catch (std::exception& e) {
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
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Self test file load error: " + what);
		return false;
	}
	return true;
}

lua::State* LuaEngine::getState() {
	return &L;
}

void lua_log(const char * s) {
	dlua.log(s);
}

string lua_getLuaDir() {
	return luaDir;
}

