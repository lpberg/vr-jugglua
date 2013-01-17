/**	@file
	@brief	implementation of Sonix binding

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


// Local includes
#include "BindSonixToLua.h"
#include "../VRJLuaOutput.h"

// Library/third-party includes
#include <luabind/luabind.hpp>
#include <osg/Vec4>
#include <snx/SoundHandle.h>

// Standard includes
#ifdef VERBOSE
#include <iostream>
#endif

namespace{
	 osg::Vec3f scalingGetPosition(snx::SoundHandle &h){
		const double feetToMeters = 0.3048;
		float x, y, z;
		snx::SoundHandle &hbetter = const_cast<snx::SoundHandle&>(h);
		hbetter.getPosition(x,y,z);
		return  osg::Vec3f(x,y,z)*feetToMeters;
	}
	void scalingSetPosition(snx::SoundHandle &h, osg::Vec3 &vec){
		const double metersToFeet = 3.28084;
		osg::Vec3f scaledVec = vec*metersToFeet;
		snx::SoundHandle &hbetter = const_cast<snx::SoundHandle&>(h);
		hbetter.setPosition(scaledVec.x(),scaledVec.y(),scaledVec.z());
		return;
	}
}
namespace vrjLua {
	using namespace luabind;

	namespace Sonix {
		void changeAPI(const std::string & api) {
			snx::sonix::instance()->changeAPI(api);
		}
	} // end of Internal namespace

	void bindSonixToLua(LuaStatePtr state) {
#ifdef VERBOSE
		std::cerr << "Registering snx module functions and objects with Lua..." << std::flush << std::endl;
#endif
		module(state.get(), "snx") [
		    def("changeAPI", &Sonix::changeAPI),

		    class_<snx::SoundInfo>("SoundInfo")
		    .def(constructor<>())
		    .def_readwrite("alias", &snx::SoundInfo::alias)
		    .def_readwrite("filename", &snx::SoundInfo::filename)
		    .def_readwrite("ambient", &snx::SoundInfo::ambient)
		    .def_readwrite("repeat", &snx::SoundInfo::repeat)
		    .def_readwrite("volume", &snx::SoundInfo::volume)
		    .def_readwrite("streaming", &snx::SoundInfo::streaming)
		    ,

		    class_<snx::SoundHandle>("SoundHandle")
		    .def(constructor<const std::string &>())
		    .def("configure", &snx::SoundHandle::configure)
		    .def("remove", &snx::SoundHandle::remove)
		    .property("name", &snx::SoundHandle::getName)
		    .def("trigger", &snx::SoundHandle::trigger)
		    .property("isPlaying", &snx::SoundHandle::isPlaying)
		    .def("stop", &snx::SoundHandle::stop)
		    .def("pause", &snx::SoundHandle::pause)
		    .def("unpause", &snx::SoundHandle::unpause)
		    .property("isPaused", &snx::SoundHandle::isPaused)
		    .property("ambient", &snx::SoundHandle::isAmbient, &snx::SoundHandle::setAmbient)
		    .property("retriggerable", &snx::SoundHandle::isRetriggerable, &snx::SoundHandle::setRetriggerable)
		    //.def("setPosition", &scalingSetPosition)
			.def("getPosition", &scalingGetPosition)
			.def("setPitchBend", &snx::SoundHandle::setPitchBend)
			.def("setVolume", &snx::SoundHandle::setVolume)
			.def("setCutoff", &snx::SoundHandle::setCutoff)
		];
	}
}// end of vrjLua namespace
