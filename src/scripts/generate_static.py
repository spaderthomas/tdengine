# The dumbest script ever for generating tile definitions in Lua, where the tile is always just one static image
tiles = [
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
    "fence_top", \
    "sand_path_bottomright_outer"]

with open("static_background.lua", "w") as file:
    for tile in tiles:
        path = tile + '.png'
        lua_def = '''
%s = {
    Graphic_Component = {
        Animations = {
            %s = {
                "%s.png"
            }
        },
        default_animation = "%s",
        z = 0
    },
    Position_Component = {
        translate = {
            x = 0,
            y = 0,
            z = 0
        },
        scale = {
            x = 16,
            y = 16
        },
        rad_rot = 0
    }
}
        ''' % (tile, tile, tile, tile)
        file.write(lua_def)