import sqlite3
c = sqlite3.connect('tdengine.db')

c.execute('''drop table if exists levels''')
c.execute('''drop table if exists tiles''')
c.execute('''drop table if exists entities''')
c.execute('''drop table if exists position_components''')
c.commit()

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
c.commit()
c.close()
