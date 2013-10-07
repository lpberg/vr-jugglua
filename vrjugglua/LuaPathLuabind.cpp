/**
	@file
	@brief Implementation

	@date 2013

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Internal Includes
#include "LuaPath.h"
#include "LuaPathUpdater.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace vrjLua {
	void LuaPath::addLuaRequirePath(LuaStatePtr state, std::string const& dirEndingInSlash) {
		LuaSearchPathUpdater searchpath(state.get());

		searchpath.extend(SearchDirectory(dirEndingInSlash));
	}
} // end of namespace vrjLua
