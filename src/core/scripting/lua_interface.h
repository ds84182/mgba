/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef M_LUA_INTERFACE_H
#define M_LUA_INTERFACE_H

#include "core/scripting.h"

#define LUA_SCRIPTING_INTERFACE_ID "lua"

void mLuaInterfaceInit(struct mScriptingInterface *si);

#endif
