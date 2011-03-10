
function Degrees(d)
	return d * 3.141592653589 / 180
end

function Radians(r)
	return r
end

ScaleFrom = {
	yards = 0.9144,
	feet = 0.3048,
	inches = 0.0254,
	mm = 0.001,
	millimeters = 0.001,
	cm = 0.01,
	centimeters = 0.01,
	meters = 1.0,
	m = 1.0
}

function Axis(coords)
	return osg.Vec3d(coords[1], coords[2], coords[3])
end

Vec3 = Axis

function Vec(x, y, z, w)
	if w ~= nil then
		return osg.Vec4d(x, y, z, w)
	end

	if z ~= nil then
		return osg.Vec3d(x, y, z)
	end

	if y ~= nil then
		return osg.Vec2d(x, y)
	end

	-- OK, so they didn't pass the arguments one-by-one, but rather
	-- as a list

	if #x == 4 then
		return osg.Vec4d(x[1], x[2], x[3], x[4])
	elseif #x == 3 then
		return osg.Vec3d(x[1], x[2], x[3])
	elseif #x == 2 then
		return osg.Vec2d(x[1], x[2])
	end

	-- Hmm, well, they didn't pass as a list either. Bail!
	error("Must call Vec with 2, 3, or 4 elements!", 2)
end

function AngleAxis(angle, axis)
	return osg.Quat(angle, axis)
end

function Lighting(a)
	return LightSource(Light(a))
end

function Light(...)
	local arg = {...}
	local num = arg.number
	if num == nil then
		num = 0 --- todo improve this
	end
	local light = osg.Light()
	light:setLightNum(num)

	if arg.ambient then
		if type(arg.ambient) == "number" then
			arg.ambient = osg.Vec4(arg.ambient, arg.ambient, arg.ambient, 1.0)
		else
			print("WARNING: Assuming your ambient value is an osg.Vec4")
		end
		light:setAmbient(arg.ambient)
	end

	if arg.diffuse then
		if type(arg.diffuse) == "number" then
			arg.diffuse = osg.Vec4(arg.diffuse, arg.diffuse, arg.diffuse, 1.0)
		else
			print("WARNING: Assuming your diffuse value is an osg.Vec4")
		end
		light:setDiffuse(arg.diffuse)
	end

	if arg.specular then
		if type(arg.specular) == "number" then
			arg.specular = osg.Vec4(arg.specular, arg.specular, arg.specular, 1.0)
		else
			print("WARNING: Assuming your specular value is an osg.Vec4")
		end
		light:setSpecular(arg.specular)
	end

	-- Always set position
	local setLightType
	local fourthPosition = 1
	if arg.directional then
		fourthPosition = 0
		setLightType = "directional"
	elseif arg.positional then
		fourthPosition = 1
		setLightType = "positional"
	end
	if arg.position == nil then
		light:setPosition(osg.Vec4(0, 1, -1, fourthPosition))
	else
		if type(arg.position) == "table" then
			if #(arg.position) == 4 then
				if setLightType then
					print("Warning: fourth value passed to 'position' overrides setting of light type as " .. setLightType)
				end
			else
				arg.position[4] = fourthPosition
			end
			local osgvec = osg.Vec4(arg.position[1], arg.position[2], arg.position[3], arg.position[4])
			print("Setting light position to " .. tostring(osgvec))
			light:setPosition()
			
		else
			--- Assume it's a vec4 - perhaps a bad assumption
			print("WARNING: Assuming your position value is an osg.Vec4")
			light:setPosition(arg.position)
		end
	end
	
	if arg.direction then
		if type(arg.direction) == "table" then
			arg.direction = osg.Vec3(arg.direction[1], arg.direction[2], arg.direction[3])
		else
			print("WARNING: Assuming your direction value is an osg.Vec3")
		end
		arg.direction:normalize()
		light:setDirection(arg.direction)
	end

	return light
end

function LightSource(light)
	local source = osg.LightSource()
	source:setLight(light)
	return source

end

function AmbientIntensity(a)
	local node
	if #a == 1 then
		node = a[1]
	else
		node = osg.Group()
		for _,v in ipairs(a) do
			node:addChild(v)
		end
	end

	local state = node:getOrCreateStateSet()
	local lightmodel = osg.LightModel()
	local val = a.intensity or 1.0

	lightmodel:setAmbientIntensity(osg.Vec4(val, val, val, 1.0))

	state:setAttributeAndModes(lightmodel)
	node:setStateSet(state)

	return node
end

function Group(arg)
	local t = osg.Group()
	-- Add nodes just tacked on the end of the list.
	for _, v in ipairs(arg) do
		if v ~= nil then
			t:addChild(v)
		end
	end

	return t
end

function Transform(arg)
	local t = osg.PositionAttitudeTransform()
	if arg.position ~= nil then
		if #arg.position ~=3 then
			error("Cannot set position: position must have 3 elements!", 2)
		else
			t:setPosition(osg.Vec3d(
				arg.position[1],
				arg.position[2],
				arg.position[3]))
		end
	end

	if arg.orientation ~= nil then
		t:setAttitude(arg.orientation)
	end

	if arg.scale ~= nil and arg.scale ~= 1.0 then
		t:setScale(osg.Vec3d(arg.scale, arg.scale, arg.scale))
		local GL_NORMALIZE = 0x0BA1
		local GL_RESCALE_NORMAL = 0x803A
		t:getOrCreateStateSet():setMode(GL_RESCALE_NORMAL, 1)
	end

	-- Deprecated: Add nodes in the "children" list
	if arg.children ~= nil then
		error("No longer need to pass children={}, just list the children directly.", 2)
	end

	-- Add nodes just tacked on the end of the list.
	for _, v in ipairs(arg) do
		if v ~= nil then
			t:addChild(v)
		end
	end

	return t
end

function Model(filename)
	local m = osgLua.loadObjectFile(filename)
	local c = osgLua.getTypeInfo(m)

	if c and #c.methods > 0 then
		-- Model loaded successfully
		return m
	else
		error("Failed to load model " .. filename, 2)
	end
end

function Sphere(a)
	local pos = osg.Vec3(0.0, 0.0, 0.0)
	if a.position then
		pos:set(unpack(a.position))
	end

	local drbl = osg.ShapeDrawable(osg.Sphere(pos, a.radius or 1.0))
	local geode = osg.Geode()
	geode:addDrawable(drbl)
	return geode
end

function Geode(arg)
	if type(arg) ~= "table" then
		arg = {arg}
	end
	local g = osg.Geode()
	for _, v in ipairs(arg) do
		g:addDrawable(v)
	end
	return g
end
