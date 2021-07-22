-- Utilities
tdengine.colors = {}
tdengine.colors.red =   { r = 1, g = 0, b = 0, a = 1 }
tdengine.colors.green = { r = 0, g = 1, b = 0, a = 1 }
tdengine.colors.blue =  { r = 0, g = 0, b = 1, a = 1 }
tdengine.colors.idk =   { r = 0, g = .75, b = 1, a = 1 }
tdengine.colors.grid_bg =  { r = .25, g = .25, b = .3, a = .8 }

function table.shallow_copy(t)
  local t2 = {}
  for k,v in pairs(t) do
	t2[k] = v
  end
  return t2
end

function tdengine.do_once(f, ...)
  if string.dump(tdengine.last_do_once) ~= string.dump(f) then
	f(...)
	tdengine.last_do_once = f
  end
end
tdengine.last_do_once = function() end

function tdengine.platform()
  local separator = package.config:sub(1,1)
  if separator == '\\' then
	return 'Windows'
  elseif separator == '/' then
	return 'Unix'
  else
	return ''
  end
end

function tdengine.is_extension(path, extension)
  local ext_len = string.len(extension)
  local path_len = string.len(path)
  if ext_len > path_len then return false end

  local last = string.sub(path, path_len - ext_len + 1, path_len)
  return last == extension
end

function tdengine.has_extension(path)
  return string.find(path, '%.')
end

function tdengine.strip_extension(path)
  local extension = tdengine.has_extension(path)
  if not extension then return path end

  return path:sub(1, extension - 1)
end

function tdengine.scandir(dir)
  local platform = tdengine.platform()
  local command = ''
  if platform == 'Unix' then command = 'ls -a "' .. dir .. '"' end
  if platform == 'Windows' then command = 'dir "' .. dir .. '" /b' end

  local i, t, popen = 0, {}, io.popen
  local pfile = popen(command)
  for filename in pfile:lines() do
	if filename ~= '.' and filename ~= '..' then 
	  i = i + 1
	  t[i] = filename
	end
  end
  pfile:close()
  return t
end

function tdengine.write_file_to_return_table(filepath, t)
  local file = assert(io.open(filepath, 'w'))
  if file then
	local serpent = require('serpent')
	file:write('return ')
	file:write(serpent.block(t, { comment = false }))
  else
	print('@cannot_open_file: ' .. filepath)
  end
end

function tdengine.fetch_module_data(module_path, log)
  log = ternary(log == nil, true, log)
  package.loaded[module_path] = nil
  local status, data = pcall(require, module_path)
  if not status then
	if log then
	  tdengine.log('@module_load_failure')
	  tdengine.log(data)
	end
	return nil
  end

  return data
end

function tdengine.screen_to_world(screen)
  local camera = tdengine.vec2(tdengine.camera())
  local screen = tdengine.vec2(screen)
  return screen:subtract(camera)
end		 

function tdengine.paths.script(relative)
  return tdengine.paths.absolute('src/scripts/' .. relative)
end

function tdengine.uuid()
  math.randomseed(os.time())
  local random = math.random
  local template ='xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'
  local sub = function (c)
	local v = (c == 'x') and random(0, 0xf) or random(8, 0xb)
	return string.format('%x', v)
  end
  return string.gsub(template, '[xy]', sub)
end

function tdengine.color(r, g, b, a)
  return { r = r, g = g, b = b, a = a }
end

function tdengine.color32(r, g, b, a)
  a = a * math.pow(2, 24)
  b = b * math.pow(2, 16)
  g = g * math.pow(2, 8)
  return r + g + b + a
end


local vec2_mixin = {
  unpack = function(self)
	return self.x, self.y
  end,
  add = function(self, other)
	return tdengine.vec2(self.x + other.x, self.y + other.y)
  end,
  subtract = function(self, other)
	return tdengine.vec2(self.x - other.x, self.y - other.y)
  end,
  scale = function(self, scalar)
	return tdengine.vec2(self.x * scalar, self.y * scalar)
  end,
  truncate = function(self, digits)
	return tdengine.vec2(truncate(self.x, digits), truncate(self.y, digits))
  end,
  abs = function(self)
	return tdengine.vec2(math.abs(self.x), math.abs(self.y))
  end,
  equals = function(self, other, eps)
	eps = eps or tdengine.deq_epsilon
	return double_eq(self.x, other.x, eps) and double_eq(self.y, other.y, eps)
  end
}

tdengine.vec2_impl = tdengine.create_class('vec2_impl')
tdengine.add_new_to_class(tdengine.vec2_impl, tdengine)
tdengine.vec2_impl:include(vec2_mixin)
tdengine.vec2 = function(x, y)
  local vec = tdengine.vec2_impl:new()

  if type(x) == 'table' then
	vec.x = x.x
	vec.y = x.y
	return vec
  else
	vec.x = x
	vec.y = y
	return vec
  end
end

function tdengine.frames(n)
  return n / 60
end

function delete(array, value)
  local len = #array
  
  for index, v in pairs(array) do
	if v == value then
	  array[index] = nil
	end
  end

  local next_available = 0
  for check = 1, len do
	if array[check] ~= nil then
	  next_available = next_available + 1
	  array[next_available] = array[check]
	end
  end

  for remove = next_available + 1, len do
	array[remove] = nil
  end
end

function contains(t, k)
  return t[k] ~= nil
end

function ternary(cond, a, b)
  if cond then return a else return b end
end

function average(a, b)
  return (a + b) / 2
end

tdengine.deq_epsilon = .00000001
function double_eq(x, y, eps)
  eps = eps or tdengine.deq_epsilon
  return math.abs(x - y) < eps
end

function is_newline(c)
  return c == '\n'
end

function is_space(c)
  return c == ' '
end

tdengine.op_or, tdengine.op_xor, tdengine.op_and = 1, 3, 4

function bitwise(oper, a, ...)
  -- base case 1: the parameter pack is empty. return nil to signal.
  if a == nil then
	return nil
  end

  local b = bitwise(oper, ...)

  -- base case 2: we're at the end of the parameter pack. just return yourself.
  if b == nil then
	return a
  end
  
  local r, m, s = 0, 2^31
  repeat
	s,a,b = a+b+m, a%m, b%m
	r,m = r + m*oper%(s-a-b), m/2
  until m < 1
  return r
end

function truncate(float, digits)
  local mult = 10 ^ digits
  return math.modf(float * mult) / mult
end
