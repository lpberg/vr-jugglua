/**	@file	LuaRunBuffer.cpp
	@brief	implementation

	@date
	2009-2010

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

// Internal Includes
#include "LuaPath.h"
#include "VRJLuaOutput.h"

#include "LuaIncludeFull.h"

// Library includes
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <vpr/System.h>
#include <vrj/vrjParam.h> // for __VJ_version define

// Standard includes
#include <fstream>
#include <sstream>

namespace fs = boost::filesystem;

#ifdef VPR_OS_Linux

#include <link.h>

extern "C" {
	int sharedObjectCallback(struct dl_phdr_info *info,
            size_t size, void *data);
}

int sharedObjectCallback(struct dl_phdr_info *info, size_t size, void *data) {
	std::string fn(info->dlpi_name);
	if (fn.find("vpr") != std::string::npos) {
		(*static_cast<std::string*>(data)) = fn;
	}
	return 0;
}

#endif // VPR_OS_Linux

namespace vrjLua {

std::string LuaPath::_findFilePath(std::vector<std::string> const& startingPlaces, std::string const& qualified) {
	std::string location;
	for (unsigned int i = 0; i < startingPlaces.size() && location.empty(); ++i) {
		try {
			location = _findFilePath(startingPlaces[i], qualified);
		} catch (std::runtime_error & e) {
			VRJLUA_MSG_START(dbgVRJLUA, MSG_DRIVEL)
				<< "LuaPath: " << e.what()
				<< ", was trying with starting path " << startingPlaces[i]
				<< VRJLUA_MSG_END(dbgVRJLUA, MSG_DRIVEL);
			location.clear();
		}
	}
	return location;
}

std::string LuaPath::_findFilePath(std::string const& startingAt, const std::string & fn) {
	// find the config file - recursively searching up.
	fs::path location = startingAt;
	fs::path filepath = fs::path(location / fn);
	VRJLUA_MSG_START(dbgVRJLUA, MSG_DRIVEL)
		<< "LuaPath: Searching for file '" << fn << "' starting in " << location.string()
		<< VRJLUA_MSG_END(dbgVRJLUA, MSG_DRIVEL);

	for (unsigned int i = 0;
#ifdef BOOST_FILESYSTEM_NO_DEPRECATED
			location.has_parent_path() &&
#else
			location.has_branch_path() &&
#endif
			i < 5 && fs::exists(location) && !fs::exists(filepath);
			++i) {

		// go up one
#ifdef BOOST_FILESYSTEM_NO_DEPRECATED
		location = location.parent_path();
#else
		location = location.branch_path();
#endif
		location.normalize();
		filepath = fs::path(location / fn);
	}

	if (fs::exists(filepath)) {
		VRJLUA_MSG_START(dbgVRJLUA, MSG_STATUS)
			<< "LuaPath: Found '" << filepath.string() << "'"
			<< VRJLUA_MSG_END(dbgVRJLUA, MSG_STATUS);
	} else {
		/// couldn't find it anywhere...
		throw std::runtime_error("Could not find file " + fn);
	}

	return location.normalize().string();
}


LuaPath& LuaPath::instance(std::string const& arg0, std::string const& vrjlua_base) {
	static LuaPath inst;
	if (!inst._valid) {
		inst._init(arg0, vrjlua_base);
	}
	return inst;
}

LuaPath::LuaPath() :
		_foundJuggler(false),
		_valid(false)
	{ }

LuaPath::~LuaPath() {
#ifdef VERBOSE
	std::cout << "In destructor " << __FUNCTION__ << std::endl;
#endif
}

void LuaPath::_init(std::string const& arg0, std::string const& vrjlua_base) {
	_valid = true;

	_initialPath = fs::initial_path().string();
	std::vector<std::string> startingPlaces;
	startingPlaces.push_back(_initialPath);
	startingPlaces.push_back(fs::complete(vrjlua_base).string());
#ifdef BOOST_FILESYSTEM_NO_DEPRECATED
	_exeDir = fs::complete(arg0).remove_filename().string();
	startingPlaces.push_back(fs::complete(arg0).remove_filename().string());
#else
	_exeDir = fs::complete(arg0).remove_leaf().string();
	startingPlaces.push_back(fs::complete(arg0).remove_leaf().string());
#endif

	_root = _findFilePath(startingPlaces, "share/vrjugglua/lua/vrjlua-init.lua");
	if (!_root.empty()) {
		_luaDir = (fs::path(_root) / "share/vrjugglua/lua/").string();
		startingPlaces.push_back((fs::path(_root) / "share/vrjugglua/").string());
	} else {
		_root = _findFilePath(startingPlaces, "vrjlua-init.lua");
		_luaDir = _root;
	}
	std::string sourceTreeIndicator = (fs::path(_luaDir)/"vrjlua-sourcetreeloc.lua").string();
	if (fs::exists(sourceTreeIndicator)) {
		std::ifstream df(sourceTreeIndicator.c_str());
		if (df.is_open()) {
			std::string line;
			std::getline(df, line);
			boost::algorithm::trim(line);
			if (!line.empty()) {
				VRJLUA_MSG_START(dbgVRJLUA, MSG_STATUS)
					<< "LuaPath: Presence of file " << sourceTreeIndicator
					<< " indicated we are running from a build tree, adding extra path hint " << line
					<< VRJLUA_MSG_END(dbgVRJLUA, MSG_STATUS);
				startingPlaces.push_back(line);
			}
		}
	}
	_shareDir = _findFilePath(startingPlaces, "assets/fonts/droid-fonts/DroidSans.ttf");
#if __VJ_version >= 2003000
	fs::path jugglerTest = "share/vrjuggler-3.0/data/definitions/simulated_positional_device.jdef";
#else
	fs::path jugglerTest = "share/vrjuggler-2.2/data/definitions/simulated_positional_device.jdef";
#endif
	fs::path jugglerTestFallback = "share/vrjuggler/data/definitions/simulated_positional_device.jdef";
	
	if (fs::exists(_root/jugglerTest) || fs::exists(_root/jugglerTestFallback)) {
		_foundJuggler = true;
		_jugglerRoot = _root;
	}


#ifdef VPR_OS_Linux
	if (!_foundJuggler) {
		std::string vprLibraryPath;
		dl_iterate_phdr(&sharedObjectCallback, &vprLibraryPath);
		if (!vprLibraryPath.empty()) {
			try {
#ifdef BOOST_FILESYSTEM_NO_DEPRECATED
				_jugglerRoot = _findFilePath(fs::complete(vprLibraryPath).remove_filename().string(), jugglerTest.string());
#else
				_jugglerRoot = _findFilePath(fs::complete(vprLibraryPath).remove_leaf().string(), jugglerTest.string());
#endif
				_foundJuggler = true;
			} catch (std::runtime_error &) {
				// nothing
			}
		}
	}
#endif

	if (!_foundJuggler) {
		vpr::System::getenv("VJ_BASE_DIR", _jugglerRoot);
	}

	_setJugglerEnvironment();
}

std::string LuaPath::findFilePath(std::string const& fn) {
	std::vector<std::string> startingPlaces;
	startingPlaces.push_back(_initialPath);
	startingPlaces.push_back(_exeDir);
	startingPlaces.push_back(_root);
	startingPlaces.push_back(_luaDir);
	return _findFilePath(startingPlaces, fn);
}

bool LuaPath::findAppRoot(std::string const& fn) {
	_appRoot = findFilePath(fn);
	return (!_appRoot.empty());
}

std::string const& LuaPath::getAppRoot() const {
	return _appRoot;
}
std::string const& LuaPath::getRootDir() const {
	return _root;
}

std::string const& LuaPath::getExeDir() const {
	return _exeDir;
}
std::string const& LuaPath::getShareDir() const {
	return _shareDir;
}
std::string const& LuaPath::getInitialPath() const {
	return _initialPath;
}

std::string LuaPath::getPathToLuaScript(const std::string & scriptfn) const {
	return (fs::path(_luaDir) / scriptfn).string();
}

void LuaPath::chdir(std::string const& path) {
	fs::current_path(path);
}

void LuaPath::addLuaRequirePath(LuaStatePtr state, std::string const& dirEndingInSlash) {
	_searchPaths.push_front(dirEndingInSlash + "?.lua");
	_searchPaths.push_front(dirEndingInSlash + "?");
	updateLuaRequirePath(state);
}

void LuaPath::updateLuaRequirePath(LuaStatePtr state) {
	if (_searchPaths.empty()) {
		_populateSearchPathsVector(state);
	}
	_setLuaSearchPaths(state);
}

bool LuaPath::_setJugglerEnvironment() const {
	bool ret;
	if (_foundJuggler) {
		/// Set these environment variables so that Juggler figures itself out.
		VRJLUA_MSG_START(dbgVRJLUA, MSG_STATUS)
			<< "LuaPath: "
			<< "Detected appropriate VJ_BASE_DIR to be "
			<< _jugglerRoot
			<< VRJLUA_MSG_END(dbgVRJLUA, MSG_STATUS);
		VRJLUA_MSG_START(dbgVRJLUA, MSG_STATUS)
			<< "LuaPath: "
			<< "Setting VJ_BASE_DIR."
			<< VRJLUA_MSG_END(dbgVRJLUA, MSG_STATUS);
		ret = vpr::System::setenv("VJ_BASE_DIR", _jugglerRoot);
		//ret = vpr::System::setenv("SNX_BASE_DIR", _root) && ret;
	} else {
		ret = (!_jugglerRoot.empty());
		VRJLUA_MSG_START(dbgVRJLUA, MSG_STATUS)
			<< "LuaPath: "
			<< "Couldn't auto-detect VJ_BASE_DIR, using what the environment has: "
			<< _jugglerRoot
			<< VRJLUA_MSG_END(dbgVRJLUA, MSG_STATUS);
	}
	return ret;
}

} // end of vrjLua namespace
