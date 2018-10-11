import sqlite3
c = sqlite3.connect('data.db')

def last_insert_id():
    # Returns a list, which contains a single tuple. The first element
    # of the tuple is the ID we want
    return str(c.execute('''select last_insert_rowid()''').fetchone()[0])

def already_have_battle(battle_id):
    sql = '''select id 
             from battle_ids b 
             where 
               b.id = %s'''%str(battle_id)
    res = c.execute(sql).fetchone()
    return res != None
    
# Constructs a query to add new label. 
def _insert_label_sql(pokemon, battle_id):
    sql = 'insert into labels '

    # Each column names a Pokemon
    columns = '('
    for name in pokemon:
        columns += name + ', '
    columns += 'battle_id)'
    sql += columns + ' values '

    # And we only have to fill in the values for present Pokemon
    values = '('
    for name in pokemon:
        values += '1, '
    values += str(battle_id)
    values += ')'
    sql += values
    
    return sql

# Constructs query to add new sample
def _insert_sample_sql(pokemon, rowid):
    sql = 'insert into samples '

    # Each column names a Pokemon
    columns = '('
    for name in pokemon:
        columns += name + ', '
    columns += 'label)'
    sql += columns + ' values '

    # Fill in a 1 for present Pokemon
    values = '('
    for name in pokemon:
        values += '1, '

    # Fill in the foreign key to the label
    values += rowid + ')'
    sql += values

    return sql

def _insert_battle_sql(battle_id):
    return '''insert into battle_ids (id) select %s
                where not exists (select id from battle_ids where id=%s)
              '''%(str(battle_id), str(battle_id))



def insert(samples, label, battle_id):
    # Make an entry in the battle table
    sql = _insert_battle_sql(battle_id)
    c.execute(sql)
    c.commit()

    # Make an entry in the label table, linking to the battle
    sql = _insert_label_sql(label, battle_id)
    c.execute(sql)
    c.commit()
    rowid = last_insert_id()

    # Construct a bunch of samples, linking to the label
    for sample in samples:
        sql = _insert_sample_sql(sample, rowid)
        print(sql)
        c.execute(sql)
        c.commit()

        
if __name__ == "__main__":
    samples = [["alakazam", "chansey"], ["alakazam", "chansey"], ["alakazam", "chansey"]]
    label = ["alakazam", "chansey", "golem"]
    battle_id = 1234
    
    insert(samples, label, battle_id)

    c.close()


