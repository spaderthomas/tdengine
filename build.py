import os, subprocess, sys, shutil, platform, colorama

colorama.init()

build_options = {
    'source_dir': os.path.join('..', 'src'),
    'include_dirs': [
        os.path.join('..', 'include')
    ],
    'lib_dir': os.path.join('..', 'lib'),
    'build_dir': 'build',
    'source_files': [
        os.path.join("..", "include", "imgui", "imgui.cpp"),
        os.path.join("..", "include", "imgui", "imgui_demo.cpp"),
        os.path.join("..", "include", "imgui", "imgui_draw.cpp"),
        os.path.join("..", "include", "imgui", "imgui_widgets.cpp"),
        os.path.join("..", "include", "glad", "glad.c"),
        "win_main.cpp"
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
            'glfw3.lib'
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
            'stdc++fs'
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
    }
}

NEW_FONT_CONFIG = '# NEW_FONT_CONFIG'

def make_cd_build_dir():
    build_dir = os.path.join(os.getcwd(), build_options['build_dir'])
    try:
        os.mkdir(build_dir)
    except:
        pass
    os.chdir(build_dir)

def print_info(message):
    print(colorama.Fore.BLUE + "[tdbuild] " + colorama.Fore.RESET + message)

def print_error(message):
    print(colorama.Fore.RED + "[tdbuild] " + colorama.Fore.RESET + message)

def print_warning(message):
    print(colorama.Fore.YELLOW + "[tdbuild] " + colorama.Fore.RESET + message)
    
def print_success(message):
    print(colorama.Fore.GREEN + "[tdbuild] " + colorama.Fore.RESET + message)

def quote(string):
    return '"{}"'.format(string)

