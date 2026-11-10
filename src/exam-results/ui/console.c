#define _CRT_SECURE_NO_WARNINGS
#include "console.h"
#include "../common/str_utils.h"
#include "../models/exam_result.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void console_init(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setvbuf(stdout, NULL, _IOFBF, 8192);
}

static int read_line_from_file(char *buf, size_t size)
{
    if (fgets(buf, (int)size, stdin) == NULL) {
        return 0;
    }
    if (strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
    str_trim(buf);
    return 1;
}

int read_line(const char *prompt, char *buf, size_t size)
{
    if (prompt != NULL) {
        printf("%s", prompt);
    }
    fflush(stdout);
    buf[0] = '\0';

    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    if (!GetConsoleMode(in, &mode)) {
        return read_line_from_file(buf, size);
    }

    wchar_t wbuf[512];
    DWORD chars_read = 0;
    if (!ReadConsoleW(in, wbuf, 511, &chars_read, NULL) || chars_read == 0) {
        return 0;
    }
    wbuf[chars_read] = L'\0';
    if (wbuf[0] == 26) {
        return 0;
    }
    WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf, (int)size, NULL, NULL);
    buf[size - 1] = '\0';
    str_trim(buf);
    return 1;
}

int read_int(const char *prompt, int *out)
{
    char buf[64];
    if (!read_line(prompt, buf, sizeof buf) || buf[0] == '\0') {
        return 0;
    }
    char *end;
    long value = strtol(buf, &end, 10);
    if (*end != '\0') {
        print_error("Нужно ввести число");
        return 0;
    }
    *out = (int)value;
    return 1;
}

int confirm(const char *question)
{
    char buf[16];
    printf("%s (д/н): ", question);
    if (!read_line(NULL, buf, sizeof buf)) {
        return 0;
    }
    return strcmp(buf, "д") == 0 || strcmp(buf, "Д") == 0
        || strcmp(buf, "y") == 0 || strcmp(buf, "Y") == 0
        || strcmp(buf, "да") == 0;
}

void print_cell(const char *text, int width)
{
    size_t len = utf8_length(text);
    if ((int)len <= width) {
        printf("%s", text);
        for (int i = (int)len; i < width; i++) {
            putchar(' ');
        }
    } else {
        int chars = 0;
        const char *p = text;
        while (*p && chars < width - 1) {
            p++;
            while (((unsigned char)*p & 0xC0) == 0x80) {
                p++;
            }
            chars++;
        }
        printf("%.*s…", (int)(p - text), text);
    }
    printf(" ");
}

void print_cell_int(int value, int width)
{
    char buf[32];
    snprintf(buf, sizeof buf, "%d", value);
    print_cell(buf, width);
}

void print_rule(int width)
{
    for (int i = 0; i < width; i++) {
        putchar('-');
    }
    putchar('\n');
}

void print_title(const char *title)
{
    printf("\n===== %s =====\n", title);
}

void print_error(const char *message)
{
    printf("[!] %s\n", message);
}

void print_success(const char *message)
{
    printf("[OK] %s\n", message);
}

const char *grade_text(int grade)
{
    switch (grade) {
    case 5: return "5 (отлично)";
    case 4: return "4 (хорошо)";
    case 3: return "3 (удовлетворительно)";
    case 2: return "2 (неудовлетворительно)";
    case GRADE_ABSENT: return "не явился";
    default: return "не выставлена";
    }
}

const char *grade_short(int grade)
{
    switch (grade) {
    case 5: return "5";
    case 4: return "4";
    case 3: return "3";
    case 2: return "2";
    case GRADE_ABSENT: return "н/я";
    default: return "—";
    }
}
