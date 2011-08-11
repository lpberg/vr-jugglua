/**	@file
	@brief	header

	@date
	2009-2011

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2009-2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDED_vrjugglua_LuaInclude_h
#define INCLUDED_vrjugglua_LuaInclude_h

#include <vrjugglua/VRJLuaConfig.h>

#ifdef __cplusplus
#ifndef BUILD_LUA_AS_CPP
#define LUA_C_INTERFACE_BEGIN extern "C" {
#define LUA_C_INTERFACE_END }
#endif
#endif

#ifndef LUA_C_INTERFACE_BEGIN
#define LUA_C_INTERFACE_BEGIN
#define LUA_C_INTERFACE_END
#endif

LUA_C_INTERFACE_BEGIN

#include <lua.h>

LUA_C_INTERFACE_END


#endif // INCLUDED_vrjugglua_LuaInclude_h
