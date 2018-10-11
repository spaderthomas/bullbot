import sqlite3
c = sqlite3.connect('data.db')

c.execute('''drop table if exists battle_ids''')
c.execute('''drop table if exists samples''')
c.execute('''drop table if exists labels''')
c.commit()

c.execute('''create table battle_ids(
               id integer not null,
               primary key(id)
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
               articuno integer default 0,
               dragonite integer default 0,
               hypno integer default 0,
               battle_id integer not null,
                 foreign key(battle_id) references battle_ids(id)
             )''')

c.execute('''create table samples(
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
               articuno integer default 0,
               dragonite integer default 0,
               hypno integer default 0,
               label integer not null,
                 foreign key(label) references labels(rowid)
             )''')


c.commit()
c.close()


