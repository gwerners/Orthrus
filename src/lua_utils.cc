
#include <sys/types.h>
#include <unistd.h>

//libuv
#include "uv.h"
#include "task.h"
#include "logger.h"

#include "lua_utils.h"

void close_state(lua_State **L)
{
   lua_close(*L);
}

int LogWithColor(lua_State *L)
{
   int ret = lua_isboolean(L, 1);
   if(ret){
      ret= lua_toboolean(L, 1);
      if(ret)
         EnabledLog |= LOG_COLOR;
      else
         EnabledLog &= ~LOG_COLOR;

      log_debug("LogWithColor[%s]",ret?"true":"false");

   }else{
      log_debug("LogWithColor not boolean!");
   }
   return 0;
}

int PrintPID(lua_State *L)
{
   log_debug("PID : %d", getpid());
   log_debug("Parent's PID: %d", getppid());
   return 0;
}

static void
WalkParameters(lua_State *L, const char *name, int *registry, int n,char *buffer)
{
   int i;
   strcat(buffer,name);

   for (i = 0; i < n; ++i)
   {
      lua_rawgeti(L, LUA_REGISTRYINDEX, registry[i]);
      strcat(buffer,lua_tostring(L, -1));
   }
   free(registry);
}

int LuaBuildLogLine(lua_State *L,char *buffer)
{
   const char *NAME = luaL_checkstring(L, 1);
   int *registry;
   int i, n;

   /* remove the name parameter from the stack */
   lua_remove(L, 1);

   /* check how many arguments are left */
   n = lua_gettop(L);

   /* create an array of registry entries */
   registry = (int*)calloc(n, sizeof (int));
   for (i = n; i > 0; --i)
      registry[i-1] = luaL_ref(L, LUA_REGISTRYINDEX);

   WalkParameters(L, NAME, registry, n, buffer);

   return 0;
}


int LuaLogDebug(lua_State *L)
{
   char buffer[1024];
   buffer[0]='\0';
   if(EnabledLog & LOG_DEBUG)
   {
      LuaBuildLogLine(L,(char*)&buffer);
      log_debug("<lua_script>%s<lua_script>",buffer);
   }
   return 0;
}

int LuaLogError(lua_State *L)
{
   char buffer[1024];
   buffer[0]='\0';
   if(EnabledLog & LOG_ERROR)
   {
      LuaBuildLogLine(L,(char*)&buffer);
      log_error("<lua_script>%s<lua_script>",buffer);
   }
   return 0;
}

int LuaLogWarning(lua_State *L)
{
   char buffer[1024];
   buffer[0]='\0';

   if(EnabledLog & LOG_WARNING)
   {
      LuaBuildLogLine(L,(char*)&buffer);
      log_warning("<lua_script>%s<lua_script>",buffer);
   }
   return 0;
}

int LuaLogInfo(lua_State *L)
{
   char buffer[1024];
   buffer[0]='\0';
   if(EnabledLog & LOG_INFO)
   {
      LuaBuildLogLine(L,(char*)&buffer);
      log_info("<lua_script>%s<lua_script>",buffer);
   }
   return 0;
}

/*static void iterate_and_print(lua_State *L, int index,int tab)
{
   int i;
   // Push another reference to the table on top of the stack (so we know
   // where it is, and this function can work for negative, positive and
   // pseudo indices
   lua_pushvalue(L, index);
   // stack now contains: -1 => table
   lua_pushnil(L);
   // stack now contains: -1 => nil; -2 => table
   while (lua_next(L, -2))
   {
      // stack now contains: -1 => value; -2 => key; -3 => table
      // copy the key so that lua_tostring does not modify the original
      lua_pushvalue(L, -2);
      // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
      const char *key = lua_tostring(L, -1);
      const char *value = lua_tostring(L, -2);
      _log_debug_("%s","\n");
      for(i=0;i<tab;i++)
      {
         _log_debug_("%s","\t");
      }
      _log_debug_("%s => %s", key, value);
      // pop value + copy of key, leaving original key
      if(lua_istable(L, -2))
         iterate_and_print(L,-2,tab+1);
      lua_pop(L, 2);
      // stack now contains: -1 => key; -2 => table
   }
   // stack now contains: -1 => table (when lua_next returns 0 it pops the key
   // but does not push anything.)
   // Pop table
   lua_pop(L, 1);
   // Stack is now the same as it was on entry to this function
}
static int AddClient(lua_State *L)
{
   iterate_and_print(L, -1,0);
   return 0;
}
*/
int SetRoot(lua_State *L)
{
   const char *root = luaL_checkstring(L, 1);
   log_debug("wwwroot=%s",root);
   return 0;
}
/*
static void IterateTable(lua_State *L, int index,int line)
{
   int i;
   lua_pushvalue(L, index);
   lua_pushnil(L);
   while (lua_next(L, -2))
   {
      lua_pushvalue(L, -2);
      const char *key = lua_tostring(L, -1);
      const char *value = lua_tostring(L, -2);
      if(value)
      {
         log_debug("[%d]%s => %s",line, key, value);
      }
      if(lua_istable(L, -2))
         IterateTable(L,-2,line++);
      lua_pop(L, 2);
   }
   lua_pop(L, 1);
}

static int RunHandlers(lua_State *L)
{
   IterateTable(L,1,0);
   return 0;
}

*/

int EnableLog(lua_State *L)
{
   int bit = luaL_checkint(L,1);
   switch(bit)
   {
      case LOG_DEBUG:  EnabledLog |= LOG_DEBUG;break;//    (0x1 << 0)
      case LOG_ERROR:  EnabledLog |= LOG_ERROR;break;//    (0x1 << 1)
      case LOG_WARNING:EnabledLog |= LOG_WARNING;break;//  (0x1 << 2)
      case LOG_INFO:   EnabledLog |= LOG_INFO;break;//     (0x1 << 3)
      case LOG_COLOR:  EnabledLog |= LOG_COLOR;break;//    (0x1 << 4)
   }
   return 0;
}
int DisableLog(lua_State *L)
{
   int bit = luaL_checkint(L,1);
   switch(bit)
   {
      case LOG_DEBUG:  EnabledLog &= ~LOG_DEBUG;break;//    (0x1 << 0)
      case LOG_ERROR:  EnabledLog &= ~LOG_ERROR;break;//    (0x1 << 1)
      case LOG_WARNING:EnabledLog &= ~LOG_WARNING;break;//  (0x1 << 2)
      case LOG_INFO:   EnabledLog &= ~LOG_INFO;break;//     (0x1 << 3)
      case LOG_COLOR:  EnabledLog &= ~LOG_COLOR;break;//    (0x1 << 4)
   }
   return 0;
}
