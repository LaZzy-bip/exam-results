#ifndef DB_H
#define DB_H

#include "sqlite3.h"

int db_open(sqlite3 **db, const char *path);

int db_init_from_file(sqlite3 *db, const char *schema_path);

void db_close(sqlite3 *db);

#endif /* DB_H */
