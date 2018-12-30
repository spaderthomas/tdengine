screen_x = 640.0
screen_y = 360.0
tilesize_x = 16.0
tilesize_y = 16.0

tile_units = {}
tile_units['neg_one_and_half'] = -1.5
tile_units['neg_one_and_quarter'] = -1.25
tile_units['neg_one'] = -1.0
tile_units['neg_half'] = -.5
tile_units['half'] = .5
tile_units['one'] = 1.0
tile_units['one_and_quarter'] = 1.25
tile_units['one_and_half'] = 1.5
tile_units['two'] = 2.0
tile_units['three'] = 3.0
tile_units['four'] = 4.0
tile_units['five'] = 5.0
tile_units['six'] = 6.0
for name, size in tile_units.items():
    screen_unit_x = size * tilesize_x / screen_x
    print(name, "=", screen_unit_x)
print("\n")
for name, size in tile_units.items():
    screen_unit_y = size * tilesize_y / screen_y
    print(name, "=", screen_unit_y)
