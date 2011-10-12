/** @file
	@brief Header

	@date 2011

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDED_IndexedPropertyProxy_h_GUID_188a9d92_bb03_4f24_897e_f6540f8688c9
#define INCLUDED_IndexedPropertyProxy_h_GUID_188a9d92_bb03_4f24_897e_f6540f8688c9

#define LUACPP_LUA_INCLUDE_FULL "LuaIncludeFull.h"

// Internal Includes
#include "LuaUserdata.h"

// Library/third-party includes
#include <osgLua/LuaInclude>
#include <osgLua/introspection/PropertyInfo>
#include <osgLua/introspection/Value>

// Standard includes
#include <string>

namespace osgLua {
	class Value;
	class IndexedPropertyProxy : public ::luacpputils::LuaUserdata<IndexedPropertyProxy> {
		public:
			typedef LuaUserdata<IndexedPropertyProxy> Base;

			friend class LuaUserdata<IndexedPropertyProxy>;

			static void pushNew(lua_State * L, introspection::Value const& instance, const introspection::PropertyInfo * property) {
				Base::pushNew(L, instance, property);
			}

			int index(lua_State *L);
			int newindex(lua_State *L);
			int len(lua_State *L);
			int insert(lua_State *L);
			int remove(lua_State *L);
		private:
			/// Look up item i, if in range, push and return true, else return false
			static const char * _getMethodRegistryString();
			bool _pushItemAtArrayIndex(lua_State *L, int i);
			IndexedPropertyProxy(introspection::Value const& instance, const introspection::PropertyInfo * property);
			static void registerAdditionalMetamethods(lua_State *L);

			introspection::Value _instance;
			const introspection::PropertyInfo * _propInfo;
			std::string _propName;
	};

} // end of namespace osgLua

#endif // INCLUDED_IndexedPropertyProxy_h_GUID_188a9d92_bb03_4f24_897e_f6540f8688c9
