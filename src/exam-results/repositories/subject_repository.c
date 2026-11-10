#include "subject_repository.h"
#include "../db.h"

static void map_row(sqlite3_stmt *st, void *item)
{
    Subject *s = item;
    s->subject_id = sqlite3_column_int(st, 0);
    db_column_text(st, 1, s->name, sizeof s->name);
}

int subject_repository_create(const char *name)
{
    sqlite3_stmt *st;
    if (db_prepare("INSERT INTO subjects (name) VALUES (?)", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, name);
    return db_insert(st);
}

int subject_repository_get_by_id(int subject_id, Subject *out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT subject_id, name FROM subjects WHERE subject_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, subject_id);
    return db_fetch_one(st, out, map_row);
}

int subject_repository_get_by_name(const char *name, Subject *out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT subject_id, name FROM subjects WHERE name = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, name);
    return db_fetch_one(st, out, map_row);
}

int subject_repository_get_all(Subject **out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT subject_id, name FROM subjects ORDER BY name", &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int subject_repository_update(int subject_id, const char *name)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE subjects SET name = ? WHERE subject_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, name);
    sqlite3_bind_int(st, 2, subject_id);
    return db_execute(st);
}

int subject_repository_delete(int subject_id)
{
    sqlite3_stmt *st;
    if (db_prepare("DELETE FROM subjects WHERE subject_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, subject_id);
    return db_execute(st);
}
