#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

void console_init(void);

int read_line(const char *prompt, char *buf, size_t size);

int read_int(const char *prompt, int *out);

int confirm(const char *question);

void print_cell(const char *text, int width);
void print_cell_int(int value, int width);

void print_rule(int width);

void print_title(const char *title);
void print_error(const char *message);
void print_success(const char *message);

const char *grade_text(int grade);
const char *grade_short(int grade);

#endif
