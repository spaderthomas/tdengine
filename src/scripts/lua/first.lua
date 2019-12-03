print("hello from first.lua!")

some_table = {
   value = 1
}

function first()
   print("You just called a function from first.lua")
end

return {
   first = first
}
