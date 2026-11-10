#include "exam_result_repository.h"
#include "../db.h"

/*
 * Общая часть запроса: строка ведомости вместе с ФИО студента, группой,
 * дисциплиной, датой и преподавателем (служебные поля модели ExamResult).
 */
#define SELECT_RESULT \
    "SELECT r.result_id, r.exam_id, r.student_id, r.grade, " \
    "       u.full_name, s.record_book, g.name, sub.name, e.exam_date, tu.full_name " \
    "FROM exam_results r " \
    "JOIN exams e ON e.exam_id = r.exam_id " \
    "JOIN students s ON s.student_id = r.student_id " \
    "JOIN users u ON u.user_id = s.user_id " \
    "JOIN student_groups g ON g.group_id = s.group_id " \
    "JOIN subjects sub ON sub.subject_id = e.subject_id " \
    "JOIN teachers t ON t.teacher_id = e.teacher_id " \
    "JOIN users tu ON tu.user_id = t.user_id "

static void map_row(sqlite3_stmt *st, void *item)
{
    ExamResult *r = item;
    if (sqlite3_column_type(st, 0) == SQLITE_NULL) {
        r->result_id = 0;                     /* строки в exam_results нет */
        r->grade = GRADE_NONE;
    } else {
        r->result_id = sqlite3_column_int(st, 0);
        r->grade = (sqlite3_column_type(st, 3) == SQLITE_NULL)
                   ? GRADE_ABSENT
                   : sqlite3_column_int(st, 3);
    }
    r->exam_id = sqlite3_column_int(st, 1);
    r->student_id = sqlite3_column_int(st, 2);
    db_column_text(st, 4, r->student_name, sizeof r->student_name);
    db_column_text(st, 5, r->record_book, sizeof r->record_book);
    db_column_text(st, 6, r->group_name, sizeof r->group_name);
    db_column_text(st, 7, r->subject_name, sizeof r->subject_name);
    db_column_text(st, 8, r->exam_date, sizeof r->exam_date);
    db_column_text(st, 9, r->teacher_name, sizeof r->teacher_name);
}

static void bind_grade(sqlite3_stmt *st, int index, int grade)
{
    if (grade == GRADE_ABSENT) {
        sqlite3_bind_null(st, index);
    } else {
        sqlite3_bind_int(st, index, grade);
    }
}

int exam_result_repository_create(int exam_id, int student_id, int grade)
{
    sqlite3_stmt *st;
    if (db_prepare("INSERT INTO exam_results (exam_id, student_id, grade) VALUES (?, ?, ?)",
                   &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, exam_id);
    sqlite3_bind_int(st, 2, student_id);
    bind_grade(st, 3, grade);
    return db_insert(st);
}

int exam_result_repository_get_by_id(int result_id, ExamResult *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_RESULT "WHERE r.result_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, result_id);
    return db_fetch_one(st, out, map_row);
}

int exam_result_repository_get_by_exam_and_student(int exam_id, int student_id, ExamResult *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_RESULT "WHERE r.exam_id = ? AND r.student_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, exam_id);
    sqlite3_bind_int(st, 2, student_id);
    return db_fetch_one(st, out, map_row);
}

int exam_result_repository_get_all(ExamResult **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_RESULT "ORDER BY g.name, u.full_name, e.exam_date", &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_result_repository_get_by_student(int student_id, ExamResult **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_RESULT "WHERE r.student_id = ? ORDER BY e.exam_date", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, student_id);
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_result_repository_get_by_group(int group_id, ExamResult **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_RESULT "WHERE e.group_id = ? ORDER BY e.exam_date, sub.name, u.full_name",
                   &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, group_id);
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_result_repository_search(int teacher_id, int student_id, int group_id,
                                  int subject_id, ExamResult **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_RESULT
                   "WHERE e.teacher_id = ?1 "
                   "  AND (?2 = 0 OR r.student_id = ?2) "
                   "  AND (?3 = 0 OR e.group_id = ?3) "
                   "  AND (?4 = 0 OR e.subject_id = ?4) "
                   "ORDER BY e.exam_date, sub.name, u.full_name", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, teacher_id);
    sqlite3_bind_int(st, 2, student_id);
    sqlite3_bind_int(st, 3, group_id);
    sqlite3_bind_int(st, 4, subject_id);
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_result_repository_get_statement(int exam_id, ExamResult **out)
{
    sqlite3_stmt *st;
    /* LEFT JOIN: студент попадает в ведомость, даже если строки с оценкой ещё нет */
    if (db_prepare(
            "SELECT r.result_id, e.exam_id, s.student_id, r.grade, "
            "       u.full_name, s.record_book, g.name, sub.name, e.exam_date, tu.full_name "
            "FROM exams e "
            "JOIN students s ON s.group_id = e.group_id "
            "JOIN users u ON u.user_id = s.user_id "
            "JOIN student_groups g ON g.group_id = s.group_id "
            "JOIN subjects sub ON sub.subject_id = e.subject_id "
            "JOIN teachers t ON t.teacher_id = e.teacher_id "
            "JOIN users tu ON tu.user_id = t.user_id "
            "LEFT JOIN exam_results r ON r.exam_id = e.exam_id AND r.student_id = s.student_id "
            "WHERE e.exam_id = ? "
            "ORDER BY u.full_name", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, exam_id);
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int exam_result_repository_count_by_exam(int exam_id)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT COUNT(*) FROM exam_results WHERE exam_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, exam_id);
    int count = -1;
    if (sqlite3_step(st) == SQLITE_ROW) {
        count = sqlite3_column_int(st, 0);
    } else {
        db_print_error("подсчёт оценок");
    }
    sqlite3_finalize(st);
    return count;
}

int exam_result_repository_update_grade(int result_id, int grade)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE exam_results SET grade = ? WHERE result_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    bind_grade(st, 1, grade);
    sqlite3_bind_int(st, 2, result_id);
    return db_execute(st);
}

int exam_result_repository_delete(int result_id)
{
    sqlite3_stmt *st;
    if (db_prepare("DELETE FROM exam_results WHERE result_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, result_id);
    return db_execute(st);
}
