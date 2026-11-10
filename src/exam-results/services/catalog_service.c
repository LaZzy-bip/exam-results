#include "catalog_service.h"
#include "service_error.h"
#include "../common/str_utils.h"
#include "../repositories/subject_repository.h"
#include "../repositories/group_repository.h"

#include <stdlib.h>
#include <string.h>

int create_subject(const char *name, char *err)
{
    if (str_is_blank(name)) {
        return service_fail(err, "Название дисциплины не может быть пустым");
    }
    if (strlen(name) >= SUBJECT_LEN) {
        return service_fail(err, "Слишком длинное название дисциплины");
    }

    Subject *subjects = NULL;
    int n = subject_repository_get_all(&subjects);
    if (n < 0) {
        return service_fail(err, "Ошибка базы данных");
    }
    for (int i = 0; i < n; i++) {
        if (utf8_equal_ci(subjects[i].name, name)) {
            free(subjects);
            return service_fail(err, "Такая дисциплина уже есть");
        }
    }
    free(subjects);

    if (subject_repository_create(name) < 0) {
        return service_fail(err, "Не удалось сохранить дисциплину");
    }
    return SERVICE_OK;
}

int get_all_subjects(Subject **out)
{
    return subject_repository_get_all(out);
}

int get_subject(int subject_id, Subject *out, char *err)
{
    int found = subject_repository_get_by_id(subject_id, out);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Дисциплина не найдена");
    return SERVICE_OK;
}

int create_group(const char *name, char *err)
{
    if (str_is_blank(name)) {
        return service_fail(err, "Название группы не может быть пустым");
    }
    if (strlen(name) >= GROUP_NAME_LEN) {
        return service_fail(err, "Слишком длинное название группы");
    }

    StudentGroup *groups = NULL;
    int n = group_repository_get_all(&groups);
    if (n < 0) {
        return service_fail(err, "Ошибка базы данных");
    }
    for (int i = 0; i < n; i++) {
        if (utf8_equal_ci(groups[i].name, name)) {
            free(groups);
            return service_fail(err, "Такая группа уже есть");
        }
    }
    free(groups);

    if (group_repository_create(name) < 0) {
        return service_fail(err, "Не удалось сохранить группу");
    }
    return SERVICE_OK;
}

int get_all_groups(StudentGroup **out)
{
    return group_repository_get_all(out);
}

int get_group(int group_id, StudentGroup *out, char *err)
{
    int found = group_repository_get_by_id(group_id, out);
    if (found < 0) return service_fail(err, "Ошибка базы данных");
    if (found == 0) return service_fail(err, "Группа не найдена");
    return SERVICE_OK;
}
