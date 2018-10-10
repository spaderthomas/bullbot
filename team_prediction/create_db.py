import sqlite3
c = sqlite3.connect('data.db')

c.execute('''drop table data''')
c.execute('''drop table labels''')
c.commit()

c.execute('''create table data(
               snorlax integer default 0,
               chansey integer default 0,
               tauros integer default 0,
               exeggutor integer default 0,
               alakazam integer default 0,
               starmie integer default 0,
               zapdos integer default 0,
               rhydon integer default 0,
               lapras integer default 0,
               jynx integer default 0,
               slowbro integer default 0,
               jolteon integer default 0,
               gengar integer default 0,
               cloyster integer default 0,
               victreebel integer default 0,
               golem integer default 0,
               persian integer default 0
             )''')

c.execute('''create table labels(
               snorlax integer default 0,
               chansey integer default 0,
               tauros integer default 0,
               exeggutor integer default 0,
               alakazam integer default 0,
               starmie integer default 0,
               zapdos integer default 0,
               rhydon integer default 0,
               lapras integer default 0,
               jynx integer default 0,
               slowbro integer default 0,
               jolteon integer default 0,
               gengar integer default 0,
               cloyster integer default 0,
               victreebel integer default 0,
               golem integer default 0,
               persian integer default 0,
               sample integer not null,
               foreign key(sample) references data(rowid)
             )''')
c.commit()
c.close()


