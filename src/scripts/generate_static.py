# The dumbest script ever for generating tile definitions in Lua, where the tile is always just one static image
static_things = [
    "grass", \
    "grass_flower1", \
    "grass_flower2", \
    "grass_flower3", \
    "sand", \
    "sand_cracked", \
    "sand_path_top", \
    "sand_path_bottom", \
    "sand_path_left", \
    "sand_path_right", \
    "sand_path_topleft_inner", \
    "sand_path_topright_inner", \
    "sand_path_bottomleft_inner", \
    "sand_path_bottomright_inner", \
    "sand_path_topleft_outer", \
    "sand_path_topright_outer", \
    "sand_path_bottomleft_outer", \
    "sand_path_bottomright_outer"]

with open("static_background.lua", "w") as file:
    for static_thing in static_things:
        path = static_thing + '.png'
        lua_def = static_thing + ' = {\n\tGraphic_Component = {\n\t\tAnimations = {\n\t\t\t' + static_thing + '= {\n\t\t\t\t\"' + path + "\"\n\t\t\t}\n\t\t},\n\tdefault_animation = \"" + static_thing + "\",\n\tz = 0\n\t}\n}\n"
        file.write(lua_def)
        
