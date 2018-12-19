import sqlite3

c = sqlite3.connect('tdengine.db')

c.execute('''update entity_state set state = 'intro_unchecked' where name = 'boonhouse_door';''')
c.execute('''update entity_state set state = 'intro' where name = 'intro_police';''')
c.execute('''update state set value = 0 where name = 'intro_door_answered';''')
c.commit()
c.close()
