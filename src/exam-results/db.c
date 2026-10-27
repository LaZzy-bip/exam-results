#include "db.h"

#include <stdio.h>
#include <stdlib.h>

int db_open(sqlite3 **db, const char *path)
{
    int rc = sqlite3_open(path, db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Не удалось открыть БД: %s\n", sqlite3_errmsg(*db));
        return rc;
    }

    char *err = NULL;
    rc = sqlite3_exec(*db, "PRAGMA foreign_keys = ON;", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Не удалось включить внешние ключи: %s\n", err);
        sqlite3_free(err);
    }
    return rc;
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

int db_init_from_file(sqlite3 *db, const char *schema_path)
{
    char *sql = read_file(schema_path);
    if (sql == NULL) {
        return SQLITE_ERROR;
    }

    char *err = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Ошибка в %s: %s\n", schema_path, err);
        sqlite3_free(err);
    }

    free(sql);
    return rc;
}

void db_close(sqlite3 *db)
{
    sqlite3_close(db);
}