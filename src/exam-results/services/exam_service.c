#include "exam_service.h"
#include "service_error.h"
#include "catalog_service.h"
#include "../repositories/exam_repository.h"
#include "../repositories/exam_result_repository.h"
#include "../repositories/teacher_repository.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int is_valid_date(const char *date)
{
    if (date == NULL || strlen(date) != 10 || date[4] != '-' || date[7] != '-') {
        return 0;
    }
    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && !isdigit((unsigned char)date[i])) {
            return 0;
        }
    }

    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);
    if (year < 2000 || year > 2100 || month < 1 || month > 12 || day < 1) {
        return 0;
    }

    static const int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int max_day = days_in_month[month - 1];
    int leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && leap) {
        max_day = 29;
    }
    return day <= max_day;
}

int create_exam(int subject_id, int group_id, int teacher_id, const char *exam_date, char *err)
{
    Subject subject;
    StudentGroup group;
    Teacher teacher;

    if (get_subject(subject_id, &subject, err) != SERVICE_OK) return SERVICE_ERROR;
    if (get_group(group_id, &group, err) != SERVICE_OK) return SERVICE_ERROR;

    int found = teacher_repository_get_by_id(teacher_id, &teacher);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Преподаватель не найден");

    if (!is_valid_date(exam_date)) {
        return service_fail(err, "Неверная дата. Формат: ГГГГ-ММ-ДД, например 2027-01-21");
    }

    /* у группы не может быть двух экзаменов в один день */
    Exam *exams = NULL;
    int n = exam_repository_get_by_group(group_id, &exams);
    if (n < 0) {
        return service_fail(err, "Ошибка базы данных");
    }
    for (int i = 0; i < n; i++) {
        if (strcmp(exams[i].exam_date, exam_date) == 0) {
            free(exams);
            return service_fail(err, "У этой группы уже есть экзамен в этот день");
        }
    }
    free(exams);

    if (exam_repository_create(subject_id, teacher_id, group_id, exam_date) < 0) {
        return service_fail(err, "Не удалось сохранить экзамен");
    }
    return SERVICE_OK;
}

int get_all_exams(Exam **out)
{
    return exam_repository_get_all(out);
}

int get_exams_by_teacher(int teacher_id, Exam **out)
{
    return exam_repository_get_by_teacher(teacher_id, out);
}

int get_exams_by_group(int group_id, Exam **out)
{
    return exam_repository_get_by_group(group_id, out);
}

int get_exam(int exam_id, Exam *out, char *err)
{
    int found = exam_repository_get_by_id(exam_id, out);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Экзамен не найден");
    return SERVICE_OK;
}

int get_teacher_exam(int teacher_id, int exam_id, Exam *out, char *err)
{
    if (get_exam(exam_id, out, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }
    if (out->teacher_id != teacher_id) {
        return service_fail(err, "Это экзамен другого преподавателя");
    }
    return SERVICE_OK;
}

int count_exam_results(int exam_id)
{
    return exam_result_repository_count_by_exam(exam_id);
}

int delete_exam(int exam_id, char *err)
{
    Exam exam;
    if (get_exam(exam_id, &exam, err) != SERVICE_OK) {
        return SERVICE_ERROR;
    }
    if (exam_repository_delete(exam_id) != 0) {
        return service_fail(err, "Не удалось удалить экзамен");
    }
    return SERVICE_OK;
}
