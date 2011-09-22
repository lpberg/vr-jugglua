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
#ifndef INCLUDED_LuaUserdata_h_GUID_52d4ca22_cd5a_420e_b6fd_f46db9eb1f93
#define INCLUDED_LuaUserdata_h_GUID_52d4ca22_cd5a_420e_b6fd_f46db9eb1f93

// Internal Includes
#include "LuaIncludeFull.h"

// Library/third-party includes
// - none

// Standard includes
#include <typeinfo>
#include <new>
#include <cassert>
#include <cstring>

/// @todo this is bad
#define LUA_INDEX_METAMETHOD_NAME "__index"

#ifdef ENABLE_LUA_USERDATA_VERBOSE
#	include <iostream>
#	define LUA_USERDATA_VERBOSE(X) {std::cerr << "LuaUserdata.h:" << __LINE__ << ":\t" << X << std::endl;}
#else
#	define LUA_USERDATA_VERBOSE(X) {}
#endif
namespace osgLua {
	class LuaUserdataBase {
		public:
			//static const char LUA_INDEX_METAMETHOD_NAME[] = "__index";
	};
	template<typename Derived>
	class LuaUserdata : public LuaUserdataBase {
		public:
			typedef int (Derived::*NonConstInstanceMethodPtrType)(lua_State *);
			typedef int (Derived::*ConstInstanceMethodPtrType)(lua_State *) const;
			typedef Derived * PointerToDerivedType;
			typedef Derived ** DerivedPtrPtr;
			typedef Derived DerivedType;

		private:

			static const char * _getRegistryString() {
				return typeid(Derived).name();
			}

			static void _pushMetatable(lua_State * L) {
				luaL_getmetatable(L, _getRegistryString());
			}

			static void _pushIndexMetatable(lua_State * L) {
				_pushMetatable(L);
				int metatableIndex = lua_gettop(L);
				lua_getfield(L, metatableIndex, LUA_INDEX_METAMETHOD_NAME);
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1);
					lua_newtable(L);
					lua_pushvalue(L, -1); /// now the top two values are the __index table
					lua_setfield(L, metatableIndex, LUA_INDEX_METAMETHOD_NAME);
				} else if (!lua_istable(L, -1)) {
					luaL_error(L, "While registering an instance method of %s: __index metamethod already set to value of type %s!", _getRegistryString(), lua_typename(L, -1));
					return;
				}
				/// OK, past this point we have the __index table at -1.

