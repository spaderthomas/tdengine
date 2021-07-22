-- Class stuff
function tdengine.include_mixin(class, mixin)
  assert(type(mixin) == 'table', "mixin must be a table")

  for name, method in pairs(mixin) do
    if name ~= "static" then
	  class[name] = method
	end
  end

  for name,method in pairs(mixin.static or {}) do
    class.static[name] = method
  end

  return class
end

-- Goal: When you update a method and save it, make it so that re-scripting the file
-- will automatically update the method in all instances.
-- To do this, we need to have __index route all method calls to the most current
-- version of the class. That could be in a couple of places, depending on the class
-- type: _G for entities and components, and tdengine.actions for actions.
function tdengine.add_new_to_class(class, class_parent)
  class.static.new = function(self, ...)
	-- Create the table we'll return to the user
	local instance = {}

	-- Give it a metatable
	local metatable = {}
	metatable.__index = function(tbl, key)
	  -- It's important that we look this up in the global namespace, because
	  -- whenever we save the file and re-script it, it will show back up there.
	  -- If we just used 'class' or 'self', that change wouldn't propagate.
	  return class_parent[self.name].__methods[key]
	end
	setmetatable(instance, metatable)

	return instance
  end
end

function tdengine.create_class(name)
  local class = {
	name = name,
	static = {},
	__methods = {},
  }

  -- Allow the class to include mixins. Do not use 'class' as the parameter, because
  -- this will be called like ClassName:include(), and could be called multiple
  -- times
  class.static.include = function(class_table, ...)
	for _, mixin in ipairs({...}) do
	  tdengine.include_mixin(class_table, mixin)
	end
	return self
  end

  -- Set up the class to look in its methods if it calls something that isn't a static method
  on_static_method_not_found = function(_, key)
	return rawget(class.__methods,key)
  end
  
  setmetatable(class.static, {
				 __index = on_static_method_not_found
  })

  -- Set the class to add new fields to methods, and to check static methods before instance methods
  local metatable = {
	__index = class.static,
	__tostring = function(self) return self.name end,
	__newindex = function(class, method_name, method)
	  class.__methods[method_name] = method
	end
  }
  setmetatable(class, metatable)

  return class
end
