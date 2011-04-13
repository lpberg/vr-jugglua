/*
	osgLua: use Lua to access dynamically to osg using osgIntrospection
	Copyright(C) 2006 Jose L. Hidalgo Valiño (PpluX) (pplux at pplux.com)
	Copyright(C) 2010-2011 Iowa State University (Author: Ryan Pavlik <rpavlik@acm.org> )

    This library is open source and may be redistributed and/or modified under
    the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
    (at your option) any later version.  The full license is in LICENSE file
    included with this distribution, and on the openscenegraph.org website.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    OpenSceneGraph Public License for more details.
*/

#include <osgLua/Value>
#include "Value_metamethods.h"
#include "Value_rawget.inl"
#include "Type.h"
#include "lua_functions.h"

#include "LuaIncludeFull.h"

#include <osgIntrospection/Reflection>
#include <osgIntrospection/MethodInfo>
#include <osgIntrospection/ConstructorInfo>
#include <osgIntrospection/PropertyInfo>
#include <osgIntrospection/Utility>
#include <osgIntrospection/ExtendedTypeInfo>
#include <osgIntrospection/variant_cast>

#include <osg/NodeVisitor>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>

namespace osgLua {
	namespace detail {
		static void push_stdString(lua_State *L, const std::string &s) {
			lua_pushstring(L, s.c_str());
		}

		template<typename T>
		struct LuaBasicTypeTraits;

		template<>
		struct LuaBasicTypeTraits<bool> {
			static void push(lua_State * L, bool val) {
				lua_pushboolean(L, val);
			}
		};

		template<typename T>
		struct LuaIntegerTypeTraits {
			static void push(lua_State * L, T val) {
				lua_pushinteger(L, val);
			}
		};

		template<typename T>
		struct LuaNumberTypeTraits {
			static void push(lua_State * L, T val) {
				lua_pushnumber(L, val);
			}
		};

		template<>
		struct LuaBasicTypeTraits<int> :
			public LuaIntegerTypeTraits<int> {};

		template<>
		struct LuaBasicTypeTraits<unsigned int> :
			public LuaIntegerTypeTraits<unsigned int> {};

		
		/// @todo this is not the best way to do this
#ifndef _WIN32
		// On Windows, ptrdiff_t is just a typedef of integer.
		template<>
		struct LuaBasicTypeTraits<lua_Integer> :
			public LuaIntegerTypeTraits<lua_Integer> {};
#endif
		template<>
		struct LuaBasicTypeTraits<float> :
			public LuaNumberTypeTraits<float> {};

		template<>
		struct LuaBasicTypeTraits<double> :
			public LuaNumberTypeTraits<double> {};
		/// @todo this is not the best way to do this
#ifndef _WIN32
#if (LUA_NUMBER != double) && (LUA_NUMBER != float)
		template<>
		struct LuaBasicTypeTraits<lua_Number> :
			public LuaNumberTypeTraits<lua_Number> {};
#endif
#endif

		template<>
		struct LuaBasicTypeTraits<const char*> {
			static void push(lua_State * L, const char* val) {
				lua_pushstring(L, val);
			}
		};

		template<>
		struct LuaBasicTypeTraits<const std::string> {
			static void push(lua_State * L, const std::string & val) {
				lua_pushstring(L, val.c_str());
			}
		};

		template<typename T>
		bool tryToPushBasicType(lua_State *L, const osgIntrospection::Value &original) {
			static const osgIntrospection::Type& testType =
				osgIntrospection::Reflection::getType(extended_typeid<T>());
			if (original.getType() == testType) {
				LuaBasicTypeTraits<T>::push(L, osgIntrospection::getInstance<T>(original));
				return true;
			}
			return false;
		}

	}

	Value::Value(const osgIntrospection::Value &v) : _value(v) {

		static const osgIntrospection::Type &t_referenced =
		    osgIntrospection::Reflection::getType(extended_typeid<osg::Referenced>());

		const osgIntrospection::Type &type = v.getType();
		if (type.isDefined()) {
			if (type.isNonConstPointer()) {
				const osgIntrospection::Type &p_type = type.getPointedType();
				if (p_type.isDefined())
					if (p_type.isSubclassOf(t_referenced)) {
						_referenced =
						    osgIntrospection::variant_cast<osg::Referenced*>(_value);
					}
			}
		}
	}

	Value::~Value() {
	}