class tdbuild():
    def __init__(self):        
        self.build_cmd = ""
        self.unix_args = []

    def push(self, item):
        self.build_cmd = self.build_cmd + item + " "
        
    def build(self):
        if platform.system() == 'Windows':
            self.build_windows()
        elif platform.system() == 'Darwin':
            self.build_mac()
        
    def build_mac(self):
        print_info("Running from {}".format(os.getcwd()))

        # Find the path to the compiler using 'which'
        compiler = build_options['Darwin']['compiler']
        process = subprocess.Popen(['which', compiler], stdout=subprocess.PIPE)
        compiler_path, err = process.communicate()
        compiler_path = compiler_path.decode('UTF-8').strip()
        if err or not compiler_path:
            print_error("which {} errored out, so not sure what's up with that".format(build_options['Darwin']['compiler']))
            exit()
            
        self.push(compiler_path)

        if build_options['cpp']:
            self.push("-std=c++{}".format(build_options['cpp_standard']))

        if build_options['debug']:
            self.push("-g")

        for extra in build_options['Darwin']['extras']:
            self.push(extra)

        for source in build_options['source_files']:
            self.push(os.path.join(build_options['source_dir'], source))

        for include in build_options['include_dirs']:
            self.push("-I" + include)

        for lib in build_options['Darwin']['user_libs']:
            self.push(os.path.join(build_options['lib_dir'], lib))

        for lib in build_options['Darwin']['system_libs']:
            self.push('-l' + lib)

        for framework in build_options['Darwin']['frameworks']:
            self.push('-framework ' + framework)

        self.push('-o ' + build_options['Darwin']['out'])
        
        print_info("Generated compiler command:")
        print(self.build_cmd.split(" "))
        print_info("Invoking the compiler")

        make_cd_build_dir()

        process = subprocess.Popen(self.build_cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        compiler_messages, err = process.communicate()
        compiler_messages = compiler_messages.decode('UTF-8').split('\n')
        err = err.decode('UTF-8').split('\n')

        compile_error = False
        compile_warning = False
        for message in err:
            if 'error' in message:
                print_error(message)
                compile_error = True
            elif 'warning' in message:
                print_warning(message)
                compile_warning = True

        if compile_error or compile_warning:
            print("")
            
        if compile_error:
            print(colorama.Fore.RED + "[BUILD FAILED]")
        else:
            print(colorama.Fore.GREEN + "[BUILD SUCCESSFUL]")

    def build_windows(self):
        print(colorama.Fore.BLUE + "[tdbuild] " + colorama.Fore.RESET + "Running from {}".format(os.getcwd()))
        
        self.push("cl.exe")

        for extra in build_options['Windows']['extras']:
            self.push(extra)

        if build_options['cpp']:
            self.push("/TP")
            self.push("/std:c++{}".format(build_options['cpp_standard']))
        else:
            self.push("/TC")

        if build_options['debug']:
            self.push("-Zi")

        for warning in build_options['Windows']['warnings']:
            self.push("/wd{}".format(warning))

        self.push("/" + build_options['Windows']['runtime_library'])

        for source_file in build_options['source_files']:
            full_source_file_path = os.path.join(build_options['source_dir'], source_file)
            self.push(quote(full_source_file_path))

        for include_dir in build_options['include_dirs']:
            self.push('/I{}'.format(quote(include_dir)))

        self.push("/link")
        for system_lib in build_options['Windows']['system_libs']:
            self.push(system_lib)

        for user_lib in build_options['Windows']['user_libs']:
            self.push(quote(os.path.join(build_options['lib_dir'], user_lib)))
            
        for ignore in build_options['Windows']['ignore']:
            self.push("/ignore:" + ignore)

        if 'machine' in build_options['Windows']:
            self.push("/MACHINE:{}".format(build_options['Windows']['machine']))

        self.push("/out:" + build_options['Windows']['out'])


        make_cd_build_dir()
        
        print_info("Generated compiler command:")
        print_info(self.build_cmd)
        print_info("Invoking the compiler")
        print("")
        
        # @hack: is there a better way to keep a process open?
        process = subprocess.Popen("{} && {}".format(os.path.join("..", "setup_devenv.bat"), self.build_cmd), stdout=subprocess.PIPE)
        compiler_messages, err = process.communicate()
        compiler_messages = compiler_messages.decode('UTF-8').split('\n')
        
        compile_error = False
        compile_warning = False
        for message in compiler_messages:
            if 'error' in message:
                print(colorama.Fore.RED + "[tdbuild] " + colorama.Fore.RESET + message)
                compile_error = True
            elif 'warning' in message:
                print(colorama.Fore.YELLOW + "[tdbuild] " + colorama.Fore.RESET + message)
                compile_warning = True

        os.chdir("..")

        if compile_error or compile_warning:
            print("")
            
        if compile_error:
            print(colorama.Fore.RED + "[BUILD FAILED]")
        else:
            print(colorama.Fore.GREEN + "[BUILD SUCCESSFUL]")

        
    def run(self):
        os.chdir(os.path.dirname(os.path.realpath(__file__)))
        subprocess.run([os.path.join(os.getcwd(), build_options['build_dir'], build_options[platform.system()]['out'])]) 
                 
    def setup(self):
        print_info("...running setup")
        computer = input("What is the name of this computer? ")
        font_dir = input("Where are your fonts stored? ")
        default_font = input("What's your default font? ")

        with open('src/scripts/init.skeleton', 'r') as init_skeleton:
            contents = init_skeleton.readlines()
            for i, line in enumerate(contents):
                if NEW_FONT_CONFIG in line:
                    del contents[i]
                    contents.insert(i, "\t" + computer + " = {\n")
                    contents.insert(i + 1, '\t\tfont_dir = "{}"\n'.format(font_dir))
                    contents.insert(i + 2, '\t\tdefault = "{}"\n'.format(default_font))
                    contents.insert(i + 3, "\t}\n")
                    break

            with open('src/scripts/__init__.tds', 'w+') as init_out:
                init_out.write("".join(contents))

        project_path = os.path.dirname(os.path.realpath(__file__))
        project_path = os.path.join(project_path, "") # add a trailing slash
        with open('src/machine_conf.hpp', "w+") as machine_cpp:
            lines = [
                'string root_dir = "{}";\n'.format(project_path),
                'string computer_id = "{}";'.format(computer)
            ]
            machine_cpp.writelines(lines)

        with open('src/scripts/machine_conf.py', 'w+') as machine_py:
            machine_py.write('GLOBAL_PROJ_ROOT_DIR = "{}"'.format(project_path))



if __name__ == "__main__":
    builder = tdbuild()
    
    if len(sys.argv) is 1 or sys.argv[1] == "build":
        builder.build()
    elif sys.argv[1] == "run":
        builder.run()
    # Some hacky bullshit to get VS to open my executable in the right directory
    elif sys.argv[1] == "debug":
        # Make sure that this script runs from the project root
        os.chdir(os.path.dirname(os.path.realpath(__file__)))

        copied_files = []
        for fname in os.listdir(builder.build_dir):
            fname = os.path.join(os.getcwd(), builder.build_dir, fname)
            if not os.path.isdir(fname):
                shutil.copy2(fname, ".")
                copied_files.append(fname)
                
        subprocess.call(["devenv.exe", builder.out])

        for fname in copied_files:
            os.remove(fname)
    elif sys.argv[1] == "setup":
        os.chdir(os.path.dirname(os.path.realpath(__file__)))

        builder.setup()



