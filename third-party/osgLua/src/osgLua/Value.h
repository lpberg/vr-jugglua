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

#ifndef OSGLUA_VALUE
#define OSGLUA_VALUE

#include <osgIntrospection/Value>
#include <osgIntrospection/Type>
#include <osg/Referenced>
#include <osg/ref_ptr>

#include <vrjugglua/LuaIncludeFull.h>

namespace osgLua {

	class Value
	{
	public:
		osgIntrospection::Value &get();
		const osgIntrospection::Type &getRealType() const;
		const osgIntrospection::Type &getType() const;
		void set( const osgIntrospection::Value &v );

		static void push(lua_State *L,const osgIntrospection::Value &v);
		static Value* get(lua_State *L, int index);

		static int getTypeInfo(lua_State *L);
		static int getTypes(lua_State *L);
		static int createByName(lua_State *L);

	private:
		Value();
		Value( const osgIntrospection::Value &v );
		~Value();
		Value(const Value &);
		Value& operator=(const Value &);
		
		osgIntrospection::Value _value;
		osg::ref_ptr<osg::Referenced> _referenced;

		static Value* rawGet(lua_State *L, int index);
		static int gc(lua_State *L);
		static int index(lua_State *L);

		static int methodCall(lua_State *L);
	};

	osgIntrospection::Value getValue(lua_State *L, int index);


	/****************************************************************/
	/* --- Inline implementations --- *******************************/
	/****************************************************************/
	inline osgIntrospection::Value &Value::get()
	{
		return _value;
	}

	inline const osgIntrospection::Type &Value::getRealType() const
	{
		return _value.getType();
	}

	inline const osgIntrospection::Type &Value::getType() const
	{
		if (_value.isTypedPointer())
		{
			return _value.getInstanceType();
		}
		return _value.getType();
	}
	
	inline void Value::set( const osgIntrospection::Value &v )
	{
		_value = v;
	}

} // end of osgLua namespace

#endif
