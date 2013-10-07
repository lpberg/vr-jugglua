print("nav-app-skeleton starting up...")

require("Navigator")
require("Scene")
require("Actions")

RelativeTo = {
	Room = osg.Group(),
	World = osg.MatrixTransform()
}

osgnav = {position = osg.Vec3d(0, 0, 0)}

GlobalStateSet = nil

local setUpActions = false

function osgnav:initScene()
	if runbuf then
		runbuf:init()
	end
	--print("Setting up position interface")
	local wand = gadget.PositionInterface("VJWand")

	--print("Setting up digital interface")
	local button = gadget.DigitalInterface("VJButton0")

	--print("Creating navigator")
	self.nav = Navigator.create(maxspeed)
	Navigator.useWandTranslation(self.nav, wand, button)

	local scene = self.appProxy:getScene()
	--print("Setting up scenegraph")
	scene:addChild(RelativeTo.World)


	--print("Attaching to app proxy's scene")
	scene:addChild(RelativeTo.Room)

	GlobalStateSet = scene:getOrCreateStateSet()

	print("Scenegraph Navigation Testbed loaded!")
	print("")
	print("RelativeTo.World is your root-level group node for the world you can navigate.")
	print("RelativeTo.Room is the root group node for objects positioned relative to your physical space.")
	print("")
	print("Do groupnode:addChild(somenode) to attach 'somenode' to the scene graph")
	print("")
	if inSimulator then
		print("Simulator Mode Instructions --")
		print("In the simulator window:")
		print("   Hold 'shift' and move the mouse to rotate the simulated wand")
		print("   Hold 'ctrl' and move the mouse to move (translate) the wand in X and Y")
		print("   Hold 'alt' and move the mouse to move (translate) the wand in X")
		print("   Mouse buttons are mapped left, middle, right to VJButton0, 1, 2 respectively.")
		print("   To use a different configuration, pass the name of the jconf file at the command line.")
		print("")
	end

end

function osgnav:preFrame()
end

function osgnav:latePreFrame()
	if runbuf then
		runbuf:runBuffer()
	end
	if Actions then
		if not setUpActions then
			Actions.setAppProxy(osgnav.appProxy)
			setUpActions = true
		end
		Actions.updateFrameActions()
	end
end


print("Setting up scenegraph")
navtransform = osg.PositionAttitudeTransform()

print("App delegate defined, now creating OsgAppProxy")
osgnav.appProxy = vrjApp.OsgAppProxy()

print ("Now setting OsgAppProxy's delegate")
osgnav.appProxy:setAppDelegate(osgnav)

print("Setting kernel application")
osgnav.appProxy:setActiveApplication()


do
	local self = osgnav
	self.standardNavAction = Actions.addFrameAction(
		function(dt)
			local dt = dt
			while true do
				self.position = self.position - self.nav:getTranslation(dt, self.position)
				RelativeTo.World:setMatrix(osg.Matrix.translate(self.position))
				dt = Actions.waitForRedraw()
			end
		end
	)
end

osgnav.removeStandardNavigation = function()
	Actions.removeFrameAction(osgnav.standardNavAction)
end
