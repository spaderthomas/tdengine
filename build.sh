#!/bin/sh
mkdir build > /dev/null
cd build

python3 ../src/scripts/generate_entity_tree.py
python3 ../src/scripts/generate_tiles.py

/usr/local/Cellar/gcc/8.3.0/bin/g++-8 -g -fpermissive -std=c++17 \
../include/imgui/*.cpp ../src/win_main.cpp ../include/glad/glad.c \
-I../include \
/Users/thspader/Programming/tdengine/lib/libfreetype.a /Users/thspader/Programming/tdengine/lib/libglfw3.a -lobjc -lbz2 -lz -lstdc++fs \
-framework Cocoa -framework OpenGL -framework Cocoa -framework CoreVideo -framework IOKit \
-o tdengine
