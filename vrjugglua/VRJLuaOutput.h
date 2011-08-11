/**	@file
	@brief	Output stream wrappers (extending vpr::Debug)

	@date
	2010

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2010-2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDED_vrjugglua_vrjLuaOutput_h
#define INCLUDED_vrjugglua_vrjLuaOutput_h

// Local includes
// - none

// Library includes
#include <vpr/Util/Debug.h>

// Standard includes
// - none

/// @brief Standard "status" message importance
#define MSG_STATUS vprDBG_CONFIG_STATUS_LVL

/// @brief Warning message importance
#define MSG_WARNING vprDBG_WARNING_LVL

/// @brief Error message importance
#define MSG_ERROR vprDBG_CRITICAL_LVL

/// @brief Very verbose message importance - not shown by default
#define MSG_DRIVEL vprDBG_HVERB_LVL

#define VRJLUA_MSG_START(msgsrc, msglevel) vprDEBUG(msgsrc, msglevel)
#define VRJLUA_MSG_END(msgsrc, msglevel) std::endl << vprDEBUG_FLUSH

const vpr::DebugCategory dbgVRJLUA(vpr::GUID("dd146240-69b2-11df-96d4-0002a5d5c51b"), "DBG_VRJLUA", "VRJLUA:");
const vpr::DebugCategory dbgVRJLUA_PROXY(vpr::GUID("dd146240-69b2-11df-96d4-0002a5d5c51b"), "DBG_LUAPROXY", "LUAPROXY:");
const vpr::DebugCategory dbgVRJLUA_APP(vpr::GUID("c153dce0-69b5-11df-90ff-0002a5d5c51b"), "DBG_LUAAPP", "LUAAPP:");
const vpr::DebugCategory dbgVRJLUA_BUFFER(vpr::GUID("1a4dd46d-1b7e-4a53-9cdd-94c3dbf84b6e"), "DBG_LUABUFFER", "LUABUFFER:");
const vpr::DebugCategory dbgVRJLUA_CONSOLE(vpr::GUID("415e7b22-d809-45f7-bfc2-3c0fc426aba2"), "DBG_LUACONSOLE", "LUACONSOLE:");


#endif // INCLUDED_vrjugglua_vrjLuaOutput_h
