
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
	return nil, "Must call Vec with 2, 3, or 4 elements!"
end

function AngleAxis(angle, axis)
	return osg.Quat(angle, axis)
end

function Lighting(a)
	local t = osg.LightSource()
	local light = osg.Light()
	if a.ambient then
		light:setAmbient(osg.Vec4(a.ambient, a.ambient, a.ambient, 1.0))
	end

	if a.diffuse then
		light:setDiffuse(osg.Vec4(a.diffuse, a.diffuse, a.diffuse, 1.0))
	end

	if a.specular then
		light:setSpecular(osg.Vec4(a.specular, a.specular, a.specular, 1.0))
	end
	t:setLight(light)

	-- Add nodes in the "children" list
	if a.children ~= nil then
		for _, v in ipairs(a.children) do
			if v ~= nil then
				t:addChild(v)
			end
		end
	end

	-- Add nodes just tacked on the end of the list.
	for _, v in ipairs(a) do
		if v ~= nil then
			t:addChild(v)
		end
	end

	return t
end

function Light(...)
	local arg = {...}
	local num = arg.number or 0 --- todo improve this
	local light = osg.Light(num)

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
	do
		local pos = {0, -1, 0, 1}
		local posOSG
		local setLightType
		if arg.directional then
			pos[4] = 0
			setLightType = "directional"
		elseif arg.positional then
			pos[4] = 1
			setLightType = "positional"
		end
		if arg.position then
			if type(arg.position) == "table" then
				for i=1,3 do
					pos[i] = arg.position[i]
				end
				if #arg.position == 4 then
					if setLightType then
						print("Warning: fourth value passed to 'position' overrides setting of light type as " .. setLightType)
					end
					pos[4] = arg.position[4]
				end

				posOSG = osg.Vec4(unpack(pos))
			else
				--- Assume it's a vec4 - perhaps a bad assumption
				print("WARNING: Assuming your position value is an osg.Vec4")
				posOSG = arg.position
			end
		end
		light:setPosition(posOSG)
	end

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

function Transform(args)
	local t = osg.PositionAttitudeTransform()
	if args.position ~= nil then
		if #args.position ~=3 then
			print("Cannot set position: position must have 3 elements!")
		else
			t:setPosition(osg.Vec3d(
				args.position[1],
				args.position[2],
				args.position[3]))
		end
	end

	if args.orientation ~= nil then
		t:setAttitude(args.orientation)
	end

	if args.scale ~= nil and args.scale ~= 1.0 then
		t:setScale(osg.Vec3d(args.scale, args.scale, args.scale))
		local GL_NORMALIZE = 0x0BA1
		local GL_RESCALE_NORMAL = 0x803A
		t:getOrCreateStateSet():setMode(GL_RESCALE_NORMAL, 1)
	end

	-- Add nodes in the "children" list
	if args.children ~= nil then
		for _, v in ipairs(args.children) do
			if v ~= nil then
				t:addChild(v)
			end
		end
	end

	-- Add nodes just tacked on the end of the list.
	for _, v in ipairs(args) do
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
		print("Warning: failed to load model " .. filename)
		return nil
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
