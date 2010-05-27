/**	@file	OsgAppProxy.h
	@brief Application proxy object header

	@date
	2009-2010

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/
#pragma once
#ifndef INCLUDED_vrjugglua_OsgAppProxy_h
#define INCLUDED_vrjugglua_OsgAppProxy_h


// Local includes
#include <vrjugglua/LuaScript.h>

// OSG includes
#include <osg/Group>
#include <osgUtil/SceneView>

// VR Juggler includes
#include <vrj/vrjConfig.h>
#include <vrj/Draw/OSG/OsgApp.h>
#include <gadget/Type/PositionInterface.h>

// LuaBind includes
#include <luabind/object.hpp>

// Standard includes
#include <string>
#include <vector>

namespace vrjLua {

class OsgAppProxy : public vrj::OsgApp {
	public:
	/// @brief Lua binding call
	static void bindToLua(LuaStatePtr & state);


	/// @brief Standard constructor
	OsgAppProxy(vrj::Kernel* kern);

	/// @brief simple destructor
	virtual ~OsgAppProxy();

	/// @name Lua-accessible methods
	/// @{

	/// @brief Default constructor, called from Lua
	OsgAppProxy();

	/// @brief Set this application as the current one in the kernel
	void setActiveApplication();

	/// @brief Set the Lua table serving as the app delegate
	void setAppDelegate(luabind::object const & delegate);

	/// @brief Retrieve the Lua table serving as the app delegate
	luabind::object const & getAppDelegate();

	/** Accessor for the scene root node
		@returns a pointer to the scene root group.
	*/
	virtual osg::Group* getScene();

	/** Interval between the most recent (current) preFrame and the previous.

		This is an accessor whose data member is updated in preFrame before
		calling the delegate.
		@returns a float representing the time interval.
	*/
	double getTimeDelta();
	/// @}

	/** Static accessor for the app pointer.
		@returns the pointer to the singleton app object.
	*/
	static OsgAppProxy* getApp();

	/** Set up the properties for our SceneView (lighting, etc)
	*/
	virtual void configSceneView(osgUtil::SceneView* newSceneViewer);

	/** This method is overridden to fix per-wall lighting issue in
		CAVEs.
	 */
	unsigned int getSceneViewDefaults();

	/// @name Methods proxied to the Lua delegate
	/// @{
	/** Execute any initialization needed before the API is started.

		This is called once before OSG is initialized.
	 */
	virtual void initScene();


	// ----- Drawing Loop Functions ------
	//
	//  The drawing loop will look similar to this:
	//
	//  while (drawing)
	//  {
	//        preFrame();
	//       <Application Data Syncronization>
	//        latePreFrame();
	//       draw();
	//        intraFrame();     // Drawing is happening while here
	//       sync();
	//        postFrame();      // Drawing is now done
	//
	//       UpdateTrackers();
	//  }
	//------------------------------------

	/** Called after getting input data but before syncing shared data.

		In this implementation, our timeDelta is updated then all of
		the frame logic units are called.

		This is the best time to call "setNextState()"

		@warning Any operation placed here increases total input latency!
	*/
	virtual void preFrame();

	/** Called after getting input data and syncing shared data.

		In this implementation, all of the frame logic units are called,
		then the parent class OsgApp's implementation is called.

		@warning Any operation placed here increases total input latency!
	*/
	virtual void latePreFrame();

	/** Called after drawing starts, during the draw process.

		In this implementation, all of the frame logic units are called.

		@warning Do not modify any information used in drawing here!
	*/
	virtual void intraFrame();

	/** Called once drawing has concluded, before getting the next inputs.

		In this implementation, all of the frame logic units are called.
		This means that the first thing that happens is that the primary
		state machine will advance to its next state (calling the go()
		method), then all other frame logic will take place.

		(All FrameStateMachines will also trigger their go() method here,
		mixed in with the rest of the Frame Logic Units in order of
		registration.)
	*/
	virtual void postFrame();
	/// @}



	protected:
	/**	Static app pointer - do not access directly	*/
	static OsgAppProxy* _pApp;

	private:
	osg::ref_ptr<osg::Group>           _rootNode;

	bool _forwardCallToDelegate(const char * call);
	luabind::object _delegate;

	/**	Time of the start of the last preframe.

		Updated each preframe using the user's head timestamp
	*/
	vpr::Interval _lastPreFrameTime;
	double _timeDelta;

	std::string _luaFn;
};

// -- inline implementations -- /
inline OsgAppProxy* OsgAppProxy::getApp() {
	return _pApp;
}

inline osg::Group* OsgAppProxy::getScene() {
	return _rootNode.get();
}

inline unsigned int OsgAppProxy::getSceneViewDefaults() {
	return osgUtil::SceneView::NO_SCENEVIEW_LIGHT;
}

inline double OsgAppProxy::getTimeDelta() {
	return _timeDelta;
}

} // end of vrjLua namespace
#endif // INCLUDED_vrjugglua_LuaOSGApp_h
