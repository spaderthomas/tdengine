import os
import re
import pprint

pp = pprint.PrettyPrinter(indent=2)


print("---Generating entity tree")

script_root_dir = "C:/Users/dboon/programming/tdengine/src/scripts"
project_root_dir = "C:/Users/dboon/programming/tdengine/src/"
entity_table = "entity."
defs = {}
already_defined = []

# Parse out all the definitions into defs
def recursive_add(full_parent_path):
    global defs

    # e.g. scripts/entities/characters/...
    
    for name in os.listdir(full_parent_path):
        # Because 'name' isn't a full path, make sure to check the full path
        full_child_path = full_parent_path + "/" + name

        # If it's a file, parse for definitions
        if os.path.isfile(full_child_path):
            # Check it's actually a Lua file
            should_be_lua = full_child_path[-4:]
            if should_be_lua != ".lua":
                continue
            just_script_dir_path = full_parent_path.replace(project_root_dir, "")
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

with open(project_root_dir + "/script_to_def.hpp", "w") as f:
    f.write(warning + signature + table_code)