				/// Remove the metatable from the stack so that all we have pushed is the __index table
				lua_remove(L, -2);
			}
			static int _gc(lua_State * L) {
				void * instancePtr = luaL_checkudata(L, 1, _getRegistryString());
				assert(instancePtr);

				PointerToDerivedType instance;
				std::memcpy(&instance, instancePtr, sizeof(PointerToDerivedType));
				LUA_USERDATA_VERBOSE("Userdata " << instance << ":\t" << "Deleting instance of type " << _getRegistryString());
				assert(instance);
				/// explicitly delete before Lua deletes the pointer.
				delete instance;
				instance = NULL;
				return 0;
			}
		protected:
			template<typename PtrToMemberFuncType>
			class InstanceMethodHandler {
				private:
					template<PtrToMemberFuncType M>
					static void * _getMethodDescription() {
						return reinterpret_cast<void *>(&_callInstanceMethod<M>);
					}

					template<PtrToMemberFuncType M>
					static int _callInstanceMethod(lua_State * L) {
						void * instancePtr = luaL_checkudata(L, 1, _getRegistryString());
						if (!instancePtr) {
							return luaL_error(L, "Trying to call an instance method of %s, but first argument is not an instance!", _getRegistryString());
						}
						PointerToDerivedType instance;
						std::memcpy(&instance, instancePtr, sizeof(PointerToDerivedType));

						LUA_USERDATA_VERBOSE("Userdata " << instance << ":\tMethod " << _getMethodDescription<M>() << "\t" << "Before instance method call with lua_gettop(L)==" << lua_gettop(L));
						//lua_remove(L, 1); /// remove the instance from the stack, since it's been handled.
						int ret = ((*instance).*(M))(L);
						LUA_USERDATA_VERBOSE("Userdata " << instance << ":\tMethod " << _getMethodDescription<M>() << "\t" << "After instance method call returning " << ret);
						return ret;
					}



				public:
					template<PtrToMemberFuncType M>
					static void pushInstanceMethod(lua_State * L) {
						lua_pushcfunction(L, &_callInstanceMethod<M>);
						LUA_USERDATA_VERBOSE("Userdata type " << _getRegistryString() << ":\tMethod " << _getMethodDescription<M>() << "\t" << "Pushing an instance method");
					}

					template<PtrToMemberFuncType M>
					static void registerMetamethod(lua_State * L, const char * metamethodName) {
						LUA_USERDATA_VERBOSE("Userdata type " << _getRegistryString << ":\tMethod " << _getMethodDescription<M>() << "\t" << "Registering metamethod " << metamethodName);
						_pushMetatable(L);
						pushInstanceMethod<M>(L);
						lua_setfield(L, -2, metamethodName); /// table is one below the top of the stack
						lua_pop(L, 1); /// pop the metatable off the stack.
					}

					template<PtrToMemberFuncType M>
					static void registerObjectMethod(lua_State * L, const char * methodName) {
						LUA_USERDATA_VERBOSE("Userdata type " << _getRegistryString << ":\tMethod " << _getMethodDescription<M>() << "\t" << "Registering object method " << methodName);
						_pushIndexMetatable(L);
						pushInstanceMethod<M>(L);
						lua_setfield(L, -2, methodName); /// table is one below the top of the stack
						lua_pop(L, 1); /// pop the __index table off the stack.
					}
			};

			typedef InstanceMethodHandler<NonConstInstanceMethodPtrType> NonConstInstanceMethod;
			typedef InstanceMethodHandler<ConstInstanceMethodPtrType> ConstInstanceMethod;

			static const size_t ALIGNMENT = sizeof(void *);

			static PointerToDerivedType allocate(lua_State * L, PointerToDerivedType instance) {
				LUA_USERDATA_VERBOSE("Userdata " << instance << ":\t" << "Created instance of type " << _getRegistryString());
				void * ud = lua_newuserdata(L, sizeof(PointerToDerivedType));
				if (!ud) {
					throw std::bad_alloc();
				}
				_pushMetatable(L);
				lua_setmetatable(L, -2);
				std::memcpy(ud, &instance, sizeof(PointerToDerivedType));
				return instance;
			}
			static PointerToDerivedType pushNewWithLuaParam(lua_State * L) {
				PointerToDerivedType p;
				try {
					p = new DerivedType(L);
				} catch (...) {
					return NULL;
				}
				allocate(L, p);
				return p;
			}

			static PointerToDerivedType pushNew(lua_State * L) {
				PointerToDerivedType p;
				try {
					p = new DerivedType();
				} catch (...) {
					return NULL;
				}
				allocate(L, p);
				return p;
			}

			template<typename T1>
			static PointerToDerivedType pushNew(lua_State * L, T1 a1) {
				PointerToDerivedType p;
				try {
					p = new DerivedType(a1);
				} catch (...) {
					return NULL;
				}
				allocate(L, p);
				return p;
			}

			template<typename T1, typename T2>
			static PointerToDerivedType pushNew(lua_State * L, T1 a1, T2 a2) {
				PointerToDerivedType p;
				try {
					p = new DerivedType(a1, a2);
				} catch (...) {
					return NULL;
				}
				allocate(L, p);
				return p;
			}

			template<typename T1, typename T2, typename T3>
			static PointerToDerivedType pushNew(lua_State * L, T1 a1, T2 a2, T3 a3) {
				PointerToDerivedType p;
				try {
					p = new DerivedType(a1, a2, a3);
				} catch (...) {
					return NULL;
				}
				allocate(L, p);
				return p;
			}

			static void pushCreatorFunction(lua_State * L) {
				lua_pushcfunction(L, pushNewWithLuaParam);
			}

			/// Implement your own method to override this if you want to
			/// register more than just __gc. You can call registerNonConstInstanceMetamethod()
			/// and registerConstInstanceMetamethod() from within your method.
			static void registerAdditionalMetamethods(lua_State *) {}

		public:
			static void createMetatable(lua_State * L) {
				if (luaL_newmetatable(L, _getRegistryString())) {
					{
						LUA_USERDATA_VERBOSE("Userdata type " << _getRegistryString << ":\t" << "Registering garbage collection metamethod");
						_pushMetatable(L);
						lua_pushcfunction(L, &_gc);
						lua_setfield(L, -2, "__gc"); /// table is one below the top of the stack
						lua_pop(L, 1); /// pop the metatable off the stack.
					}
					//NonConstInstanceMethod::template registerMetamethod <&Derived::~Derived> (L, );
					Derived::registerAdditionalMetamethods(L);
				}
			}


	};


}

#endif // INCLUDED_LuaUserdata_h_GUID_52d4ca22_cd5a_420e_b6fd_f46db9eb1f93
