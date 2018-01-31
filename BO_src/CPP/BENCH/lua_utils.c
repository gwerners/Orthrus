
#include "logger.h"
#include "lua_utils.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *progname = "Lua_worker";
int x;
#define HOW_MANY_TIMES 98778

lua_State *L=NULL;

int LogWithColor(lua_State *L);
int LuaBuildLogLine(lua_State *L,char *buffer);

int LuaLogDebug(lua_State *L);
int LuaLogError(lua_State *L);
int LuaLogWarning(lua_State *L);
int LuaLogInfo(lua_State *L);

int EnableLog(lua_State *L);
int DisableLog(lua_State *L);


static int SetX(lua_State *L) {
  x = luaL_checkint(L,1);
  return 0;
}
static int GetX(lua_State *L) {
  lua_pushinteger(L,x);
  return 1;
}

static void l_message (const char *pname, const char *msg) {
    if (pname)
        fprintf(stderr, "%s: ", pname);
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
}
static int report (lua_State *L, int status) {
    if (status && !lua_isnil(L, -1)) {
        const char *msg = lua_tostring(L, -1);
        if (msg == NULL)
            msg = "(error object is not a string)";
        l_message(progname, msg);
        lua_pop(L, 1);
    }
    return status;
}
static int traceback (lua_State *L) {
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);  
    lua_pushinteger(L, 2);  
    lua_call(L, 2, 1);  
    return 1;
}
static int docall (lua_State *L, int narg, int clear) {
    int status;
    int base = lua_gettop(L) - narg;  /* function index */
    lua_pushcfunction(L, traceback);  /* push traceback function */
    lua_insert(L, base);  /* put it under chunk and args */
    status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
    lua_remove(L, base);  /* remove traceback function */
    /* force a complete garbage collection in case of errors */
    if (status != 0)
        lua_gc(L, LUA_GCCOLLECT, 0);
    return status;
}
static int execute_script (lua_State *L,const char *script_name, int n) {
  int status;
  status = luaL_loadfile(L,script_name);
  if (status == 0)
    status = docall(L, 0, 0);
  else
    lua_pop(L, 0);      
  return report(L, status);
}

void Run_Lua()
{
    int i;
    lua_State *l;
    l = lua_open();
    luaL_openlibs(l);

	//Get/Set for integer
	lua_register(l, "SetX", SetX);
   lua_register(l, "GetX", GetX);
	
	//set x
	x = 0;

	int status;

	//execute script
   //change current dir to scripts dir!
   if(chdir("scripts"))
   {
      log_info("problem changing current dir to 'scripts' to run lua files!");
   }

	status = luaL_loadfile(l,"init.lua");
   lua_setglobal(l,"bogus");
	if (status != 0)
	{
		lua_pop(l, 0);      
		printf ("Error loading file\n");
	}
   for(i=0;i<HOW_MANY_TIMES;i++){
      lua_getglobal(l,"bogus");
      lua_pcall( l, 0, 0, 0 );  // .. or whatever args you use to pcall
   }
	report(l, status);
#ifndef DISABLE_PRINTING
	printf("x value after script [%d]\n",x);
#endif
	//Remember to destroy the Lua State 
    lua_close(l);

}



char *ReadFile(const char *script_name)
{
	char *str_buff;
	
	struct stat stat_buffer;
	size_t file_size,numread;
	FILE *input;
	if(stat(script_name,&stat_buffer)!=0)
    {
       return NULL;
    } 
	input = fopen(script_name, "r");
	file_size=stat_buffer.st_size;
	str_buff=(char*)malloc(file_size+1);
    numread = fread((char*)str_buff,sizeof(char),file_size,input);
	str_buff[numread]='\0';
    if(numread<=0)
    {
		free(str_buff);
		return NULL;
    }
	fclose(input);
	return str_buff;
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

static int SetHTML(lua_State *L) {
  const char *page = luaL_checkstring(L, 1);
  log_debug("page is[%s]",page);
  return 0;
}
char * RunLuaPage(const char*func)
{
   char *page;
   int ret;
   if(L)
   {
      lua_getglobal(L, func); 
      if( lua_isfunction(L, -1 ) ){
         log_debug("é funcao!!");
      }
      ret = lua_pcall(L, 0, 1, 0);
      if (ret !=0)
      {
         const char* r404="HTTP/1.1 404 Not Found\r\nContent-Length: 35\r\nConnection: close\r\n\r\n<html><h1>404 NOT FOUND</h1></html>\r\n";
         //log_error("%s",lua_tostring(L, -1));
         lua_pop(L, 0); 
         return strdup(r404);
      }else{
         //log_debug("%s",lua_tostring(L, -1));
         page=strdup(lua_tostring(L, -1));
         lua_pop(L, 1); 
         return page;
      }
   }
   return NULL;
}
int RunConfiguration(const char *filename)
{
   int ret;
   char *path;
   L = luaL_newstate();

   if (!L)
   {
      log_error("unable to create new lua state");
      return 1;
   }

   luaL_openlibs(L); // Open standard libraries

   lua_register(L, "LogWithColor", LogWithColor);

   lua_register(L, "LogDebug", LuaLogDebug);
   lua_register(L, "LogError", LuaLogError);
   lua_register(L, "LogWarning", LuaLogWarning);
   lua_register(L, "LogInfo", LuaLogInfo);

   lua_register(L, "EnableLog", EnableLog);
   lua_register(L, "DisableLog", DisableLog);

   lua_register(L, "SetHTML", SetHTML);

   path = getwd(NULL);
   log_debug("current dir",path);
   free(path);
   //change current dir to scripts dir!
   if(chdir("BO/CPP/BENCH/scripts"))
   {
      log_info("problem changing current dir to 'scripts' to run lua files!");
   }

	ret = luaL_loadfile(L,filename);
   lua_setglobal(L,"GetPerformancePage");
	if (ret != 0)
	{
		lua_pop(L, 0);      
		log_error ("Error loading file\n");
	}

   return 0;
}

