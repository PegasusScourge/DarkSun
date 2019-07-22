# DarkSun

SFML + OpenGL RTS game engine

## Pegasus' notes to himself:

 - Never allow lua to call a c++ function that interacts with mutexes if possible, it will auto-destruct the game

## Versions of software used to build this
SFML: version SFML-2.5.1-vc-64-bit (2.5.1)

Lua: version 5.3.5

LuaBridge: (https://github.com/vinniefalco/LuaBridge/) version 2.3.2

LuaState: (https://github.com/AdUki/LuaState) version 2.1

Assimp: 4.1.0

GLM: 0.9.9.5

TGUI: 0.8.5

## Resources
### LuaBridge + LuaState
LuaBridge + LuaState question: https://stackoverflow.com/questions/25498277/binding-c-functions-calling-lua-functions-with-class-as-parameter

Example of above: https://github.com/d-led/lua_cpp_tryout/blob/25498277/try_luabridge.cpp

LuaState: https://github.com/AdUki/LuaState

Info on bindings: http://lua-users.org/wiki/BindingCodeToLua

LuaBridge: https://github.com/vinniefalco/LuaBridge

TGUI: https://tgui.eu/

### OpenGL
OpenGL info thread: https://en.sfml-dev.org/forums/index.php?topic=20906.0

OpenGL tutorial (basis for code): https://open.gl/introduction

OpenGL tutorial: https://learnopengl.com/

Assimp loading tutorial: https://learnopengl.com/Model-Loading/Assimp

Terrain and heightmaps tutorial (Java but is portable): https://lwjglgamedev.gitbooks.io/3d-game-development-with-lwjgl/content/chapter14/chapter14.html

Extra shadow tutorial: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

