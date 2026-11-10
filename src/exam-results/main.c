/*
 * ИС «Учёт результатов экзаменов студентов» — точка входа.
 *
 * Слои приложения:
 *   ui/           — консольный интерфейс (меню, ввод и вывод)
 *   services/     — бизнес-логика (проверки, права доступа, расчёты)
 *   repositories/ — доступ к данным (все SQL-запросы)
 *   models/       — модели данных (структуры, соответствующие таблицам)
 *   db.c          — подключение к SQLite
 *
 * Запуск из корня репозитория:
 *   exam-results.exe           — открыть базу (создаётся при первом запуске)
 *   exam-results.exe --reset   — пересоздать базу из schema.sql с тестовыми данными
 */
#include <stdio.h>
#include <string.h>

#include "db.h"
#include "models/user_role.h"
#include "ui/console.h"
#include "ui/auth.h"
#include "ui/admin_menu.h"
#include "ui/teacher_menu.h"
#include "ui/student_menu.h"

#define DB_PATH     "exams.db"
#define SCHEMA_PATH "src/db/schema.sql"

static int prepare_database(int reset)
{
    if (db_connect(DB_PATH) != SQLITE_OK) {
        return 0;
    }
    if (reset || !db_has_schema()) {
        if (db_init_from_file(SCHEMA_PATH) != SQLITE_OK) {
            return 0;
        }
        printf("База данных создана и заполнена тестовыми данными.\n");
    }
    return 1;
}

int main(int argc, char *argv[])
{
    console_init();

    int reset = (argc > 1 && strcmp(argv[1], "--reset") == 0);
    if (!prepare_database(reset)) {
        db_disconnect();
        return 1;
    }

    char choice[16];
    while (1) {
        print_title("ИС «Учёт результатов экзаменов»");
        printf("1. Войти\n");
        printf("0. Выход из программы\n");
        if (!read_line("Выберите действие: ", choice, sizeof choice)) break;

        if (strcmp(choice, "0") == 0) {
            break;
        }
        if (strcmp(choice, "1") != 0) {
            print_error("Нет такого пункта меню");
            continue;
        }

        User user;
        if (!login(&user)) {
            continue;
        }

        /* разграничение доступа: каждая роль получает своё меню */
        switch (user.user_role) {
        case ROLE_ADMIN:   admin_menu(&user);   break;
        case ROLE_TEACHER: teacher_menu(&user); break;
        case ROLE_STUDENT: student_menu(&user); break;
        default: print_error("Неизвестная роль пользователя");
        }
    }

    printf("До свидания!\n");
    db_disconnect();
    return 0;
}
