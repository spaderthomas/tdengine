import sqlite3
from argparse import ArgumentParser


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("-i", dest='init', action='store_const', const=True, default=False)
    parser.add_argument("-u", dest='update', action='store_const', const=True, default=False)
    args = parser.parse_args()
    
    c = sqlite3.connect('tdengine.db')

    cursor = c.execute('''PRAGMA table_info(entity_state)''')
    for row in cursor:
        print(row)

    # Clean out the database and remake the tables
    if args.init:
        # Kill all the old tables
        c.execute('''drop table if exists levels''')
        c.execute('''drop table if exists tiles''')
        c.execute('''drop table if exists entities''')
        c.execute('''drop table if exists position_components''')
        c.execute('''drop table if exists state''')
        c.execute('''drop table if exists entity_state''')
        c.commit()

        # Recreate them
        c.execute('''create table levels(
                       name text not null
                     )''')
        
        c.execute('''create table entities(
                       lua_id  text,
                       game_id integer,
                       level   integer,
                         foreign key(level) references levels(rowid)
                     )''')
        
        c.execute('''create table position_components(
                       x      integer,
                       y      integer,
                       entity integer,
                         foreign key(entity) references entiites(rowid)
                     )''')

        # 0: False
        # 1: True
        # 2: Unitialized
        c.execute('''create table state(
                       name   text,
                       value  integer default 2
                     )''')
        
        c.execute('''create table entity_state(
                       name   text,
                       state  text
                     )''')
        
        c.commit()

    # If you just need to update the database, do it here
    if args.update:
        c.execute('''insert into state values('a', 0)''')
        c.execute('''insert into entity_state values('wilson', 'intro')''')
        c.commit()
        
    c.close()
