/*
 * LuaStack.cpp
 *
 *  Created on: Jan 31, 2014
 *      Author: denia
 */
#include <iostream>
#include "protocolRF.h"

#include "LuaStack.h"

extern protocolRF *g_ProtocolRF;

extern "C" {
  static int l_cppfunction(lua_State *L) {
    double arg = luaL_checknumber(L,1);
    lua_pushnumber(L, arg * 0.5);
    return 1;
  }

  static int l_addDataToFrame(lua_State *L){
	  int type = luaL_checkinteger(L, 1);
	  int value = luaL_checkinteger(L, 2);
	  std::cout << type << " : " << value;
	  g_ProtocolRF->addData(type, value);
	  return 1;
  }

  static int l_createNewFrame(lua_State *L){
	  int dest = luaL_checknumber(L, 1);
	  int source = luaL_checknumber(L, 2);
	  int type = luaL_checknumber(L, 3);
	  g_ProtocolRF->dataToFrame(dest, source, type);
  }

  static int l_addCmd(lua_State *L){
	  int type = luaL_checkinteger(L, 1);
	  int value = luaL_checkinteger(L, 2);
	  std::cout << type << " : " << value;
	  g_ProtocolRF->addCmd(type, value);
	  return 1;
  }

  static int l_send(lua_State *L){
	  g_ProtocolRF->transmit();
	  return 1;
  }
  static int l_extractInt(lua_State *L){
	  int pos = luaL_checknumber(L, 1);
	  int data=0;
	  g_ProtocolRF->extractData(pos, data);
	  lua_pushinteger(L, data);
	  return 1;
  }
  static int l_extractFloat(lua_State *L){
	  int pos = luaL_checknumber(L, 1);
	  float data=0;
	  g_ProtocolRF->extractData(pos, data);
	  lua_pushnumber(L, data);
	  return 1;
  }
}


namespace ydle {

LuaStack::LuaStack() {
	L = luaL_newstate();
}

void LuaStack::dataReceived(){
    lua_getglobal(L, "receive");  /* function to be called */
    // No args, no return
    lua_pcall(L, 0, 0, 0);
}

void LuaStack::runScript(std::string script){
	luaL_openlibs(L);

	if (luaL_loadfile(L, script.c_str())) {
		std::cerr << "Something went wrong loading the chunk (syntax error?)" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L,1);
	}

	lua_pushcfunction(L, l_send);
	lua_setglobal(L, "send");

	lua_pushcfunction(L, l_createNewFrame);
	lua_setglobal(L, "createNewFrame");

	lua_pushcfunction(L, l_addDataToFrame);
	lua_setglobal(L, "addDataToFrame");

	lua_pushcfunction(L, l_extractInt);
	lua_setglobal(L, "extractInt");

	lua_pushcfunction(L, l_extractFloat);
	lua_setglobal(L, "extractFloat");

	lua_pushcfunction(L, l_addCmd);
	lua_setglobal(L, "addCmd");

	if (lua_pcall(L,0, LUA_MULTRET, 0)) {
		std::cerr << "Something went wrong during execution" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L,1);
	}
}

LuaStack::~LuaStack() {
	lua_close(L);
}

} /* namespace ydle */
