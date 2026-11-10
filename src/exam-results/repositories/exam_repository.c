#include "exam_repository.h"
#include "../db.h"

/* Названия дисциплины, группы и ФИО преподавателя подтягиваются через JOIN */
#define SELECT_EXAM \
    "SELECT e.exam_id, e.subject_id, e.teacher_id, e.group_id, e.exam_date, " \
    "       sub.name, tu.full_name, g.name " \
    "FROM exams e " \
    "JOIN subjects sub ON sub.subject_id = e.subject_id " \
    "JOIN teachers t ON t.teacher_id = e.teacher_id " \
    "JOIN users tu ON tu.user_id = t.user_id " \
    "JOIN student_groups g ON g.group_id = e.group_id "

#define ORDER_EXAM "ORDER BY e.exam_date, sub.name"

static void map_row(sqlite3_stmt *st, void *item)
{
    Exam *e = item;
    e->exam_id = sqlite3_column_int(st, 0);
    e->subject_id = sqlite3_column_int(st, 1);
    e->teacher_id = sqlite3_column_int(st, 2);
    e->group_id = sqlite3_column_int(st, 3);
    db_column_text(st, 4, e->exam_date, sizeof e->exam_date);
    db_column_text(st, 5, e->subject_name, sizeof e->subject_name);
    db_column_text(st, 6, e->teacher_name, sizeof e->teacher_name);
    db_column_text(st, 7, e->group_name, sizeof e->group_name);
}

int exam_repository_create(int subject_id, int teacher_id, int group_id, const char *exam_date)
{
    sqlite3_stmt *st;
    if (db_prepare("INSERT INTO exams (subject_id, teacher_id, group_id, exam_date) "
                   "VALUES (?, ?, ?, ?)", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, subject_id);
    sqlite3_bind_int(st, 2, teacher_id);
    sqlite3_bind_int(st, 3, group_id);
    db_bind_text(st, 4, exam_date);
    return db_insert(st);
}

int exam_repository_get_by_id(int exam_id, Exam *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_EXAM "WHERE e.exam_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, exam_id);
    return db_fetch_one(st, out, map_row);
}

int exam_repository_get_all(Exam **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_EXAM ORDER_EXAM, &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_repository_get_by_teacher(int teacher_id, Exam **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_EXAM "WHERE e.teacher_id = ? " ORDER_EXAM, &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, teacher_id);
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_repository_get_by_group(int group_id, Exam **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_EXAM "WHERE e.group_id = ? " ORDER_EXAM, &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, group_id);
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_repository_update(int exam_id, int teacher_id, const char *exam_date)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE exams SET teacher_id = ?, exam_date = ? WHERE exam_id = ?",
                   &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, teacher_id);
    db_bind_text(st, 2, exam_date);
    sqlite3_bind_int(st, 3, exam_id);
    return db_execute(st);
}

int exam_repository_delete(int exam_id)
{
    sqlite3_stmt *st;
    if (db_prepare("DELETE FROM exams WHERE exam_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, exam_id);
    return db_execute(st);
}
