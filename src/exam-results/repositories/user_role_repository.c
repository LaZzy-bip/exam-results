#include "user_role_repository.h"
#include "../db.h"

static void map_row(sqlite3_stmt *st, void *item)
{
    UserRole *r = item;
    r->user_role_id = sqlite3_column_int(st, 0);
    db_column_text(st, 1, r->name, sizeof r->name);
}

int user_role_repository_get_by_id(int user_role_id, UserRole *out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT user_role_id, name FROM user_roles WHERE user_role_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, user_role_id);
    return db_fetch_one(st, out, map_row);
}

int user_role_repository_get_all(UserRole **out)
{
    sqlite3_stmt *st;
    if (db_prepare("SELECT user_role_id, name FROM user_roles ORDER BY user_role_id", &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}
