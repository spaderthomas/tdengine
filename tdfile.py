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
        #'dl_stub.c',
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
            'freetyped.lib',
            'glfw3.lib',
            'lua51.lib'
        ],
        'dlls': [
            'lua51.dll'
        ],
        'ignore': [
            '4099',
            '4068'   # unknown pragma
        ],
        'machine': 'X86',
        'out': 'tdengine.exe',
        'runtime_library': 'MDd',
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

# I really hate having this file in my project
def find_devenv_script():
    potential_devenv_scripts = [
        "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvars32.bat",
        "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars32.bat"
    ]
    for script in potential_devenv_scripts:
        if os.path.exists(os.path.normpath(script)):
            print('Found devenv setup script at {}'.format(script))
            return script

    return None
setup_devenv = '''@echo off
if not defined DevEnvDir (
   call "{}"
)'''

class Builder(tdbuild.base_builder):
    def __init__(self):
        super().__init__()

    def build(self):
        super().build()
        # print("fuckkkkkk")
        # import subprocess
        # if platform.system() == 'Windows':
        #     devenv_script = tempfile.NamedTemporaryFile('w')
        #     command = ['call', find_devenv_script()]
        #     prog = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #     out, err = prog.communicate()
        #     print(out)
        #     print(err)
        #     devenv_script.close()
        
    def run(self):
        super().run()
        
    def setup(self):
        pass

