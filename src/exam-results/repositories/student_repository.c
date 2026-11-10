#include "student_repository.h"
#include "../db.h"

#define SELECT_STUDENT \
    "SELECT s.student_id, s.user_id, s.group_id, s.record_book, u.full_name, g.name " \
    "FROM students s " \
    "JOIN users u ON u.user_id = s.user_id " \
    "JOIN student_groups g ON g.group_id = s.group_id "

static void map_row(sqlite3_stmt *st, void *item)
{
    Student *s = item;
    s->student_id = sqlite3_column_int(st, 0);
    s->user_id = sqlite3_column_int(st, 1);
    s->group_id = sqlite3_column_int(st, 2);
    db_column_text(st, 3, s->record_book, sizeof s->record_book);
    db_column_text(st, 4, s->full_name, sizeof s->full_name);
    db_column_text(st, 5, s->group_name, sizeof s->group_name);
}

int student_repository_create(int user_id, int group_id, const char *record_book)
{
    sqlite3_stmt *st;
    if (db_prepare("INSERT INTO students (user_id, group_id, record_book) VALUES (?, ?, ?)",
                   &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, user_id);
    sqlite3_bind_int(st, 2, group_id);
    db_bind_text(st, 3, record_book);
    return db_insert(st);
}

int student_repository_get_by_id(int student_id, Student *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_STUDENT "WHERE s.student_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, student_id);
    return db_fetch_one(st, out, map_row);
}

int student_repository_get_by_user_id(int user_id, Student *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_STUDENT "WHERE s.user_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, user_id);
    return db_fetch_one(st, out, map_row);
}

int student_repository_get_by_record_book(const char *record_book, Student *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_STUDENT "WHERE s.record_book = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, record_book);
    return db_fetch_one(st, out, map_row);
}

int student_repository_get_all(Student **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_STUDENT "ORDER BY g.name, u.full_name", &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int student_repository_get_by_group(int group_id, Student **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_STUDENT "WHERE s.group_id = ? ORDER BY u.full_name", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, group_id);
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int student_repository_update(int student_id, int group_id, const char *record_book)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE students SET group_id = ?, record_book = ? WHERE student_id = ?",
                   &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, group_id);
    db_bind_text(st, 2, record_book);
    sqlite3_bind_int(st, 3, student_id);
    return db_execute(st);
}

int student_repository_delete(int student_id)
{
    sqlite3_stmt *st;
    if (db_prepare("DELETE FROM students WHERE student_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, student_id);
    return db_execute(st);
}
