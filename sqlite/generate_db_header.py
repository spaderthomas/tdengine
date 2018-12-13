import sqlite3
import os

print("---Generating SQL column definitions")

# Make sure to run this script in the root project directory
old_cwd = os.getcwd()
os.chdir('C:/Users/dboon/programming/tdengine')

c = sqlite3.connect('sqlite/tdengine.db')

def last_index(index, mylen):
    return index == (mylen - 1)

signature = "unordered_map<const char*, unordered_map<const char*, int>> db_schema = {\n"

cursor = c.execute("SELECT name FROM sqlite_master WHERE type='table';")
rows = cursor.fetchall()

# The size of the inner one is tricky.
# We have to make them all the size of the largest one, and pad out smaller ones with dummies
max_inner_size = 0
for table_row in rows:
    table_name = table_row[0]
    cursor = c.execute('select * from {}'.format(table_name))
    column_names = list(map(lambda x: x[0], cursor.description))
    max_inner_size = max(max_inner_size, len(column_names))
signature = signature.replace("INNER_SIZE", str(max_inner_size))

warning = '''// WARNING: THIS FILE WAS GENERATED BY generate_db_header.py

'''
table_code = ""

for i, table_row in enumerate(rows):
    # Produces e.g.
    # {
    #     "levels", 
    table_name = table_row[0]
    cursor = c.execute('select * from {}'.format(table_name))
    column_names = list(map(lambda x: x[0], cursor.description))
    table_cpp = "\t{\n" # Start the outer table
    table_cpp += "\t\t\"{}\"".format(table_name) # Add the table as the key
    table_cpp += ", {\n" # Start the inner table

    # Produces e.g. {"name", 0}, {"dog", 1}
    for j in range(len(column_names)):
        table_cpp += "\t\t\t{" # Start one element of the inner table 
        table_cpp += "\"{}\", ".format(column_names[j]) # Add in the name
        table_cpp += "{}".format(j) # Add the mapping to the row index
        table_cpp += "}" # End one element of the inner table
        
        if not last_index(j, len(column_names)):
            table_cpp += ","

        table_cpp += "\n"
        j = j+1

    table_cpp += "\t\t}\n" # Closing bracket for one table's entry
    table_cpp += "\t}"
    if not last_index(i, len(rows)):
        table_cpp += ","
    table_cpp += "\n"
    
    table_code += table_cpp

table_code += "};" # End brace for the outer map
header = warning + signature + table_code
with open("src/db.hpp", "w") as f:
    f.write(header)

os.chdir(old_cwd)
c.close()
