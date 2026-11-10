#include "group_repository.h"
#include "../db.h"

static void map_row(sqlite3_stmt *st, void *item)
{
    StudentGroup *g = item;
    g->group_id = sqlite3_column_int(st, 0);
    db_column_text(st, 1, g->name, sizeof g->name);
}

int group_repository_create(const char *name)
{
    sqlite3_stmt *st;
    if (db_prepare("INSERT INTO student_groups (name) VALUES (?)", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, name);
    return db_insert(st);
}

int group_repository_get_by_id(int group_id, StudentGroup *out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT group_id, name FROM student_groups WHERE group_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, group_id);
    return db_fetch_one(st, out, map_row);
}

int group_repository_get_by_name(const char *name, StudentGroup *out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT group_id, name FROM student_groups WHERE name = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, name);
    return db_fetch_one(st, out, map_row);
}

int group_repository_get_all(StudentGroup **out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT group_id, name FROM student_groups ORDER BY name", &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int group_repository_update(int group_id, const char *name)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE student_groups SET name = ? WHERE group_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, name);
    sqlite3_bind_int(st, 2, group_id);
    return db_execute(st);
}

int group_repository_delete(int group_id)
{
    sqlite3_stmt *st;
    if (db_prepare("DELETE FROM student_groups WHERE group_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, group_id);
    return db_execute(st);
}
