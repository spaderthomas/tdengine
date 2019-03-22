@echo off
python3 src/scripts/generate_entity_tree.py
python3 src/scripts/generate_tiles.py

if not exist build mkdir build
pushd build

cl.exe -Zi /nologo /std:c++17 /MDd /D_CRT_SECURE_NO_WARNINGS /W2 /wd4530 /wd4201 /wd4577 /wd4310 /wd4624 "..\include\imgui\imgui.cpp" "..\include\imgui\imgui_demo.cpp" "..\include\imgui\imgui_draw.cpp" "..\include\imgui\imgui_widgets.cpp" "..\include\glad\glad.c" "..\src\win_main.cpp" /I"..\include" /link user32.lib opengl32.lib gdi32.lib Shell32.lib Kernel32.lib "..\lib\freetyped.lib" "..\lib\glfw3.lib" /IGNORE:4099 /MACHINE:X86 /out:"tdengine.exe"

popd