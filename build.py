import os, subprocess, sys, re, shutil, platform, colorama

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))

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
            'glfw3'
        ],
        'system_libs':[
            'z',
            'GL',
            'X11',
            'c',
            'dl',
            'stdc++fs'
        ],
        'extras': [
            '-pthread'
        ],
        'out': 'tdengine'
    }
}

# Utility variables and functions
NEW_FONT_CONFIG = '# NEW_FONT_CONFIG'

def make_cd_build_dir():
    build_dir = os.path.join(os.getcwd(), build_options['build_dir'])
    try:
        os.mkdir(build_dir)
    except:
        pass
    os.chdir(build_dir)

def print_info(message):
    print(colorama.Fore.BLUE + "[info] " + colorama.Fore.RESET + message)

def print_error(message):
    print(colorama.Fore.RED + "[error] " + colorama.Fore.RESET + message)

def print_warning(message):
    print(colorama.Fore.YELLOW + "[warning] " + colorama.Fore.RESET + message)
    
def print_success(message):
    print(colorama.Fore.GREEN + "[success] " + colorama.Fore.RESET + message)
    
def print_prebuild(message):
    print(colorama.Fore.CYAN + "[prebuild] " + colorama.Fore.RESET + message)
    
def quote(string):
    return '"{}"'.format(string)

def trailing_slash(path):
    return os.path.join(path, "")

def actually_has_message(maybe_messages):
    # @hack. Sometimes I'll get a list containing one empty string.
    # It makes my output look ugly, so I hack around it. 
    for maybe_message in maybe_messages:
        if len(maybe_message):
            return True

    return False

