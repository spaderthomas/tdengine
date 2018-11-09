
--We're only using these to calculate ratios, so it's fine to hardcode this. Once we have the ratios, this works for any resolution.
screen_x = 640
screen_y = 360
tilesize_x = 16
tilesize_y = 16

function tile_x_to_screen (tile_x)
    return tile_x * tilesize_x  / screen_x
end
function tile_y_to_screen (tile_y)
    return tile_y * tilesize_y / screen_y
end
function px_x_to_screen (px_x)
    return px_x / screen_x
end
function px_y_to_screen (px_y)
    return px_y / screen_y
end

Collider_Kind = {
   STATIC = 0,
   DYNAMIC = 1,
   NO_COLLIDER = 2
}

Render_Flags = {
   NO_FLAGS = 0,
   HIGHLIGHTED = 1
}

