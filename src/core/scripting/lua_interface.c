/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "lua_interface.h"

#include <lua.h>
#include <lauxlib.h>

// TODO: Rework this to use a hook registration system

static void luaLoadScript(struct mScriptingInterface* si, const char* script) {
	lua_State* L = (lua_State*)si->state;

	int s = luaL_loadstring(L, script); // function or "error message"
	if (s != 0) {
		mLOG(SCRIPTING, WARN, "Lua Error: %s\n", lua_tostring(L, -1)); // "error message"
		lua_pop(L, 1); // [none]
	} else {
		int s = lua_pcall(L, 0, 0, 0); // [none] or "error message"
		if (s != 0) {
			mLOG(SCRIPTING, WARN, "Lua Error: %s\n", lua_tostring(L, -1)); // "error message"
			lua_pop(L, 1); // [none]
		}
	}
}

static void luaOnReset(struct mScriptingInterface* si) {
	lua_State* L = (lua_State*)si->state;

	lua_getglobal(L, "onReset"); // onReset
	if (lua_isfunction(L, -1)) {
		int s = lua_pcall(L, 0, 0, 0); // [none] or "error message"
		if (s != 0) {
			mLOG(SCRIPTING, WARN, "Lua Error on Reset: %s\n", lua_tostring(L, -1)); // "error message"
			lua_pop(L, 1); // [none]
		}
	} else
		lua_pop(L, 1); // [none]
}

static void luaOnHalt(struct mScriptingInterface* si) {
	lua_State* L = (lua_State*)si->state;

	lua_getglobal(L, "onHalt"); // onHalt
	if (lua_isfunction(L, -1)) {
		int s = lua_pcall(L, 0, 0, 0); // [none] or "error message"
		if (s != 0) {
			mLOG(SCRIPTING, WARN, "Lua Error on Halt: %s\n", lua_tostring(L, -1)); // "error message"
			lua_pop(L, 1); // [none]
		}
	} else
		lua_pop(L, 1); // [none]
}

static void luaOnFrameEnd(struct mScriptingInterface* si) {
	lua_State* L = (lua_State*)si->state;

	lua_getglobal(L, "onFrameEnd"); // onFrameEnd
	if (lua_isfunction(L, -1)) {
		int s = lua_pcall(L, 0, 0, 0); // [none] or "error message"
		if (s != 0) {
			mLOG(SCRIPTING, WARN, "Lua Error on Frame End: %s\n", lua_tostring(L, -1)); // "error message"
			lua_pop(L, 1); // [none]
		}
	} else
		lua_pop(L, 1); // [none]
}

/*** MGBA API functions ***/

static struct mScriptingInterface* getScriptingInterface(lua_State* L) {
	lua_getfield(L, LUA_REGISTRYINDEX, "mgba"); // mgba
	lua_getfield(L, -1, "si"); // si mgba
	struct mScriptingInterface* si = lua_topointer(L, -1);
	lua_pop(L, 2);
	return si;
}

static int mgba_getGameCode(lua_State* L) {
	struct mScriptingInterface* si = getScriptingInterface(L);
	char code[16] = {0};
	si->core->getGameCode(si->core, code);
	lua_pushstring(L, code);
	return 1;
}

static int mgba_getGameTitle(lua_State* L) {
	struct mScriptingInterface* si = getScriptingInterface(L);
	char title[16] = {0};
	si->core->getGameTitle(si->core, title);
	lua_pushstring(L, title);
	return 1;
}

#define mgba_readX(width) static int mgba_read##width (lua_State* L) { \
	struct mScriptingInterface* si = getScriptingInterface(L); \
	uint32_t address = luaL_checkint(L, 1); \
	int segment = luaL_optint(L, 2, 0); \
	uint32_t x = si->core->rawRead##width (si->core, address, segment); \
	lua_pushinteger(L, x); \
	return 1; \
}

mgba_readX(32)
mgba_readX(16)
mgba_readX(8)

#define mgba_writeX(width) static int mgba_write##width (lua_State* L) { \
	struct mScriptingInterface* si = getScriptingInterface(L); \
	uint32_t address = luaL_checkint(L, 1); \
	uint32_t value = luaL_checkint(L, 2); \
	int segment = luaL_optint(L, 3, 0); \
	si->core->rawWrite##width (si->core, address, value, segment); \
	return 0; \
}

mgba_writeX(32)
mgba_writeX(16)
mgba_writeX(8)

// TODO: bus variants

static const luaL_Reg lib[] = {
	{"getGameCode", mgba_getGameCode},
	{"getGameTitle", mgba_getGameTitle},

	{"read32", mgba_read32},
	{"read16", mgba_read16},
	{"read8", mgba_read8},

	{"write32", mgba_write32},
	{"write16", mgba_write16},
	{"write8", mgba_write8},
	{NULL, NULL},
};

void mLuaInterfaceInit(struct mScriptingInterface *si) {
	si->interface = LUA_SCRIPTING_INTERFACE_ID;
	si->loadScript = luaLoadScript;
	si->onReset = luaOnReset;
	si->onHalt = luaOnHalt;
	si->onFrameEnd = luaOnFrameEnd;


	lua_State *L = luaL_newstate();
	si->state = L;
	luaL_openlibs(L);


	//put a pointer to the scripting interface and the gba into LUA_REGISTRYINDEX
	lua_pushstring(L, "mgba"); // "mgba"
	lua_newtable(L); // {} "mgba"

	lua_pushstring(L, "si"); // "si" {} "mgba"
	lua_pushlightuserdata(L, si); // *si "si" {} "mgba"
	lua_settable(L, -3); // {si=*si} "mgba"

	lua_settable(L, LUA_REGISTRYINDEX); // [STACK EMPTY :)]


	//create the global api for mgba
	luaL_register(L, "mgba", lib);
	lua_setglobal(L, "mgba");
}
