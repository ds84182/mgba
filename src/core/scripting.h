/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef M_SCRIPTING_H
#define M_SCRIPTING_H

#include "core.h"
#include "core/log.h"

mLOG_DECLARE_CATEGORY(SCRIPTING);

// Null terminated list of supported scripting interfaces
extern const char *mScriptingInterfaces[];

// Struct that contains relevant data for a scripting interface
struct mScriptingInterface {
	const char* interface; // The name of the interface
	void* state; // Pointer to interface specific data
	struct mCore* core;

	void (*loadScript)(struct mScriptingInterface* si, const char* script);
	void (*onReset)(struct mScriptingInterface* si);
	void (*onHalt)(struct mScriptingInterface* si);
	void (*onFrameEnd)(struct mScriptingInterface* si);
};

struct mScriptingInterface *mScriptingInit(struct mCore* core, const char* interface);

#endif
