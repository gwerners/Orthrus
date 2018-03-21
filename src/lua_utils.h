/*
Copyright 2018 Gabriel Wernersbach Farinas

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LUA_UTILS_H_
#define _LUA_UTILS_H_

// lua
extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#define cleanup(x) __attribute__((cleanup(x)))
#define auto_lclose cleanup(close_state)

void
close_state(lua_State** L);
int
LogWithColor(lua_State* L);
int
PrintPID(lua_State* L);
int
LuaBuildLogLine(lua_State* L, char* buffer);

int
LuaLogDebug(lua_State* L);
int
LuaLogError(lua_State* L);
int
LuaLogWarning(lua_State* L);
int
LuaLogInfo(lua_State* L);

int
SetRoot(lua_State* L);
int
EnableLog(lua_State* L);
int
DisableLog(lua_State* L);
#endif
