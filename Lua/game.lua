if not _game_lua then

game = {data = {}}
dofile("Lua/map.lua");

game.START = 1

function game:_write_table(f, t)
	local first = true
	for key, value in pairs(t) do
		if not first then
			f:write(",\n")
		else
			first = false
		end

		if type(value) == "table" then
			f:write(key .. " = {\n")
			self:_write_table(f, value)
			f:write("\n}")
		elseif type(value) == "string" then
			f:write(key .. " = \"" .. value .. "\"")
		else
			f:write(key .. " = " .. value)
		end
	end
end

function game:create_map()
	self.active_map = {}
	setmetatable(self.active_map, self._map_class)
	self._map_class.__index = self._map_class
	return self.active_map
end

function game:get_active_map()
	return self.active_map
end

function game:load(filename)
	self:cleanup()
	self.data = nil
	self.data = {}
	dofile("Saves/" .. filename)
	--self.active_map:warp(self.active_map.player, self.data.position_x, self.data.position_y)
	self:exec_events("on_load")
end

function game:save(filename)
	f = io.open(filename, "w")
	f:write("game.data = {\n")
	self:_write_table(f, self.data)
	f:write("\n}")

	local map = game:get_active_map()
	if map then
		f:write("dofile(\"Levels/" .. map.level_name .. ".lua\")\n")
	end

	f:close()
end

else
_game_lua = true
end
