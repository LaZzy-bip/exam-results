#include <stdio.h>
#include "db.h"

int main(void)
{
    sqlite3 *db = NULL;

    if (db_open(&db, "exams.db") != SQLITE_OK) {
        return 1;
    }
    if (db_init_from_file(db, "db/schema.sql") != SQLITE_OK) {
        db_close(db);
        return 1;
    }
    printf("База данных создана и заполнена тестовыми данными.\n\n");

    db_close(db);
    return 0;
}