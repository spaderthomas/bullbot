import sqlite3
c = sqlite3.connect('data.db')

def last_insert_id():
    # Returns a list, which contains a single tuple. The first element
    # of the tuple is the ID we want
    return str(c.execute('''select last_insert_rowid()''').fetchone()[0])

# Constructs a query to add new label. 
def insert_label_sql(pokemon):
    sql = 'insert into labels '

    # Each column names a Pokemon
    columns = '('
    for name in pokemon:
        columns += name + ', '
    columns += 'sample)'
    sql += columns + ' values '

    # And we only have to fill in the values for present Pokemon
    values = '('
    for name in pokemon:
        values += '1, '
    values += last_insert_id() + ')'
    sql += values
    
    return sql;

# Constructs query to add new sample
def insert_data_sql(pokemon):
    sql = 'insert into data '

    # Each column names a Pokemon
    columns = '('
    for name in pokemon:
        columns += name + ', '
    columns = columns[:-2] # Trailing comma and space
    columns += ')'
    sql += columns + ' values '

    # And we only have to fill in the values for present Pokemon
    values = '('
    for name in pokemon:
        values += '1, '
    values = values[:-2] # Trailing comma and space
    values += ')'
    sql += values

    return sql;



# Both sample and label should be a list of Pokemon names
def insert(sample, label):
    sql = insert_data_sql(sample)
    c.execute(sql)
    c.commit()

    sql = insert_label_sql(label)
    print(sql)
    c.execute(sql)
    c.commit()

if __name__ == "__main__":
    insert(["alakazam", "chansey"], ["zapdos"])
    c.close()


