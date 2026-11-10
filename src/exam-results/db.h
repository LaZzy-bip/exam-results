#ifndef DB_H
#define DB_H

#include <stddef.h>
#include "sqlite3.h"

/* Открывает файл базы данных и включает проверку внешних ключей. */
int db_connect(const char *path);

/* Текущее подключение (аналог get_connection() из методички). */
sqlite3 *get_connection(void);

/* Закрывает подключение. */
void db_disconnect(void);

/* 1, если таблицы уже созданы (в базе есть таблица users). */
int db_has_schema(void);

/* Выполняет SQL-скрипт (schema.sql): создаёт таблицы и тестовые данные. */
int db_init_from_file(const char *schema_path);

/* Транзакции: несколько запросов выполняются целиком или не выполняются вовсе. */
int db_begin(void);
int db_commit(void);
void db_rollback(void);

/* Копирует текстовый столбец результата запроса в буфер (NULL -> ""). */
void db_column_text(sqlite3_stmt *stmt, int col, char *dst, size_t size);

/* Печатает сообщение об ошибке SQLite. */
void db_print_error(const char *where);

/* Подготавливает SQL-запрос; при ошибке печатает её и возвращает не SQLITE_OK. */
int db_prepare(const char *sql, sqlite3_stmt **stmt);

/*
 * Функция преобразования строки результата в модель (аналог _map из методички).
 * item указывает на структуру нужного типа (User, Exam, ...).
 */
typedef void (*db_row_mapper)(sqlite3_stmt *stmt, void *item);

/*
 * Читает все строки результата запроса в динамический массив моделей.
 * Возвращает количество строк (массив *out освобождает вызывающий через free())
 * или -1 при ошибке. Запрос stmt финализируется внутри.
 */
int db_fetch_all(sqlite3_stmt *stmt, void **out, size_t item_size, db_row_mapper map);

/*
 * Читает одну строку результата в модель item.
 * Возвращает 1 — запись найдена, 0 — не найдена, -1 — ошибка. stmt финализируется.
 */
int db_fetch_one(sqlite3_stmt *stmt, void *item, db_row_mapper map);

/* Выполняет INSERT и возвращает id новой записи или -1. stmt финализируется. */
int db_insert(sqlite3_stmt *stmt);

/* Выполняет INSERT/UPDATE/DELETE. Возвращает 0 или -1. stmt финализируется. */
int db_execute(sqlite3_stmt *stmt);

/* Привязка текстового параметра (символ ? в запросе), нумерация с 1. */
void db_bind_text(sqlite3_stmt *stmt, int index, const char *value);

#endif /* DB_H */
