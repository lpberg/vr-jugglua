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

#include "lua_functions.h"
#include "LuaIncludeFull.h"


namespace osgLua {

	const char *getName(const introspection::Type &type) {
		if (type.isDefined()) {
			return type.getName().c_str();
		}
		return "(not defined)";
	}

	void pushParameter(lua_State *L, const introspection::ParameterInfo *p) {
		const char *in = "IN";
		const char *out = "OUT";
		const char *inout = "IN/OUT";

		const char *type = in;
		if (p->isInOut()) {
			type = inout;
		} else if (p->isOut()) {
			type = out;
		}

		lua_pushfstring(L, "[%s] %s %s", type,
		                getName(p->getParameterType()),
		                p->getName().c_str()
		               );
	}

	void pushMethodInfo(lua_State *L, const introspection::MethodInfo *mi) {
		int top = lua_gettop(L);
		lua_pushfstring(L, "%s\t%s(",
		                getName(mi->getReturnType()),
		                mi->getName().c_str());
		const introspection::ParameterInfoList &param = mi->getParameters();
		for (introspection::ParameterInfoList::const_iterator
		        i = param.begin(); i != param.end(); ++i) {
			pushParameter(L, *i);
			lua_pushstring(L, ", ");
		}
		if (!param.empty()) {
			lua_pop(L, 1);
		}
		lua_pushstring(L, ")");
		if (mi->isConst()) {
			lua_pushstring(L, " const");
		}
		if (mi->isStatic()) {
			lua_pushstring(L, " static");
		}
		lua_concat(L, lua_gettop(L) - top);
	}

	void pushArrayPropertyInfo(lua_State *L, const introspection::PropertyInfo *pi) {
		lua_pushstring(L, getName(pi->getPropertyType()));
		lua_pushstring(L, "\t");

		lua_pushstring(L, pi->getName().c_str());

		lua_pushstring(L, "[i]\t(array - access through 1-based index, the # operator for size, or method calls :insert)");
	}

	void pushIndexedPropertyInfo(lua_State *L, const introspection::PropertyInfo *pi) {

		lua_pushstring(L, getName(pi->getPropertyType()));
		lua_pushstring(L, "\t");
		lua_pushstring(L, pi->getName().c_str());

		lua_pushstring(L, "(");
		const introspection::ParameterInfoList & param = pi->getIndexParameters();
		for (introspection::ParameterInfoList::const_iterator
		        i = param.begin(); i != param.end(); ++i) {
			pushParameter(L, *i);
			lua_pushstring(L, ",");
		}
		if (!param.empty()) {
			lua_pop(L, 1);
		}
		lua_pushstring(L, ")\t(indexed)");
	}

	void pushSimplePropertyInfo(lua_State *L, const introspection::PropertyInfo *pi) {


		lua_pushstring(L, getName(pi->getPropertyType()));
		lua_pushstring(L, "\t");
		lua_pushstring(L, pi->getName().c_str());
		if (pi->canGet() && pi->canSet()) {
			lua_pushstring(L, "\t(read/write)");
		} else if (pi->canGet() && !pi->canSet()) {
			lua_pushstring(L, "\t(read-only)");
		} else if (!pi->canGet() && pi->canSet()) {
			lua_pushstring(L, "\t(write-only)");
		} else {
			// should never happen
			lua_pushstring(L, "\t(can't read or write!)");
		}
	}

	void pushPropertyInfo(lua_State *L, const introspection::PropertyInfo *pi) {

		int top = lua_gettop(L);

		if (pi->isArray()) {
			lua_pushstring(L, " ");
			pushArrayPropertyInfo(L, pi);
		} else if (pi->isIndexed()) {
			/// @todo indexed properties not yet supported - no sense in showing them
			lua_pushstring(L, " ");
			//pushIndexedPropertyInfo(L, pi);
		} else if (pi->isSimple()) {
			pushSimplePropertyInfo(L, pi);
		} else {
			// should not happen
			luaL_error(L, "%s:%d Property is neither array, indexed, nor simple!",
			           __FILE__, __LINE__);
		}

		lua_concat(L, lua_gettop(L) - top);
	}

	void pushConstructorInfo(lua_State *L,
	                         const introspection::ConstructorInfo *ci) {
		int top = lua_gettop(L);
		lua_pushfstring(L, "%s (",
		                getName(ci->getDeclaringType()));
		const introspection::ParameterInfoList &param = ci->getParameters();
		for (introspection::ParameterInfoList::const_iterator
		        i = param.begin(); i != param.end(); ++i) {
			pushParameter(L, *i);
			lua_pushstring(L, ",");
		}
		if (!param.empty()) {
			lua_pop(L, 1);
		}
		lua_pushstring(L, ")");
		lua_concat(L, lua_gettop(L) - top);
	}

	void pushTypeInfo(lua_State *L, const introspection::Type *type) {
		if (type) {
			lua_newtable(L);
			int table = lua_gettop(L);

			{
				// Stdname
				lua_pushstring(L,  type->getStdTypeInfo().name());
				lua_setfield(L, table, "stdName");
			}
			{
				// Type_pointer
				lua_pushinteger(L, (lua_Integer)(void*) type);
				lua_setfield(L, table, "typePointer");
			}

			if (!type->isDefined()) {
				return;
			}

			{
				// name
				lua_pushstring(L,  type->getQualifiedName().c_str());
				lua_setfield(L, table, "name");
			}
			{
				// methods
				lua_newtable(L);
				int count = 1;
				introspection::MethodInfoList list;
				try {
					type->getAllMethods(list);
					for (introspection::MethodInfoList::const_iterator
					        i = list.begin(); i != list.end(); ++i, ++count) {
						pushMethodInfo(L, *i);
						lua_rawseti(L, -2, count);
					}
				} catch (introspection::Exception & e) {
					luaL_error(L, "Caught exception %s in type named %s", e.what().c_str(), type->getQualifiedName().c_str());
					return;
				}
				lua_setfield(L, table, "methods");
			}
			{
				// properties
				lua_newtable(L);
				int count = 1;
				introspection::PropertyInfoList list;
				type->getAllProperties(list);
				for (introspection::PropertyInfoList::const_iterator
				        i = list.begin(); i != list.end(); ++i, ++count) {
					pushPropertyInfo(L, *i);
					lua_rawseti(L, -2, count);
				}
				lua_setfield(L, table, "properties");
			}
			{
				// constructors
				lua_newtable(L);
				int count = 1;
				const introspection::ConstructorInfoList &list =
				    type->getConstructors();
				for (introspection::ConstructorInfoList::const_iterator
				        i = list.begin(); i != list.end(); ++i, ++count) {
					pushConstructorInfo(L, *i);
					lua_rawseti(L, -2, count);
				}
				lua_setfield(L, table, "constructors");
			}
			/// @todo add properties here
		} else {
			lua_pushnil(L);
		}
	}

} // end of osgLua namespace
