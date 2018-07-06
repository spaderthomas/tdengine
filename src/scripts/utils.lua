
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