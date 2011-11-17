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

#include <snx/SoundHandle.h>

// Standard includes
#ifdef VERBOSE
#include <iostream>
#endif

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
			//.def_readwrite("position", &snx::SoundInfo::position)
		    .def_readwrite("ambient", &snx::SoundInfo::ambient)
			.def_readwrite("retriggerable",&snx::SoundInfo::retriggerable)
		    .def_readwrite("repeat", &snx::SoundInfo::repeat)
			.def_readwrite("pitchbend",&snx::SoundInfo::pitchbend)
			.def_readwrite("cutoff",&snx::SoundInfo::cutoff)
		    .def_readwrite("volume", &snx::SoundInfo::volume)
		    .def_readwrite("streaming", &snx::SoundInfo::streaming)
		    ,

		    class_<snx::SoundHandle>("SoundHandle")
		    .def(constructor<const std::string &>())
			.def("getName", &snx::SoundHandle::getName)
			.def("trigger", &snx::SoundHandle::trigger)
			.def("isPlaying", &snx::SoundHandle::isPlaying)
		    .def("pause", &snx::SoundHandle::pause)
		    .def("configure", &snx::SoundHandle::configure)
		    .def("remove", &snx::SoundHandle::remove)
		    .property("name", &snx::SoundHandle::getName)
		    .property("isPlaying", &snx::SoundHandle::isPlaying)
		    .def("stop", &snx::SoundHandle::stop)
		    .def("unpause", &snx::SoundHandle::unpause)
		    .property("isPaused", &snx::SoundHandle::isPaused)
		    .property("ambient", &snx::SoundHandle::isAmbient, &snx::SoundHandle::setAmbient)
			.def("setPitchBlend", &snx::SoundHandle::setPitchBend)
			.def("setVolume", &snx::SoundHandle::setVolume)
			.def("setCutoff", &snx::SoundHandle::setCutoff)
		    .property("retriggerable", &snx::SoundHandle::isRetriggerable, &snx::SoundHandle::setRetriggerable)
		    .def("setPosition", &snx::SoundHandle::setPosition)
			.def("getPositionByRef", &snx::SoundHandle::getPosition)
			//.def("setListenerPosition", &snx::SoundHandle::setListenerPosition)
			//.def("getListenerPosition", &snx::SoundHandle::getListenerPosition)

		];

	}

}// end of vrjLua namespace
