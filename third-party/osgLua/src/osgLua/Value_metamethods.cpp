/*
	osgLua: use Lua to access dynamically to osg using osgIntrospection
	Copyright(C) 2006 Jose L. Hidalgo Valiño (PpluX) (pplux at pplux.com)

    This library is open source and may be redistributed and/or modified under
    the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
    (at your option) any later version.  The full license is in LICENSE file
    included with this distribution, and on the openscenegraph.org website.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    OpenSceneGraph Public License for more details.
*/

#include "Value_metamethods.h"
#include "Value.h"
#include "Type.h"
#include "lua_functions.h"

#include <osgIntrospection/Reflection>
#include <osgIntrospection/MethodInfo>
#include <osgIntrospection/ConstructorInfo>
#include <osgIntrospection/Utility>
#include <osgIntrospection/ExtendedTypeInfo>
#include <osgIntrospection/variant_cast>

#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec3f>
#include <osg/Vec4>
#include <osg/Vec4d>
#include <osg/Vec4f>

namespace osgLua {

	int metamethods::tostring(lua_State *L) {
		Value *a = Value::getRequired(L,1);

		lua_pushstring(L, a->get().toString().c_str());
		return 1;
	}

	int metamethods::eq(lua_State *L) {
		Value *a = Value::getRequired(L,1);
		Value *b = Value::getRequired(L,2);

		bool ret;
		try {
			ret = a->get() == b->get();
		} catch(osgIntrospection::Exception &e) {
			luaL_error(L,"[%s:%d] %s",__FILE__,__LINE__,e.what().c_str());
		}

		lua_pushboolean(L, ret);

		return 1;
	}

	int metamethods::lt(lua_State *L) {
		Value *a = Value::getRequired(L,1);

		Value *b = Value::getRequired(L,2);
		bool ret;
		try {
			ret = a->get() < b->get();
		} catch(osgIntrospection::Exception &e) {
			luaL_error(L,"[%s:%d] %s",__FILE__,__LINE__,e.what().c_str());
		}

		lua_pushboolean(L, ret);

		return 1;
	}

	int metamethods::le(lua_State *L) {
		Value *a = Value::getRequired(L,1);

		Value *b = Value::getRequired(L,2);
		bool ret;
		try {
			ret = a->get() <= b->get();
		} catch(osgIntrospection::Exception &e) {
			luaL_error(L,"[%s:%d] %s",__FILE__,__LINE__,e.what().c_str());
		}

		lua_pushboolean(L, ret);

		return 1;
	}

} // end of osgLua namespace