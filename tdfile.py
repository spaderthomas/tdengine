import os, platform, subprocess, tempfile
import tdbuild.tdbuild as tdbuild

build_options = {
    'source_dir': 'src',
    'include_dirs': [
        'include',
        os.path.join('include', 'lua'), # symlinked to /usr/include on linux
        os.path.join('include', 'freetype'), # symlinked to /usr/include on linux
        os.path.join('include', 'sol')
    ],
    'lib_dir': 'lib',
    'build_dir': 'build',
    'source_files': [
        os.path.join('imgui', 'imgui.cpp'),
        os.path.join('imgui', 'imgui_demo.cpp'),
        os.path.join('imgui', 'imgui_draw.cpp'),
        os.path.join('imgui', 'imgui_widgets.cpp'),
        os.path.join('imgui', 'imgui_tables.cpp'),
        'glad.c',
        'main.cpp'
    ],
    'debug': True,
    'cpp': True,
    'cpp_standard': '17',
    'Windows': {
        'system_libs': [
            'user32.lib',
            'opengl32.lib',
            'gdi32.lib',
            'Shell32.lib',
            'Kernel32.lib'
        ],
        'user_libs': [
            'glfw3.lib',
            'lua51.lib',
            'freetyped.lib',
        ],
        'dlls': [
            'freetyped.dll',
            'lua51.dll'
        ],
        'ignore': [
            '4099',
            '4068'   # unknown pragma
        ],
        'machine': 'X64',
        'out': 'tdengine.exe',
        'runtime_library': 'MD',
        'warnings': [
            '4530',
            '4201',
            '4577',
            '4310',
            '4624'
        ],
        'extras': [
            '/nologo',
            '/D_CRT_SECURE_NO_WARNINGS',
            '/W2'
        ]
    },
    'Darwin': {
        'compiler': 'g++-9',
        'user_libs': [
            'libfreetype.a',
            'libglfw3.a'
        ],
        'system_libs': [
            'objc',
            'bz2',
            'z',
        ],
        'frameworks': [
            'Cocoa',
            'OpenGL',
            'CoreVideo',
            'IOKit'
        ],
        'out': 'tdengine',
        'extras': [
            '-Wall',
            '-fpermissive'
        ]
    },
    'Linux': {
        'compiler': 'g++',
        'user_libs': [
            'freetype',
            'glfw',
            'luajit-5.1'
        ],
        'system_libs':[
            #'z',
            'GL',
            'X11',
            'c',
            'dl',
            'stdc++fs',
        ],
        'extras': [
            '-pthread',
            '-fmax-errors=10',
        ],
        'out': 'tdengine'
    }
}

class Builder(tdbuild.base_builder):
    def __init__(self):
        super().__init__()

    def build(self):
        super().build()

    def run(self):
        super().run()
        
    def setup(self):
        tdbuild.print_info('creating src/machine_conf.hpp')
        cwd = os.path.normpath(os.getcwd())
        cwd = os.path.join(cwd, '')
        cwd = cwd.replace('\\', '/')
        print(cwd)
        
        machine_conf = os.path.join(cwd, 'src', 'machine_conf.hpp')

        code = 'std::string root_dir = "{}";'.format(cwd)
        with open(machine_conf, 'w') as f:
            f.write(code)
        pass

    def prebuild(self):
        pass
