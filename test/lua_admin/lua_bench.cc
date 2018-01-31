#include <string.h>
#include <iostream>
#include <unistd.h>

int x;

//#define DISABLE_PRINTING
#define HOW_MANY_TIMES 66778


static const char *progname = "Lua_admin";


extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

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

   chdir("/home/gabrielfarinas/M4U_Interfaces/server/test/lua_admin");
   status = luaL_loadfile(l,"admin.lua");
   lua_setglobal(l,"bogus");
   if (status != 0)
   {
      lua_pop(l, 0);      
      printf ("Error loading file\n");
   }
   for(int i=0;i<HOW_MANY_TIMES;i++)
   {
      //for( int i = 0; i < NUM_TIMES_TO_CALL_SCRIPT; i++ ) {
      //int top = lua_gettop(l);
      lua_getglobal(l,"bogus");
      lua_pcall( l, 0, 0, 0 );  // .. or whatever args you use to pcall
      //lua_settop(l, top);
   }
   //status = docall(l, 0, 0);


   report(l, status);



#ifndef DISABLE_PRINTING
	printf("x value after script [%d]\n",x);
#endif
	//Remember to destroy the Lua State 
    lua_close(l);

}

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

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

int main(int argc, char** argv)
{
   printf("gwf bogus\n");
   Run_Lua();
	return 0;
}
