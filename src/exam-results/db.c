#define _CRT_SECURE_NO_WARNINGS
#include "db.h"
#include "common/str_utils.h"

#include <stdio.h>
#include <stdlib.h>

static sqlite3 *g_db = NULL;

int db_connect(const char *path)
{
    int rc = sqlite3_open(path, &g_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Не удалось открыть БД: %s\n", sqlite3_errmsg(g_db));
        return rc;
    }

    char *err = NULL;
    rc = sqlite3_exec(g_db, "PRAGMA foreign_keys = ON;", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Не удалось включить внешние ключи: %s\n", err);
        sqlite3_free(err);
    }
    return rc;
}

sqlite3 *get_connection(void)
{
    return g_db;
}

void db_disconnect(void)
{
    sqlite3_close(g_db);
    g_db = NULL;
}

int db_has_schema(void)
{
    sqlite3_stmt *st;
    int found = 0;
    if (sqlite3_prepare_v2(g_db,
            "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = 'users'",
            -1, &st, NULL) == SQLITE_OK) {
        found = (sqlite3_step(st) == SQLITE_ROW);
    }
    sqlite3_finalize(st);
    return found;
}

static char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Не удалось открыть файл %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc((size_t)size + 1);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }

    size_t n = fread(buf, 1, (size_t)size, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

int db_init_from_file(const char *schema_path)
{
    char *sql = read_file(schema_path);
    if (sql == NULL) {
        return SQLITE_ERROR;
    }

    char *err = NULL;
    int rc = sqlite3_exec(g_db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Ошибка в %s: %s\n", schema_path, err);
        sqlite3_free(err);
    }

    free(sql);
    return rc;
}

int db_begin(void)
{
    return sqlite3_exec(g_db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

int db_commit(void)
{
    return sqlite3_exec(g_db, "COMMIT;", NULL, NULL, NULL);
}

void db_rollback(void)
{
    sqlite3_exec(g_db, "ROLLBACK;", NULL, NULL, NULL);
}

void db_column_text(sqlite3_stmt *stmt, int col, char *dst, size_t size)
{
    str_copy(dst, size, (const char *)sqlite3_column_text(stmt, col));
}

void db_print_error(const char *where)
{
    fprintf(stderr, "[Ошибка БД] %s: %s\n", where, sqlite3_errmsg(g_db));
}

int db_prepare(const char *sql, sqlite3_stmt **stmt)
{
    int rc = sqlite3_prepare_v2(g_db, sql, -1, stmt, NULL);
    if (rc != SQLITE_OK) {
        db_print_error("подготовка запроса");
    }
    return rc;
}

int db_fetch_all(sqlite3_stmt *stmt, void **out, size_t item_size, db_row_mapper map)
{
    char *items = NULL;
    int count = 0;
    int capacity = 0;
    int rc;

    *out = NULL;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (count == capacity) {
            capacity = capacity ? capacity * 2 : 8;
            char *grown = realloc(items, (size_t)capacity * item_size);
            if (grown == NULL) {
                free(items);
                sqlite3_finalize(stmt);
                return -1;
            }
            items = grown;
        }
        map(stmt, items + (size_t)count * item_size);
        count++;
    }

    if (rc != SQLITE_DONE) {
        db_print_error("чтение данных");
        free(items);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    *out = items;
    return count;
}

int db_fetch_one(sqlite3_stmt *stmt, void *item, db_row_mapper map)
{
    int rc = sqlite3_step(stmt);
    int result;
    if (rc == SQLITE_ROW) {
        map(stmt, item);
        result = 1;
    } else if (rc == SQLITE_DONE) {
        result = 0;
    } else {
        db_print_error("чтение записи");
        result = -1;
    }
    sqlite3_finalize(stmt);
    return result;
}

int db_execute(sqlite3_stmt *stmt)
{
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("изменение данных");
    }
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

void db_bind_text(sqlite3_stmt *stmt, int index, const char *value)
{
    sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
}

int db_insert(sqlite3_stmt *stmt)
{
    if (db_execute(stmt) != 0) {
        return -1;
    }
    return (int)sqlite3_last_insert_rowid(g_db);
}
