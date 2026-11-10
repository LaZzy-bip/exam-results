#ifndef DB_H
#define DB_H

#include <stddef.h>
#include "sqlite3.h"

int db_connect(const char *path);

sqlite3 *get_connection(void);

void db_disconnect(void);

int db_has_schema(void);

int db_init_from_file(const char *schema_path);

int db_begin(void);
int db_commit(void);
void db_rollback(void);

void db_column_text(sqlite3_stmt *stmt, int col, char *dst, size_t size);

void db_print_error(const char *where);

int db_prepare(const char *sql, sqlite3_stmt **stmt);

typedef void (*db_row_mapper)(sqlite3_stmt *stmt, void *item);

int db_fetch_all(sqlite3_stmt *stmt, void **out, size_t item_size, db_row_mapper map);

int db_fetch_one(sqlite3_stmt *stmt, void *item, db_row_mapper map);

int db_insert(sqlite3_stmt *stmt);

int db_execute(sqlite3_stmt *stmt);

void db_bind_text(sqlite3_stmt *stmt, int index, const char *value);

#endif
