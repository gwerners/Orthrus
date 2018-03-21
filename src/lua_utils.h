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
