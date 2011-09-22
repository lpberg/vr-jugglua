/**
	@file
	@brief Implementation

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

// Internal Includes
#include "IndexedPropertyProxy.h"
#include "LuaIncludeFull.h"
#include <osgLua/Value>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osgLua {
	static const char INDEXED_PROPERTY_PROXY_REGISTRY_NAME[] = "osgLua.IndexedPropertyProxy";
	/*
		void IndexedPropertyProxy::addToRegistry(lua_State * L) {
			luaL_newmetatable(L, INDEXED_PROPERTY_PROXY_REGISTRY_NAME);
		}
		IndexedPropertyProxy * IndexedPropertyProxy::get(lua_State * L, int index) {
			void * ud = luaL_checkudata(L, index, INDEXED_PROPERTY_PROXY_REGISTRY_NAME);
			luaL_argcheck(L, ud != NULL, index, "osgLua IndexedPropertyProxy expected");
			return static_cast<IndexedPropertyProxy *>(ud);
		}

		IndexedPropertyProxy * IndexedPropertyProxy::pushNew(lua_State * L, introspection::Value & instance, const introspection::PropertyInfo * property) {
			IndexedPropertyProxy * p;
			void * ud = lua_newuserdata(L, sizeof(IndexedPropertyProxy));
			if (!ud) {
				throw std::bad_alloc();
			}
			try {
				p = new(ud) IndexedPropertyProxy(instance, property);
				return p;
			} catch (...) {
				lua_pop(L);
				return NULL;
			}
		}
	*/
	IndexedPropertyProxy::IndexedPropertyProxy(introspection::Value  const& instance, const introspection::PropertyInfo * property)
		: _instance(instance)
		, _propInfo(property)
	{}

	int IndexedPropertyProxy::index(lua_State *L) {
		int argType = lua_type(L, 2);
		if (argType == LUA_TSTRING) {
			int top = lua_gettop(L);
			lua_pushliteral(L, "insert");
			if (lua_equal(L, 2, -1)) {
				Base::NonConstInstanceMethod::pushInstanceMethod<&IndexedPropertyProxy::insert>(L);
				return 1;
			}
			lua_settop(L, top);
		} else if (argType == LUA_TNUMBER) {
			/// indexed property element access
			int i = lua_tointeger(L, 2) - 1;
			if (i < _propInfo->getNumArrayItems(_instance)) {
				std::cerr << "In " << __FUNCTION__ << " getting item " << i << std::endl << std::flush;
				Value::push(L, _propInfo->getArrayItem(_instance, i));
				return 1;
			}
		}

		//lua_pushnil(L);
		//return 1;
		return luaL_error(L, "Indexed property %s expected access of element by index, starting at 1, or a method call to 'insert'", _propInfo->getName().c_str());
	}

	int IndexedPropertyProxy::newindex(lua_State *L) {
		int eltIndex = luaL_checkint(L, 2) - 1;
		_propInfo->setArrayItem(_instance, eltIndex, Value::getRequired(L, 3)->get());
		return 0;
	}

	int IndexedPropertyProxy::len(lua_State *L) {
		assert(_propInfo);
		lua_pushinteger(L, _propInfo->getNumArrayItems(_instance));
		return 1;
	}

	int IndexedPropertyProxy::insert(lua_State *L) {
		int numItems = _propInfo->getNumArrayItems(_instance);
		int location = -1;
		bool atEnd = true;
		introspection::Value newval;
		switch (lua_gettop(L)) {
			case 2:
				newval = Value::getRequired(L, 2)->get();
				break;
			case 3:
				location = luaL_checkint(L, 2) - 1;
				newval = Value::getRequired(L, 3)->get();
				atEnd = (location == numItems);
				break;
			default:
				return luaL_error(L, "Indexed property %s call to 'insert' expected at least a value, and potentially a location and a value", _propInfo->getName().c_str());
		}

		if (atEnd) {
			_propInfo->addArrayItem(_instance, newval);
		} else {
			_propInfo->insertArrayItem(_instance, location, newval);
		}

		return 0;
	}



	void IndexedPropertyProxy::registerAdditionalMetamethods(lua_State *L) {
		Base::NonConstInstanceMethod::registerMetamethod<&IndexedPropertyProxy::index>(L, "__index");
		Base::NonConstInstanceMethod::registerMetamethod<&IndexedPropertyProxy::newindex>(L, "__newindex");
		Base::NonConstInstanceMethod::registerMetamethod<&IndexedPropertyProxy::len>(L, "__len");
	}
}
