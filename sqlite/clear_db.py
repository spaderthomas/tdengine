import sqlite3
c = sqlite3.connect('tdengine.db')

c.execute('''drop table if exists levels''')
c.execute('''drop table if exists tiles''')
c.execute('''drop table if exists entities''')
c.execute('''drop table if exists position_components''')
c.commit()
