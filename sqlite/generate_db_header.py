import sqlite3
c = sqlite3.connect('../../sqlite/tdengine.db')
#c = sqlite3.connect('tdengine.db')

def last_index(index, mylen):
    return index == (mylen - 1)

signature = "constexpr frozen::unordered_map<frozen::string, const frozen::unordered_map<frozen::string, int, INNER_SIZE>, OUTER_SIZE> db_schema = {\n"

cursor = c.execute("SELECT name FROM sqlite_master WHERE type='table';")
rows = cursor.fetchall()

# Because of the unordered_map library, the size must be in the template
# The size of the outer one is easy: How many tables are there
signature = signature.replace("OUTER_SIZE", str(len(rows)))

# The size of the inner one is tricky.
# We have to make them all the size of the largest one, and pad out smaller ones with dummies
max_inner_size = 0
for table_row in rows:
    table_name = table_row[0]
    cursor = c.execute('select * from {}'.format(table_name))
    column_names = list(map(lambda x: x[0], cursor.description))
    max_inner_size = max(max_inner_size, len(column_names))
signature = signature.replace("INNER_SIZE", str(max_inner_size))


table_code = ""
for i, table_row in enumerate(rows):
    table_name = table_row[0]
    cursor = c.execute('select * from {}'.format(table_name))
    column_names = list(map(lambda x: x[0], cursor.description))
    table_cpp = "\t{\n" # Start the outer table
    table_cpp += "\t\t\"{}\"".format(table_name) # Add the table as the key
    table_cpp += ", {" # Start the inner table

    j = 0
    while j < max_inner_size:
        name_or_dummy = None
        if j < len(column_names):
            name_or_dummy = column_names[j]
        else:
            name_or_dummy = "dummy" + str(j)
        # Inner loop produces e.g. {"name", 0}, {"dog", 1}
        table_cpp += "{" # Start one element of the inner table 
        table_cpp += "\"{}\", ".format(name_or_dummy) # Add in the name
        table_cpp += "{}".format(j) # Add the mapping to the row index
        table_cpp += "}" # End one element of the inner table
        
        if not last_index(j, max_inner_size):
            table_cpp += ", "
            
        j = j+1
    
    table_cpp += "}\n" # End the inner table
    table_cpp += "\t}"
    if not last_index(i, len(rows)):
        table_cpp += ","
    table_cpp += "\n"
    
    table_code += table_cpp

table_code += "};" # End brace for the outer map
header = signature + table_code
with open("../../src/db.hpp", "w") as f:
    f.write(header)
