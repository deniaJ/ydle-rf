/*
 * LuaStack.h
 *
 *  Created on: Jan 31, 2014
 *      Author: denia
 */
#include <lua5.1/lua.hpp>

#ifndef LUASTACK_H_
#define LUASTACK_H_

namespace ydle {

class LuaStack {
public:
	LuaStack();
	virtual ~LuaStack();
	void dataReceived();
	void runScript(std::string script);
private:
	lua_State *L;
};

} /* namespace ydle */

#endif /* LUASTACK_H_ */
