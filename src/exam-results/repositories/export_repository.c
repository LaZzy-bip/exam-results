#include "export_repository.h"
#include "../db.h"

#define EXPORT_STUDENTS_SQL \
    "SELECT s.student_id, " \
    "       s.record_book, " \
    "       u.user_id   AS \"user.user_id\", " \
    "       u.login     AS \"user.login\", " \
    "       u.full_name AS \"user.full_name\", " \
    "       g.group_id  AS \"group.group_id\", " \
    "       g.name      AS \"group.name\" " \
    "FROM students s " \
    "JOIN users u ON u.user_id = s.user_id " \
    "JOIN student_groups g ON g.group_id = s.group_id " \
    "ORDER BY s.student_id"

int export_repository_get_students(DataTable *out)
{
    sqlite3_stmt *st;
    if (db_prepare(EXPORT_STUDENTS_SQL, &st) != SQLITE_OK) {
        return -1;
    }
    return data_table_load(st, out);
}
