#include "user_repository.h"
#include "../db.h"

#define SELECT_USER \
    "SELECT u.user_id, u.login, u.password_hash, u.full_name, u.user_role, r.name " \
    "FROM users u " \
    "JOIN user_roles r ON r.user_role_id = u.user_role "

static void map_row(sqlite3_stmt *st, void *item)
{
    User *u = item;
    u->user_id = sqlite3_column_int(st, 0);
    db_column_text(st, 1, u->login, sizeof u->login);
    db_column_text(st, 2, u->password_hash, sizeof u->password_hash);
    db_column_text(st, 3, u->full_name, sizeof u->full_name);
    u->user_role = sqlite3_column_int(st, 4);
    db_column_text(st, 5, u->role_name, sizeof u->role_name);
}

int user_repository_create(const char *login, const char *password_hash,
                           const char *full_name, int user_role)
{
    sqlite3_stmt *st;
    if (db_prepare("INSERT INTO users (login, password_hash, full_name, user_role) "
                   "VALUES (?, ?, ?, ?)", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, login);
    db_bind_text(st, 2, password_hash);
    db_bind_text(st, 3, full_name);
    sqlite3_bind_int(st, 4, user_role);
    return db_insert(st);
}

int user_repository_get_by_id(int user_id, User *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_USER "WHERE u.user_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, user_id);
    return db_fetch_one(st, out, map_row);
}

int user_repository_get_by_login(const char *login, User *out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_USER "WHERE u.login = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, login);
    return db_fetch_one(st, out, map_row);
}

int user_repository_get_all(User **out)
{
    sqlite3_stmt *st;
    if (db_prepare(SELECT_USER "ORDER BY u.user_role, u.full_name", &st) != SQLITE_OK) {
        return -1;
    }
    return db_fetch_all(st, (void **)out, sizeof **out, map_row);
}

int user_repository_update(int user_id, const char *login, const char *full_name)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE users SET login = ?, full_name = ? WHERE user_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, login);
    db_bind_text(st, 2, full_name);
    sqlite3_bind_int(st, 3, user_id);
    return db_execute(st);
}

int user_repository_update_password(int user_id, const char *password_hash)
{
    sqlite3_stmt *st;
    if (db_prepare("UPDATE users SET password_hash = ? WHERE user_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    db_bind_text(st, 1, password_hash);
    sqlite3_bind_int(st, 2, user_id);
    return db_execute(st);
}

int user_repository_delete(int user_id)
{
    sqlite3_stmt *st;
    if (db_prepare("DELETE FROM users WHERE user_id = ?", &st) != SQLITE_OK) {
        return -1;
    }
    sqlite3_bind_int(st, 1, user_id);
    return db_execute(st);
}