	osgIntrospection::Value getValue(lua_State *L, int index) {
		int top = lua_gettop(L);
		index = (index>0)? index : top + index + 1;

		if (lua_isuserdata(L, index)) {
			Value *v = Value::get(L, index);
			if (v) return v->get();
			else {
				luaL_error(L,
				           "userdata can not be used as osgIntrospection::Value");
			}
		}

		if (lua_isnumber(L, index)) {
			lua_Integer vi = lua_tointeger(L, index);
			lua_Number  vf = lua_tonumber(L, index);

			if (((lua_Number)vi) == vf) {
				return osgIntrospection::Value(vi);
			}
			return osgIntrospection::Value(vf);
		}

		if (lua_isstring(L, index)) {
			return osgIntrospection::Value(
			           std::string(lua_tostring(L, index))
			       );
		}

		if (lua_isboolean(L, index)) {
			return osgIntrospection::Value((lua_toboolean(L, index) == 0) ? false : true);
		}

		luaL_error(L,
		           "Value at %d(%s) can not be used as osgIntrospection::Value",
		           index, lua_typename(L, lua_type(L, index)));

		return osgIntrospection::Value();
	}




	void Value::push(lua_State *L, const osgIntrospection::Value &original) {
		if (detail::tryToPushBasicType<bool>(L, original)) { return; }
		if (detail::tryToPushBasicType<int>(L, original)) { return; }
		if (detail::tryToPushBasicType<unsigned int>(L, original)) { return; }
		if (detail::tryToPushBasicType<lua_Integer>(L, original)) { return; }
		if (detail::tryToPushBasicType<float>(L, original)) { return; }
		if (detail::tryToPushBasicType<double>(L, original)) { return; }
		if (detail::tryToPushBasicType<lua_Number>(L, original)) { return; }
		if (detail::tryToPushBasicType<const char*>(L, original)) { return; }
		if (detail::tryToPushBasicType<const std::string>(L, original)) { return; }

		// if not is a basic type...
		// create a userdata
		Value** data = (Value**) lua_newuserdata(L, sizeof(Value*));

		// create/get the metatable
		_getOrCreateMetatable(L, original.getType());
		lua_setmetatable(L, -2);

		Value *v = new Value(original); //lua's copy
		*data = v;
	}



	Value* Value::get(lua_State *L, int index) {
		int top = lua_gettop(L);
		index = (index>0)? index : top + index + 1;

		if (_hasOsgLuaValueMetatable(L, index)) {
			Value *v = _rawGet(L,index);
			lua_settop(L,top);
			return v;
		}

		lua_settop(L,top);
		return 0;
	}

	Value* Value::getRequired(lua_State *L, int index) {
		Value *a = Value::get(L,index);
		if (a == 0) {
			luaL_error(L, "%s:%d Expected a osgLua userdata but get %s",
			           __FILE__,__LINE__, lua_typename(L,lua_type(L, index))) ;
		}
		return a;
	}


	int Value::getTypeInfo(lua_State *L) {
		const osgIntrospection::Type *type = 0;
		Value *v = Value::get(L,1);
		if (v) type = &(v->getType());
		else if (lua_isstring(L,1)) {
			try {
				const osgIntrospection::Type &t =
				    osgIntrospection::Reflection::getType(lua_tostring(L,1));
				type = &t;
			} catch (osgIntrospection::TypeNotFoundException&) {
				return 0;
			}
		}

		if (type) {
			pushTypeInfo(L, type);
			return 1;
		}

		return 0;
	}

	int Value::getTypes(lua_State *L) {
		const osgIntrospection::TypeMap &map =
		    osgIntrospection::Reflection::getTypes();
		int counter = 1;

		lua_newtable(L);

		for (osgIntrospection::TypeMap::const_iterator i = map.begin();
		        i != map.end(); ++i) {
			osgIntrospection::Type *type = i->second;
			if (!type->isDefined() || type->isPointer()) continue;
			lua_pushstring(L, type->getQualifiedName().c_str());
			lua_rawseti(L,-2,counter);
			counter++;

		}
		return 1;
	}

	int Value::createByName(lua_State *L) {
		int top = lua_gettop(L);
		std::string name(luaL_checkstring(L, 1));
		try {
			const osgIntrospection::Type &type =
			    osgIntrospection::Reflection::getType(name);

			osgIntrospection::ValueList vl;
			for (int i = 2; i <= top; ++i) {
				vl.push_back(getValue(L, i));
			}

			osgIntrospection::Value returnedval = type.createInstance(vl);

			Value::push(L, returnedval);
			return 1;
		} catch (osgIntrospection::Exception &e) {
			luaL_error(L,"[%s:%d] %s",__FILE__,__LINE__,e.what().c_str());
		}
		return 0;
	}

} // end of osgLua namespace
