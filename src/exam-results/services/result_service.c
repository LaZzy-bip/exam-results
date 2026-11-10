#include "result_service.h"
#include "service_error.h"
#include "exam_service.h"
#include "catalog_service.h"
#include "../common/str_utils.h"
#include "../models/exam.h"
#include "../models/student.h"
#include "../repositories/exam_result_repository.h"
#include "../repositories/student_repository.h"

#include <stdlib.h>

/* ---------- бизнес-правила ---------- */

int is_valid_grade(int grade)
{
    return grade == GRADE_ABSENT || (grade >= 2 && grade <= 5);
}

int is_debt(const ExamResult *result)
{
    return result->grade == 2 || result->grade == GRADE_ABSENT;
}

int calc_average(const ExamResult *results, int n, double *average)
{
    int sum = 0;
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (results[i].grade >= 2) {        /* «не явился» и «не выставлена» пропускаем */
            sum += results[i].grade;
            count++;
        }
    }
    *average = count ? (double)sum / count : 0.0;
    return count;
}

/* Оставляет в массиве только задолженности; возвращает их количество. */
static int keep_debts(ExamResult *results, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (is_debt(&results[i])) {
            results[k++] = results[i];
        }
    }
    return k;
}

/* ---------- выставление оценок ---------- */

/* Общие проверки: экзамен принадлежит преподавателю, студент из группы экзамена, оценка верна */
static int check_grade_request(int teacher_id, int exam_id, int student_id, int grade, char *err)
{
    Exam exam;
    if (get_teacher_exam(teacher_id, exam_id, &exam, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }

    Student student;
    int found = student_repository_get_by_id(student_id, &student);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Студент не найден");
    if (student.group_id != exam.group_id) {
        return service_fail(err, "Студент не из той группы, которая сдаёт этот экзамен");
    }

    if (!is_valid_grade(grade)) {
        return service_fail(err, "Оценка должна быть 2, 3, 4, 5 или «не явился»");
    }
    return SERVICE_OK;
}

int set_grade(int teacher_id, int exam_id, int student_id, int grade, char *err)
{
    if (check_grade_request(teacher_id, exam_id, student_id, grade, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }

    /* повторно выставить оценку за тот же экзамен нельзя — только изменить */
    ExamResult existing;
    int found = exam_result_repository_get_by_exam_and_student(exam_id, student_id, &existing);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 1) {
        return service_fail(err, "Оценка уже выставлена. Чтобы исправить её, выберите «Изменить оценку»");
    }

    if (exam_result_repository_create(exam_id, student_id, grade) < 0) {
        return service_fail(err, "Не удалось сохранить оценку");
    }
    return SERVICE_OK;
}

int update_grade(int teacher_id, int exam_id, int student_id, int grade, char *err)
{
    if (check_grade_request(teacher_id, exam_id, student_id, grade, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }

    ExamResult existing;
    int found = exam_result_repository_get_by_exam_and_student(exam_id, student_id, &existing);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) {
        return service_fail(err, "Оценка ещё не выставлена. Выберите «Выставить оценку»");
    }

    if (exam_result_repository_update_grade(existing.result_id, grade) != 0) {
        return service_fail(err, "Не удалось изменить оценку");
    }
    return SERVICE_OK;
}

/* ---------- ведомости и поиск ---------- */

int get_exam_statement(int teacher_id, int exam_id, ExamResult **out, char *err)
{
    Exam exam;
    int rc = teacher_id ? get_teacher_exam(teacher_id, exam_id, &exam, err)
                        : get_exam(exam_id, &exam, err);
    if (rc != SERVICE_OK) {
        return -1;
    }

    int n = exam_result_repository_get_statement(exam_id, out);
    if (n < 0) {
        service_fail(err, "Ошибка базы данных");
    }
    return n;
}

int search_results(int teacher_id, const char *record_book, int group_id, int subject_id,
                   ExamResult **out, char *err)
{
    int student_id = 0;

    if (!str_is_blank(record_book)) {
        Student student;
        int found = student_repository_get_by_record_book(record_book, &student);
        if (found < 0) return service_fail(err, "Ошибка базы данных");
        if (found == 0) return service_fail(err, "Студент с такой зачётной книжкой не найден");
        student_id = student.student_id;
    }

    int n = exam_result_repository_search(teacher_id, student_id, group_id, subject_id, out);
    if (n < 0) {
        service_fail(err, "Ошибка базы данных");
    }
    return n;
}

/* ---------- студент ---------- */

int get_student_results(int student_id, ExamResult **out)
{
    return exam_result_repository_get_by_student(student_id, out);
}

int get_student_debts(int student_id, ExamResult **out)
{
    int n = exam_result_repository_get_by_student(student_id, out);
    return n < 0 ? -1 : keep_debts(*out, n);
}

/* ---------- администратор ---------- */

int get_debts(int group_id, ExamResult **out)
{
    int n = group_id ? exam_result_repository_get_by_group(group_id, out)
                     : exam_result_repository_get_all(out);
    return n < 0 ? -1 : keep_debts(*out, n);
}

int get_group_summary(int group_id, ExamResult **out, char *err)
{
    StudentGroup group;
    if (get_group(group_id, &group, err) != SERVICE_OK) {
        return -1;
    }
    int n = exam_result_repository_get_by_group(group_id, out);
    if (n < 0) {
        service_fail(err, "Ошибка базы данных");
    }
    return n;
}

int get_student_average(int student_id, double *average, int *count, char *err)
{
    ExamResult *results = NULL;
    int n = exam_result_repository_get_by_student(student_id, &results);
    if (n < 0) {
        return service_fail(err, "Ошибка базы данных");
    }
    *count = calc_average(results, n, average);
    free(results);
    return SERVICE_OK;
}

int get_group_average(int group_id, double *average, int *count, char *err)
{
    ExamResult *results = NULL;
    int n = get_group_summary(group_id, &results, err);
    if (n < 0) {
        return SERVICE_ERROR;
    }
    *count = calc_average(results, n, average);
    free(results);
    return SERVICE_OK;
}
