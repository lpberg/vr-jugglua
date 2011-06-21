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

#include <osgLua/Config>
#ifdef OSGLUA_USE_CPPINTROSPECTION
#	include <cppintrospection/Reflection>
#	include <cppintrospection/MethodInfo>
#	include <cppintrospection/ConstructorInfo>
#	include <cppintrospection/PropertyInfo>
#	include <cppintrospection/Utility>
#else
#	include <osgIntrospection/Reflection>
#	include <osgIntrospection/MethodInfo>
#	include <osgIntrospection/ConstructorInfo>
#	include <osgIntrospection/PropertyInfo>
#	include <osgIntrospection/Utility>
#endif
#include <osgLua/IntrospectionValue>
#include <osgLua/IntrospectionType>

#include <osgLua/LuaInclude>

namespace osgLua {

	void pushParameter(lua_State *L, const osgIntrospection::ParameterInfo *p);
	void pushMethodInfo(lua_State *L, const osgIntrospection::MethodInfo *mi);
	void pushPropertyInfo(lua_State *L, const osgIntrospection::PropertyInfo *mi);
	void pushConstructorInfo(lua_State *L,
	                         const osgIntrospection::ConstructorInfo *ci);
	void pushTypeInfo(lua_State *L, const osgIntrospection::Type *type);


} // end of osgLua namespace
