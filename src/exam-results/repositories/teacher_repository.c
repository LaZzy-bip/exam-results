#include "teacher_repository.h"
#include "../db.h"

#define SELECT_TEACHER \
    "SELECT t.teacher_id, t.user_id, t.department, u.full_name " \
    "FROM teachers t " \
    "JOIN users u ON u.user_id = t.user_id "

static void map_row(sqlite3_stmt *st, void *item)
{
    Teacher *t = item;
    t->teacher_id = sqlite3_column_int(st, 0);
    t->user_id = sqlite3_column_int(st, 1);
    db_column_text(st, 2, t->department, sizeof t->department);
    db_column_text(st, 3, t->full_name, sizeof t->full_name);
}

int teacher_repository_create(int user_id, const char *department)
{
    sqlite3_stmt *st;
    if (db_prepare("INSERT INTO teachers (user_id, department) VALUES (?, ?)", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, user_id);
    db_bind_text(st, 2, department);
    return db_insert(st);
}

int teacher_repository_get_by_id(int teacher_id, Teacher *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_TEACHER "WHERE t.teacher_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, teacher_id);
    return db_fetch_one(st, out, map_row);
}

int teacher_repository_get_by_user_id(int user_id, Teacher *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_TEACHER "WHERE t.user_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, user_id);
    return db_fetch_one(st, out, map_row);
}

int teacher_repository_get_all(Teacher **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_TEACHER "ORDER BY u.full_name", &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int teacher_repository_update(int teacher_id, const char *department)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE teachers SET department = ? WHERE teacher_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, department);
    sqlite3_bind_int(st, 2, teacher_id);
    return db_execute(st);
}

int teacher_repository_delete(int teacher_id)
{
    sqlite3_stmt *st;
    if (db_prepare("DELETE FROM teachers WHERE teacher_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, teacher_id);
    return db_execute(st);
}