# Base builder class. Reads in configurations, generates a compiler command, executes it. Provides
# 'virtual' methods you can override. 
class tdbuild():
    def __init__(self):        
        self.build_cmd = ""
        self.unix_args = []

    def push(self, item):
        self.build_cmd = self.build_cmd + item + " "

    def prebuild(self):
        print_warning("Calling prebuild, but no prebuild step was defined.")
        
    def build(self):
        print_info("Running from {}".format(os.getcwd()))
        if platform.system() == 'Windows':
            self.build_windows()
        elif platform.system() == 'Darwin':
            self.build_mac()
        elif platform.system() == 'Linux':
            self.build_linux()

    def build_linux(self):
        # Find the path to the compiler using 'which'
        compiler = build_options['Linux']['compiler']
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

        for extra in build_options['Linux']['extras']:
            self.push(extra)

        for source in build_options['source_files']:
            self.push(os.path.join(build_options['source_dir'], source))

        for include in build_options['include_dirs']:
            self.push("-I" + include)
            
        self.push('-o ' + build_options['Linux']['out'])

        for lib in build_options['Linux']['user_libs']:
            self.push('-l{}'.format(lib))

        for lib in build_options['Linux']['system_libs']:
            self.push('-l' + lib)

        
        print_info("Generated compiler command:")
        print(self.build_cmd)
        print_info("Running compiler command...")

        make_cd_build_dir()

        process = subprocess.Popen(self.build_cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        compiler_messages, err = process.communicate()
        compiler_messages = compiler_messages.decode('UTF-8').split('\n')
        err = err.decode('UTF-8').split('\n')

        if actually_has_message(compiler_messages):
            print_info("Compiler messages:")
            for message in compiler_messages:
                print_info(message)

        compile_error = False
        compile_warning = False
        if actually_has_message(err):
            print_info("Compiler warnings + errors:")
            
            for message in err:
                if message == '':
                    continue
                elif 'error' in message:
                    print_error(message)
                    compile_error = True
                    # Maybe printing everything else as a warning is Not Smart, but our heuristic for whether a line is an error is...lacking.
                else:
                    print_warning(message)
                    compile_warning = True


            if compile_error or compile_warning:
                print("")
            
        if compile_error:
            print(colorama.Fore.RED + "[BUILD FAILED]")
        else:
            print(colorama.Fore.GREEN + "[BUILD SUCCESSFUL]")

    def build_mac(self):
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
        print(self.build_cmd)
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
        imgui_font_size = input("What's your preferred GUI font size? ")

        new_skellington = []
        with open('src/scripts/init.skeleton', 'r') as init_skeleton:
            contents = init_skeleton.readlines()
            for i, line in enumerate(contents):
                if NEW_FONT_CONFIG in line:
                    # Kill the old comment
                    del contents[i]

                    # Add the new table entry
                    contents.insert(i, "\t" + computer + " = {\n")
                    contents.insert(i + 1, '\t\timgui_font_size = "{}"\n'.format(imgui_font_size))

                    # Write it to where the real init file goes
                    with open('src/scripts/__init__.tds', 'w+') as init_out:
                        init_out.write("".join(contents))
                        print_info("Created configuration file at src/scripts/__init__.tds")

                    # Update the skellerton to have this config so we can commit to source control
                    contents.insert(i + 4, "\t" + NEW_FONT_CONFIG + "\n")
                    new_skellington = contents
                    break

        with open('src/scripts/init.skeleton', 'w') as init_skeleton:
            init_skeleton.write("".join(new_skellington))


        project_path = os.path.dirname(os.path.realpath(__file__))
        project_path = os.path.join(project_path, "") # add a trailing slash
        with open('src/machine_conf.hpp', "w+") as machine_cpp:
            lines = [
                'string root_dir = "{}";\n'.format(project_path),
                'string computer_id = "{}";'.format(computer)
            ]
            machine_cpp.writelines(lines)
            print_info("Created src/machine_conf.hpp")


        with open('src/scripts/machine_conf.py', 'w+') as machine_py:
            machine_py.write('GLOBAL_PROJ_ROOT_DIR = "{}"'.format(project_path))
            print_info("Created src/scripts/machine_conf.py")

# All of the custom tdengine stuff goes here. 
class TDEngineBuilder(tdbuild):
    def __init__(self):
        super().__init__()

    def build(self):
        super().build()
        
    def run(self):
        super().run()
        
    def setup(self):
        super().setup()
        
    def prebuild(self):
        self.generate_tiles()
        self.generate_entity_tree()
        
    def generate_tiles(self):
        WARNING = '''# THIS FILE WAS AUTOGENERATED BY generate_tiles.py
        '''
        
        print_prebuild("generate_tiles: Generating tile tdscript definitions")

        tiles_dir = os.path.join(PROJECT_ROOT, 'textures', 'src', 'tiles')

        tiles = []
        for root, dirs, files in os.walk(tiles_dir):
            for maybe_png in files:
                if maybe_png.endswith('.png'):
                    tile = maybe_png[:-4]
                    tiles.append(tile)

    
        with open("src/scripts/utils/tiles.tds", "w") as f:
            f.write(WARNING);
            for i, tile in enumerate(tiles):
                path = tile + '.png'
                tds = '''
entity.%(tile_name)s = {}
entity.%(tile_name)s.components = {}
entity.%(tile_name)s.components.Graphic_Component = {
    Animations = {
        %(tile_name)s = {
            "%(tile_name)s.png"
        }
    }
    default_animation = "%(tile_name)s"
    z = 0
}
entity.%(tile_name)s.components.Position_Component = {}
entity.%(tile_name)s.components.Tile_Component = {}

''' % {'tile_name' : tile}
            
                f.write(tds)
                
    def generate_entity_tree(self):
        print_prebuild("generate_entity_tree(): Generating entity tree")

        script_root_dir = os.path.join(PROJECT_ROOT, 'src', 'scripts')
        scripts_dir = os.path.join(PROJECT_ROOT,  'src', 'scripts')
        entity_table = "entity."
        defs = {}
        already_defined = []

        # Parse out all the definitions into defs
        def recursive_add(full_parent_path):
            # e.g. scripts/entities/characters/...
            for name in os.listdir(full_parent_path):
                # Because 'name' isn't a full path, make sure to check the full path
                full_child_path = full_parent_path + "/" + name

                # If it's a file, parse for definitions
                if os.path.isfile(full_child_path):
                    # Check it's actually a TDS file
                    should_be_tds = full_child_path[-4:]
                    if should_be_tds != ".tds":
                        continue
                    just_script_dir_path = full_parent_path.replace(script_root_dir + "/", "")
                    just_script_file_path = just_script_dir_path + "/" + name
                    defs[just_script_file_path] = []

                    
                    f = open(full_child_path, "r")
                    text = f.read()
                    text = re.split(' |\n', text)
                    for word in text:
                        # Check for the existence of entity.NAME_HERE.blah
                        if entity_table in word:
                            # Parse out entity. and .blah ...
                            entity_name = word.replace(entity_table, "")
                            entity_name = entity_name.split(".")[0]
                            # This string could appear multiple places -- only do it once
                            if entity_name not in already_defined:
                                defs[just_script_file_path].append(entity_name)
                                already_defined.append(entity_name)

                # If it's a directory, recurse
                elif os.path.isdir(full_child_path):
                    recursive_add(full_child_path)
                    

        recursive_add(script_root_dir)

        # Put it in a C++ header
        warning = '''// THIS FILE IS AUTOGENERATED BY entity_tree.py

        '''
        signature = "unordered_map<string, vector<string>> script_to_entity = {\n"
        table_code = ""
        for key, val in defs.items():
            # Pass over all the empty keys we created
            if not len(val):
                continue
            
            table_code += "\t{\n" # Opening brace for one entry of map
            table_code += "\t\t\"" + key + "\",\n" # Key for map and opening brace for inner map
            table_code += "\t\t{"
            
            for i, item in enumerate(val):
                table_code += "\"" + item + "\""
                if i is len(val) - 1:
                    table_code += "}\n"
                else:
                    table_code += ", "

            table_code += "\t},\n"

        table_code = table_code[:-2] # Remove last newline and comma
        table_code += "\n};"

        #with open(scripts_dir + "/entity_file_map.tds", "w") as f:
        #    f.write(warning + signature + table_code)
            

# Main to let you invoke through the command line. 
if __name__ == "__main__":
    builder = TDEngineBuilder()
    
    if len(sys.argv) is 1 or sys.argv[1] == "build":
        builder.prebuild()
        builder.build()
    elif sys.argv[1] == "prebuild":
        builder.prebuild()
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



