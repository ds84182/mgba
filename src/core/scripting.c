/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "scripting.h"

#ifdef USE_LUAJIT
#include "scripting/lua_interface.h"
#endif

mLOG_DEFINE_CATEGORY(SCRIPTING, "Scripting Interface");

const char *mScriptingInterfaces[] = {
#ifdef USE_LUAJIT
	LUA_SCRIPTING_INTERFACE_ID,
#endif
	NULL
};

struct mScriptingInterface *mScriptingInit(struct mCore* core, const char* interfaceType) {
	struct mScriptingInterface *interface = malloc(sizeof(struct mScriptingInterface));
	interface->core = core;
	printf("Getting scripting interface for \"%s\"\n", interfaceType);
#ifdef USE_LUAJIT
	if (strcmp(interfaceType, "lua") == 0) {
		mLuaInterfaceInit(interface);
		return interface;
	}
#endif
	printf("Interface not found!\n");
	free(interface);
	return NULL;
}
