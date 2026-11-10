#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

/* Включает UTF-8 в консоли Windows. */
void console_init(void);

/*
 * Печатает приглашение и читает строку. Перевод строки и пробелы по краям удаляются.
 * Возвращает 0, если ввод закончился (Ctrl+Z / Ctrl+D), иначе 1.
 */
int read_line(const char *prompt, char *buf, size_t size);

/*
 * Читает целое число. Возвращает 1 — число прочитано, 0 — пустой ввод или не число
 * (в этом случае выводится подсказка, если строка не пустая).
 */
int read_int(const char *prompt, int *out);

/* Вопрос «да/нет»: 1 — пользователь ответил «д» / «y». */
int confirm(const char *question);

/* Ячейка таблицы шириной width символов (длина считается в символах, а не в байтах). */
void print_cell(const char *text, int width);
void print_cell_int(int value, int width);

/* Горизонтальная линия из width символов «-». */
void print_rule(int width);

/* Заголовок меню и сообщения */
void print_title(const char *title);
void print_error(const char *message);
void print_success(const char *message);

/* Текстовое представление оценки */
const char *grade_text(int grade);   /* «5 (отлично)», «не явился», «не выставлена» */
const char *grade_short(int grade);  /* «5», «н/я», «—» */

#endif
