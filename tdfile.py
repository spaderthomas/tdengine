import os, platform, subprocess, tempfile
import tdbuild.tdbuild as tdbuild

build_options = {
    'source_dir': 'src',
    'include_dirs': [
        'include',
        os.path.join('include', 'lua')
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
            'glfw3',
            'lua51'
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
setup_devenv = '''@echo off
if not defined DevEnvDir (
   call "C:/Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
)'''
setup_devenv = 'echo imascript'

class Builder(tdbuild.base_builder):
    def __init__(self):
        super().__init__()

    def build(self):
        if platform.system() == 'Windows':
            devenv_script = tempfile.NamedTemporaryFile('w')
            devenv_script.write(setup_devenv)
            subprocess.run([devenv_script.name], stdout=subprocess.PIPE)
            devent_script.close()
        super().build()
        
    def run(self):
        super().run()
        
    def setup(self):
        super().setup()
        
    def prebuild(self):
        pass
    
if __name__ == '__main__':
    import os, shutil
    import glob

    header_names = [
        'imstb_rectpack.h',
        'imstb_textedit.h',
        'imstb_truetype.h',
        'imconfig.h',
        'imgui.h',
        'imgui_internal.h',
    ]
    imgui_dir = os.path.abspath(os.path.join('..', 'imgui'))
    
    tdengine_imgui_include = os.path.abspath(os.path.join('.', 'include', 'imgui'))
    headers = glob.glob(os.path.join(imgui_dir, '*.h'))
    for header in headers:
        shutil.copy(header, tdengine_imgui_include)
        print(header)

    tdengine_imgui_src = os.path.abspath(os.path.join('.', 'src', 'imgui'))
    sources = glob.glob(os.path.join(imgui_dir, '*.cpp'))
    for source in sources:
        shutil.copy(source, tdengine_imgui_src)
        print(source)

    print('fixin up sources')
    sources = glob.glob(os.path.join(tdengine_imgui_src, '*.cpp'))
    for source in sources:
        with open(source, 'r+') as f:
            lines = f.readlines()
            for i, line in enumerate(lines):
                for header_name in header_names:
                    include = f'#include "{header_name}"'
                    if include in line:
                        lines[i] = f'#include "imgui/{header_name}"\n'
                        print(source)
                        print(line)

            f.seek(0)
            f.write(''.join(lines))

